# encoding=utf-8
# !/usr/bin/env python

import maya.cmds as cmds
import maya.mel as mel

import functools
import pxr
from pxr import Sdf
from pxr import Usd
from pxr import UsdUtils
from pxr import UsdMaya

def variantSetsChangeCommmand(unused, omg, node, variantSetName):
    #print 'variantSetsChangeCommmand'
    val = cmds.optionMenuGrp(omg, q=True, value=True)
    variantAttr = 'usdVariantSet_%s'%variantSetName
    if not cmds.attributeQuery(variantAttr, node=node, exists=True):
        cmds.addAttr(node, ln=variantAttr, dt='string', internalSet=True)
    cmds.setAttr('%s.%s'%(node, variantAttr), val, type='string')

    resolvedVariant = ''
    usdPrim = UsdMaya.GetPrim(node)
    if usdPrim:
        variantSet = usdPrim.GetVariantSet(variantSetName)
        if variantSet:
            resolvedVariant = variantSet.GetVariantSelection()
    cmds.optionMenuGrp(omg, edit=True, extraLabel=resolvedVariant)

    #cmds.refreshEditorTemplates()


def variantSetsReplace(nodeAttr, new):
    #print 'variantSetsReplace'
    origParent = cmds.setParent(q=True)

    frameLayoutName = 'AETN_UsdProxyTemplate_variantSets_Layout'
    if new == True:
        cmds.frameLayout(frameLayoutName, label='VariantSets', collapse=False)
    else:
        cmds.setParent(frameLayoutName)

    # Remove existing children of layout
    children = cmds.frameLayout(frameLayoutName, q=True, childArray=True)
    if children:
        for child in children:
            cmds.deleteUI(child)

    # Calculate some parameters
    node = nodeAttr.split('.', 1)[0]

    # Create variantSetsDict
    from pxr import Usd, UsdUtils
    variantSetsDict = {}
    usdPrim = UsdMaya.GetPrim(node)
    regVarSetNames = [regVarSet.name for regVarSet in UsdUtils.GetRegisteredVariantSets()]

    if usdPrim:
        variantSets         = usdPrim.GetVariantSets()
        variantSetNames     = variantSets.GetNames()
        for variantSetName in variantSetNames:
            if regVarSetNames and (variantSetName not in regVarSetNames):
                continue

            usdVariant = usdPrim.GetVariantSet(variantSetName)
            if not usdVariant:
                continue
            usdVariantChoices = usdVariant.GetVariantNames()
            usdVariantSelection = usdVariant.GetVariantSelection()
            variantSetsDict[variantSetName] = {
                'variants' : usdVariantChoices,
                'selection' : usdVariantSelection,
            }
            # Handle override
            variantAttrName = 'usdVariantSet_%s'%variantSetName
            if cmds.attributeQuery(variantAttrName, node=node, exists=True):
                variantSetPlgVal = cmds.getAttr('%s.%s'%(node, variantAttrName))
                if variantSetPlgVal:
                    variantSetsDict[variantSetName]['override'] = variantSetPlgVal
                variantSetsDict[variantSetName]['settable'] = cmds.getAttr('%s.%s'%(node, variantAttrName), settable=True)

    # Construct the UI from the variantSetsDict
    for variantSetName,variantSetDict in variantSetsDict.items():
        variantResolved = variantSetDict.get('selection', '')
        variantOverride = variantSetDict.get('override', '')
        variantSetChoices = [''] + variantSetDict['variants']
        variantSettable = variantSetDict.get('settable', True)

        omg = cmds.optionMenuGrp(
            label=variantSetName,
            columnWidth=(2,150),
            enable=variantSettable,
            extraLabel=variantResolved,
        )
        for choice in variantSetChoices:
            cmds.menuItem(label=choice)
        try:
            cmds.optionMenuGrp(omg, e=True, value=variantOverride)
        except RuntimeError, e:
            cmds.warning('Invalid choice %r for %r'%(variantOverride, variantSetName))

        cmds.optionMenuGrp(omg, e=True, changeCommand=functools.partial(variantSetsChangeCommmand, omg=omg, node=node, variantSetName=variantSetName))

    # Restore the original parent
    cmds.setParent(origParent)

