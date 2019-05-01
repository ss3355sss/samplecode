#encoding=utf-8
#!/usr/bin/env python
import os
import sys

import maya.cmds as cmds
import maya.mel as mel



#import rfm.rlf as rlf
#import rfm.rlf2maya as rlf2maya

#import TaneEditor.TaneEditor as TaneEditor
#import TaneFrustumCulling.TaneFrustumCullingMain as TaneFrustumCullingMain
import random

from OptionsWnd import OptionsWnd
class ImportZennFeatherWindow(OptionsWnd):
    """A class for a simple window to create polygon primitives"""
    def __init__(self):
        """Initialize base class and override data attributes"""
        OptionsWindow.__init__(self)
        # overridden from base class
        self.title = 'Import ZennFeather to Tane'
        # overridden from base class
        self.actionName = 'Import'

    def setBodyMeshBtnCB(self, *args):
        nodes = cmds.ls(sl=True)
        if len(nodes) > 1 :
            cmds.warning("select one zennFeater body mesh")
            return
        node = nodes[0]
        shape = None
        if cmds.nodeType(node) == 'transform':
            shape = cmds.listRelatives(node, s=True, f=True)[0]

        if not cmds.nodeType(shape):
            cmds.warning("select zennFeater body mesh shape node")
            return
        cmds.textField(self.bodyMeshTf, edit=True, tx=shape)

    def existsInFeaterInstanceList(self, node):
        allItem = cmds.textScrollList(self.fiList, q=True, allItems=True)
        if allItem == None:
            return False

        for item in allItem:
            if item == node:
                return True
        return False

    def addFiBtnCB(self, *args):
        nodes = cmds.ls(sl=True)
        featherInstanceList = []
        for node in nodes:
            if cmds.nodeType(node) == "ZN_FeatherInstance":
                featherInstanceList.append(node)

        for featherInstance in featherInstanceList:
            if not self.existsInFeaterInstanceList(featherInstance):
                cmds.textScrollList(self.fiList, e=True, append=featherInstance)

    def removeFiBtnCB(self,*args):
        selectItems = cmds.textScrollList(self.fiList, q=True, selectItem=True)
        for selectItem in selectItems:
            cmds.textScrollList(self.fiList, e=True, removeItem=selectItem)

    def clearFiBtnCB(self,*args):
        allItem = cmds.textScrollList(self.fiList, q=True, allItems=True)

        if allItem == None:
            return

        for item in allItem:
            cmds.textScrollList(self.fiList, e=True, removeItem=item)

    def displayOptions(self):
        cmds.columnLayout( columnAttach=('both', 5), rowSpacing=10, columnWidth=500 )

        cmds.columnLayout( adjustableColumn=True )
        cmds.text( label='ZENN Feather Body Mesh', font='boldLabelFont', al='left', w=80)
        cmds.setParent('..')

        cmds.rowLayout( nc=2, cw2=(360, 150))
        self.bodyMeshTf     = cmds.textField(w=350)
        self.bodyMeshSetBtn = cmds.button(l='Set', w=100, h=30, align='center', c=self.setBodyMeshBtnCB)
        cmds.setParent('..')

        cmds.text(l="ZENN Feather Instance nodes to be Import", font="boldLabelFont",w=330,al="left")

        cmds.rowLayout( nc=2, cw2=(360, 10))
        self.fiList = cmds.textScrollList( numberOfRows=8, allowMultiSelection=False, w=350)

        cmds.columnLayout(rowSpacing=5)
        self.addFiBtn       = cmds.button(l='Add'    ,w=100, h=30, align='center', c=self.addFiBtnCB)
        self.removeFiBtn    = cmds.button(l='Remove' ,w=100, h=30, align='center', c=self.removeFiBtnCB)
        self.clearFiBtn     = cmds.button(l='Clear'  ,w=100, h=30, align='center', c=self.clearFiBtnCB)
        cmds.setParent('..')

        cmds.setParent('..')

    def applyBtnCmd(self, *args):
        """Overridden from base class"""
        body   = cmds.textField(self.bodyMeshTf, q=True, tx=True)
        fiList = cmds.textScrollList(self.fiList, q=True, allItems=True)
        for fi in fiList:
            cmds.TN_ImportZennFeatherCmd(b=body, n=fi)
class ExportZFeatherDataWnd(OptionsWnd):
    """A class for a simple window to create polygon primitives"""
    def __init__(self):
        """Initialize base class and override data attributes"""
        OptionsWindow.__init__(self)
        # overridden from base class
        self.title = 'Import ZennFeather to Tane'
        # overridden from base class
        self.actionName = 'Import'
    def setBodyMeshBtnCB(self, *args):
        nodes = cmds.ls(sl=True)
        if len(nodes) > 1 :
            cmds.warning("select one zennFeater body mesh")
            return
        node = nodes[0]
        shape = None
        if cmds.nodeType(node) == 'transform':
            shape = cmds.listRelatives(node, s=True, f=True)[0]

        if not cmds.nodeType(shape):
            cmds.warning("select zennFeater body mesh shape node")
            return
        cmds.textField(self.bodyMeshTf, edit=True, tx=shape)
    def existsInFeaterInstanceList(self, node):
        allItem = cmds.textScrollList(self.fiList, q=True, allItems=True)
        if allItem == None:
            return False

        for item in allItem:
            if item == node:
                return True
        return False
    def addFiBtnCB(self, *args):
        nodes = cmds.ls(sl=True)
        featherInstanceList = []
        for node in nodes:
            if cmds.nodeType(node) == "ZN_FeatherInstance":
                featherInstanceList.append(node)

        for featherInstance in featherInstanceList:
            if not self.existsInFeaterInstanceList(featherInstance):
                cmds.textScrollList(self.fiList, e=True, append=featherInstance)
    def removeFiBtnCB(self,*args):
        selectItems = cmds.textScrollList(self.fiList, q=True, selectItem=True)
        for selectItem in selectItems:
            cmds.textScrollList(self.fiList, e=True, removeItem=selectItem)
    def clearFiBtnCB(self,*args):
        allItem = cmds.textScrollList(self.fiList, q=True, allItems=True)

        if allItem == None:
            return

        for item in allItem:
            cmds.textScrollList(self.fiList, e=True, removeItem=item)
    def displayOptions(self):
        cmds.columnLayout( columnAttach=('both', 5), rowSpacing=10, columnWidth=500 )

        cmds.columnLayout( adjustableColumn=True )
        cmds.text( label='ZENN Feather Body Mesh', font='boldLabelFont', al='left', w=80)
        cmds.setParent('..')

        cmds.rowLayout( nc=2, cw2=(360, 150))
        self.bodyMeshTf     = cmds.textField(w=350)
        self.bodyMeshSetBtn = cmds.button(l='Set', w=100, h=30, align='center', c=self.setBodyMeshBtnCB)
        cmds.setParent('..')

        cmds.text(l="ZENN Feather Instance nodes to be Import", font="boldLabelFont",w=330,al="left")

        cmds.rowLayout( nc=2, cw2=(360, 10))
        self.fiList = cmds.textScrollList( numberOfRows=8, allowMultiSelection=False, w=350)

        cmds.columnLayout(rowSpacing=5)
        self.addFiBtn       = cmds.button(l='Add'    ,w=100, h=30, align='center', c=self.addFiBtnCB)
        self.removeFiBtn    = cmds.button(l='Remove' ,w=100, h=30, align='center', c=self.removeFiBtnCB)
        self.clearFiBtn     = cmds.button(l='Clear'  ,w=100, h=30, align='center', c=self.clearFiBtnCB)
        cmds.setParent('..')

        cmds.setParent('..')
    def applyBtnCmd(self, *args):
        """Overridden from base class"""
        body   = cmds.textField(self.bodyMeshTf, q=True, tx=True)
        fiList = cmds.textScrollList(self.fiList, q=True, allItems=True)
        for fi in fiList:
            cmds.TN_ImportZennFeatherCmd(b=body, n=fi)

#sys.path.append('/netapp/backstage/pub/lib/python_lib_pyside')

def GetFPS():
    fps         = 24
    tunit       = cmds.currentUnit( time=True, q=True )
    fpsMap = {
        '2fps': 2,
        '3fps': 3,
        '4fps': 4,
        '5fps': 5,
        '6fps': 6,
        '8fps': 8,
        '10fps': 10,
        '12fps': 12,
        'game': 15,
        '16fps': 2,
        '20fps': 2,
        '23.976fps': 2,
        'film': 24,
        'pal' : 25,
        '29.97fps': 2,
        'ntsc': 30,
        '40fps': 30,
        '47.952fps': 30,
        'show': 48,
        'palf': 50,
        '59.94fps': 50,
        'ntscf' : 60,
        '75fps' : 70,
        '80fps' : 80,
        '100fps' : 100,
        '120fps' : 120,
        '125fps' : 125,
        '150fps' : 150,
        '200fps' : 200,
        '240fps' : 240,
        '250fps' : 250,
        '300fps' : 300,
        '375fps' : 375,
        '400fps' : 400,
        '500fps' : 500,
        '600fps' : 600,
        '750fps' : 750,
        '1200fps' : 1200,
        '1500fps' : 1500,
        '2000fps' : 2000,
        '3000fps' : 3000,
        '6000fps' : 6000,
        '44100fps' : 44100,
        '48000fps' : 48000,
    }
    if fpsMap.has_key(tunit):
        fps = fpsMap[tunit]
    return fps

def LockTrasnform(transform):
    #lock transform of xform
    pass
    #cmds.setAttr('%s.translateX' % transform, l=True)
    #cmds.setAttr('%s.translateY' % transform, l=True)
    #cmds.setAttr('%s.translateZ' % transform, l=True)
    #cmds.setAttr('%s.rotateX' % transform, l=True)
    #cmds.setAttr('%s.rotateY' % transform, l=True)
    #cmds.setAttr('%s.rotateZ' % transform, l=True)
    #cmds.setAttr('%s.scaleX'  % transform, l=True)
    #cmds.setAttr('%s.scaleY'  % transform, l=True)
    #cmds.setAttr('%s.scaleZ'  % transform, l=True)
    #cmds.setAttr('%s.shearXY' % transform, l=True)
    #cmds.setAttr('%s.shearXZ' % transform, l=True)
    #cmds.setAttr('%s.shearYZ' % transform, l=True)

def VisibilitySetting(shape):
    cmds.addAttr(shape, ln='rman__riattr__visibility_camera',       dv=1)
    cmds.addAttr(shape, ln='rman__riattr__visibility_indirect',     dv=1)
    cmds.addAttr(shape, ln='rman__riattr__visibility_transmission', dv=1)

def ConnectShadingGroup2(shape):
    dagSetMembers = cmds.getAttr('initialShadingGroup.dagSetMembers', mi=True)
    if dagSetMembers == None:
        cmds.connectAttr('%s.instObjGroups[0]' % shape, 'initialShadingGroup.dagSetMembers[0]')
    else:
        idx = len(dagSetMembers)
        cmds.connectAttr('%s.instObjGroups[0]' % shape, 'initialShadingGroup.dagSetMembers[%d]' % idx)

def MessageBox(winTitle="Warning!", message="Message Text", icon="warning", button=["OK", "Cancel"], bgColor=[.5, .5, .5]):
    msg = cmds.confirmDialog(title=winTitle,
                             message='%s    ' % message,
                             messageAlign='center',
                             icon=icon,
                             button=button,
                             backgroundColor=bgColor)

    return msg

#----------------------------------------------- Create Tane Network Template
def CreateEnvironmentNetwork():
    nodes = cmds.ls(sl=True, long=True)
    print "####### CreateEnvironmentNetwork #######"
    print "nodes :", nodes
    if not nodes:
        # print 'No body mesh selected, Scatter Type "Ground Plane"'
        msg = MessageBox(winTitle="Information!",
                   message='No body mesh selected.\n',
                   icon = "information",
                   button = ["OK"])
    else:
        #body selected
        success = False
        for node in nodes:
            body = cmds.listRelatives(node, s=True, f=True)
            print "body :", body
            # if type(body) == list:
            for shape in body:
                if shape.endswith("Shape"):
                    dist = cmds.createNode('AETN_EnvironmentMPxNode')
                    taneShape = CreateTaneMPxSurfaceShape()

                    cmds.connectAttr('%s.worldMesh[0]' % shape, '%s.inBody' % dist)

                    cmds.connectAttr('%s.output' % dist, '%s.input' % taneShape)

                    success = True

                    mel.eval('updateAE "%s";' % dist)
                    mel.eval('updateAE "%s";' % taneShape)

                    break
            if success == False:
                dist = cmds.createNode('AETN_EnvironmentMPxNode')
                taneShape = CreateTaneMPxSurfaceShape()

                cmds.connectAttr('%s.worldMesh[0]' % body[0], '%s.inBody' % dist)
                cmds.connectAttr('%s.output' % dist, '%s.input' % taneShape)

                success = True
                mel.eval('updateAE "%s";' % dist)
                mel.eval('updateAE "%s";' % taneShape)
            # elif cmds.nodeType(body) == 'mesh':
            #     # print '%s selected, Scatter Type "Mesh"' % body[0]
            #     dist = cmds.createNode('AETN_EnvironmentMPxNode')
            #     shape = CreateTaneMPxSurfaceShape()
            #
            #     cmds.connectAttr('%s.worldMesh[0]' % body[0], '%s.inBody' % dist)
            #
            #     cmds.connectAttr('%s.output' % dist, '%s.input' % shape)
            #
            #     success = True
            #
            #     mel.eval('updateAE "%s";' % dist)
            #     mel.eval('updateAE "%s";' % shape)

        if not success:
            cmds.warning('Create Environment Tane Network falied')


#----------------------------------------------- Open Tane Source Manger
def OpenTaneSourceManger():
    pass
    #TaneEditor.main()

#----------------------------------------------- TaneMPxSurfaceShape Render Command
def TaneMPxSurfaceShapeRenderCmd():
    defaultRenderGlobals    = 'defaultRenderGlobals'
    renderGlobals           = 'renderManRISGlobals'

    workspace   = cmds.workspace(q=True, rd=True)
    rfmRIBs     = mel.eval('rman getvar rfmRIBs')   #tmp path(relative path)
    STAGE       = mel.eval('rman getvar STAGE')     #STAGE = maya scene file name
    RMSPROJ     = mel.eval('rman getvar RMSPROJ')   #RMSProj
    RMSPROD     = mel.eval('rman getvar RMSPROD')   #RMSProd
    node        = mel.eval('rman ctxGetObject')
    F4          = mel.eval('rman getvar F4')

    sts  = cmds.getAttr('%s.rman__toropt___shutterAngle' % renderGlobals) / 360.0
    mbr  = cmds.getAttr('%s.rman__torattr___motionBlur' % renderGlobals)

    log  = False
    if log:
        print 'workspace = %s'  % workspace
        print 'rfmRIBs = %s'    % rfmRIBs
        print 'STAGE = %s'      % STAGE
        print 'RMSPROD = %s'    % RMSPROD
        print 'RMSPROJ = %s'    % RMSPROJ
        #print 'node = %s' % node
        #print 'cfr  = %s' % cfr
        #print 'sts  = %f' % sts
        #print 'mbr  = %d' % mbr

    path = RMSPROJ + rfmRIBs.replace('$STAGE', STAGE) + '/' + F4 + '/'
    try:
        offset  = 0
        maxx    = cmds.getAttr('%s.maxCorner0' % node) + offset
        maxy    = cmds.getAttr('%s.maxCorner1' % node) + offset
        maxz    = cmds.getAttr('%s.maxCorner2' % node) + offset

        minx    = cmds.getAttr('%s.minCorner0' % node) - offset
        miny    = cmds.getAttr('%s.minCorner1' % node) - offset
        minz    = cmds.getAttr('%s.minCorner2' % node) - offset

        bound               = '%s %s %s %s %s %s' % (minx, maxx, miny, maxy, minz, maxz)
        immediatesubdivide  = cmds.getAttr('%s.immediateSubdivide' % node)
        ribfile             = cmds.TN_TaneMPxSurfaceShapeRenderCmd(p=path, n=node)

        if log:
            print 'ribfile              = %s' % ribfile
            print 'bound                = %s' % bound
            print 'immediatesubdivide   = %d' % immediatesubdivide

        #mel.eval('RiReverseOrientation()')
        mel.eval('RiArchiveRecord("verbatim", "Procedural2 \\"DelayedReadArchive2\\" \\"SimpleBound\\" \\"string filename\\" [\\"%s\\"] \\"float[6] bound\\" [%s] \\"int __immediatesubdivide\\" [%d]\\n");' % (ribfile, bound, immediatesubdivide))
    except:
        cmds.error("TaneMPxSurfaceShapeRenderCmd() failed")

#----------------------------------------------- TaneMPxSurfaceShape Render Command
def TaneMPxSurfaceShapeUsdRenderCmd():
    defaultRenderGlobals    = 'defaultRenderGlobals'
    renderGlobals           = 'renderManRISGlobals'

    workspace    = cmds.workspace(q=True, rd=True)
    MAYA_VER     = cmds.about(version=True)
    RMAN_VER     = mel.eval('rman getversion').split()[0]
    rfmRIBs      = mel.eval('rman getvar rfmRIBs')   #tmp path(relative path)
    STAGE        = mel.eval('rman getvar STAGE')     #STAGE = maya scene file name
    RMSPROJ      = mel.eval('rman getvar RMSPROJ')   #RMSProj
    RMSPROD      = mel.eval('rman getvar RMSPROD')   #RMSProd
    node         = mel.eval('rman ctxGetObject')
    F4           = mel.eval('rman getvar F4')
    frame        = cmds.currentTime(q=True)
    fps          = GetFPS();
    sts          = cmds.getAttr('%s.rman__toropt___shutterAngle' % renderGlobals) / 360.0
    mbr          = cmds.getAttr('%s.rman__torattr___motionBlur' % renderGlobals)

    shutterOpen  = 0.0
    shutterClose = 0.0
    if mbr:
        shutterOpen  = cmds.getAttr('%s.rman__riopt__Camera_shutteropening0' % renderGlobals)
        shutterClose = cmds.getAttr('%s.rman__riopt__Camera_shutteropening1' % renderGlobals)

    fileFormat  = cmds.getAttr('%s.rman__riopt__rib_format' % renderGlobals)

    print 'workspace = %s' % workspace
    print 'rfmRIBs = %s' % rfmRIBs
    print 'STAGE = %s' % STAGE
    print 'RMSPROD = %s' % RMSPROD
    print 'RMSPROJ = %s' % RMSPROJ
    # print 'node = %s' % node
    # print 'cfr  = %s' % cfr
    # print 'sts  = %f' % sts
    # print 'mbr  = %d' % mbr

    outDirectory = RMSPROJ + rfmRIBs.replace('$STAGE', STAGE) + '/' + F4
    print 'outDirectory = %s' % outDirectory
    #try:
    offset  = 0
    maxx    = cmds.getAttr('%s.maxCorner0' % node) + offset
    maxy    = cmds.getAttr('%s.maxCorner1' % node) + offset
    maxz    = cmds.getAttr('%s.maxCorner2' % node) + offset

    minx    = cmds.getAttr('%s.minCorner0' % node) - offset
    miny    = cmds.getAttr('%s.minCorner1' % node) - offset
    minz    = cmds.getAttr('%s.minCorner2' % node) - offset

    bound               = '%s %s %s %s %s %s' % (minx, maxx, miny, maxy, minz, maxz)
    immediatesubdivide  = 2

    usdFile = cmds.TN_ExportTaneUsdCmd(od=outDirectory,
                                       sf=frame,
                                       ef=frame,
                                       ss=sts,
                                       fp=fps,
                                       fm=fileFormat,
                                       n=node
                                       )[0]
    print 'usdFile              = %s' % usdFile
    print 'bound                = %s' % bound
    print 'immediatesubdivide   = %d' % immediatesubdivide

    buildPath   = "/dexter/Cache_DATA/RND/dohyeon/build/Tane2/%s_%s/renderman" % (MAYA_VER, RMAN_VER)
    dsoName     = 'TaneRiProcedural.so'
    dso         = '%s/%s' % (buildPath, dsoName)
    print 'dso                  = %s' % dso
    options =  '--filename %s ' % usdFile
    options += '--frame %s ' % frame
    options += '--fps %s ' % fps
    options += '--shutterOpen %s ' % shutterOpen
    options += '--shutterClose %s ' % shutterClose

    mel.eval('RiAttributeBegin' )
    mel.eval('RiTransformBegin' )
    mel.eval('RiAttribute "procedural" "int immediatesubdivide" 2')
    mel.eval('RiProcedural "DynamicLoad" "%s" %s "%s"' % (dso, bound, options))
    mel.eval('RiTransformEnd' )
    mel.eval('RiAttributeEnd' )
    #except:
    #    cmds.error("TaneMPxSurfaceShapeUsdRenderCmd() failed")

#----------------------------------------------- Bake Tane Cache(*.tane, actually *.mb)
def BakeCache(dir, filename):
    path = '%s/%s' % (dir, filename)
    resultpath = ''
    try:
        resultpath = cmds.file(filename,
                               defaultExtensions=False,
                               options='v=0;',
                               type='mayaBinary',
                               preserveReferences=True,
                               exportSelected=True)
    except:
        cmds.error("BakeCache() failed")
    return resultpath

#----------------------------------------------- Pre-Render Mel
def PreRenderMel():
    print 'TanePreRenderMel'
    defaultRenderGlobals    = 'defaultRenderGlobals'
    renderGlobals           = 'renderManRISGlobals'

    workspace = cmds.workspace(q=True, rd=True)
    rfmRIBs = mel.eval('rman getvar rfmRIBs')   #tmp path(relative path)
    STAGE   = mel.eval('rman getvar STAGE')     #STAGE = maya scene file name
    RMSPROJ = mel.eval('rman getvar RMSPROJ')   #RMSProj
    RMSPROD = mel.eval('rman getvar RMSPROD')   #RMSProd

    node = mel.eval('rman ctxGetObject')
    cfr  = mel.eval('rman getvar F4')
    sts  = cmds.getAttr('%s.rman__toropt___shutterAngle' % renderGlobals) / 360.0
    mbr  = cmds.getAttr('%s.rman__torattr___motionBlur' % renderGlobals)

    print 'workspace = %s' % workspace
    print 'rfmRIBs = %s' % rfmRIBs
    print 'STAGE = %s' % STAGE
    print 'RMSPROD = %s' % RMSPROD
    print 'RMSPROJ = %s' % RMSPROJ

    print 'node = %s' % node
    print 'cfr  = %s' % cfr
    print 'sts  = %f' % sts
    print 'mbr  = %d' % mbr

    cmds.TN_TaneBakeRIBCmd()

#----------------------------------------------- Post-Render Mel
def PostRenderMel():
    print 'TanePostRenderMel'
    defaultRenderGlobals    = 'defaultRenderGlobals'
    renderGlobals           = 'renderManRISGlobals'

    workspace = cmds.workspace(q=True, rd=True)
    rfmRIBs = mel.eval('rman getvar rfmRIBs')   #tmp path(relative path)
    STAGE   = mel.eval('rman getvar STAGE')     #STAGE = maya scene file name
    RMSPROJ = mel.eval('rman getvar RMSPROJ')   #RMSProj
    RMSPROD = mel.eval('rman getvar RMSPROD')   #RMSProd
    node = mel.eval('rman ctxGetObject')
    F4 = mel.eval('rman getvar F4')


    path = RMSPROJ + rfmRIBs.replace('$STAGE', STAGE) + '/' + F4 + '/' + node

    sts  = cmds.getAttr('%s.rman__toropt___shutterAngle' % renderGlobals) / 360.0
    mbr  = cmds.getAttr('%s.rman__torattr___motionBlur' % renderGlobals)


    print 'workspace = %s' % workspace
    print 'rfmRIBs = %s' % rfmRIBs
    print 'STAGE = %s' % STAGE
    print 'RMSPROD = %s' % RMSPROD
    print 'RMSPROJ = %s' % RMSPROJ

    print 'node = %s' % node
    print 'cfr  = %s' % cfr
    print 'sts  = %f' % sts
    print 'mbr  = %d' % mbr

    #clear ribs

#----------------------------------------------- Export Cache CMD
def ExportCacheCmd():
    fn = cmds.fileDialog2(fm=3,
                          cap='Export Tane Cache (Select Directory)',
                          okc='Export',
                          ocr='TN_ExportCache_UICreate',
                          oin='TN_ImportCache_UIInit',
                          ocm='TN_ImportCache_UICommit'
                          )
    if not fn:
       return
    #print fn

    # 1 : static  cache type
    # 2 : dynamic cache type
    cachemode   = cmds.optionVar(q='TN_ExportCache_CacheType')
    isDynamic   = False
    start       = 0.0
    end         = 0.0
    stepsize    = 0.0
    if cachemode == 0:  #invalid cache type
       cmds.error('invalid cache type')
       return

    if cachemode == 1:  #static cache type
       cachemode   = 1
       isDynamic   = False
       start       = 1.0
       end         = 1.0
       stepsize    = 1.0

    if cachemode == 2:  #dynamic cache type
       cachemode   = cmds.optionVar(q='TN_ExportCache_CacheType')
       isDynamic   = True
       start       = cmds.optionVar(q='TN_ExportCache_Start')
       end         = cmds.optionVar(q='TN_ExportCache_End')
       stepsize    = cmds.optionVar(q='TN_ExportCache_StepSize')

    print 'cachemode = %d' % cachemode
    print 'start     = %d' % start
    print 'end       = %d' % end
    print 'stepsize  = %d' % stepsize

    try:
       files = cmds.TN_ExportCacheCmd(cp=fn[0],
                                      id=isDynamic,
                                      sf=start,
                                      ef=end,
                                      ss=stepsize
                                      )
       for file in files:
           print '%s is exported' % file

    except:
       cmds.error("TN_ExportCacheCmd() failed")

    for file in files:
       print '%s is exported' % file

#----------------------------------------------- Import Cache CMD
def ImportCacheCmd():
    multipleFilters = "Tane Cache (*.tane);;All Files (*.*)"
    files = cmds.fileDialog2(fileFilter   = multipleFilters,
                             fileMode     = 4,
                             dialogStyle  = 2,
                             caption      = 'Select Tane Cache Files',
                             okCaption    = "Import"
                            )

    if files == None:
        #cmds.warning("files is None")
        return

    for file in files:
        cmds.TN_ImportCacheCmd(cp=file)
        print '%s is imported' % file

#----------------------------------------------- ExportCacheCmdAsMB
def ExportCacheCmdAsMB():
    nodes = cmds.ls(sl = True, type = 'TN_TaneMPxTransform')

    if len(nodes) == 0:
        cmds.warning('nodes is 0')
        return

    multipleFilters = "Tane Feather Cache (*.taneFeather);;All Files (*.*)"
    filename = cmds.fileDialog2(fileFilter         = multipleFilters,
                                dialogStyle        = 2,
                                )
    if filename == None:
        cmds.warning('filename is None')
        return


    output = filename[0]
    cmds.file(output, f=True,
              options="v=0",
              typ="mayaBinary",
              pr=True,
              es=True,
              de=False,
              sh=False
              )

#----------------------------------------------- Import CacheAsMB
def saveRlf():
    pass
    #scope     = rlf.RLFScope()
    #curScope  = rlf2maya.GetActiveScope()
    #rules = curScope.GetRules()
    #return rules
def recoverRlf(rules):
    pass
    #scope = rlf.RLFScope()
    #for rule in rules:
    #    scope.AddRule(rule)
    #rlf2maya.SetActiveScope(scope)
def ImportCacheCmdAsMB():
    pass
    #multipleFilters = "Tane Feather Cache (*.taneFeather);;All Files (*.*)"
    #caches = cmds.fileDialog2(fileFilter         = multipleFilters,
    #                          fileMode           = 4,
    #                          dialogStyle        = 2,
    #                          )
    #if caches == None:
    #    return
    #
    #dynRules = saveRlf()
    #for cache in caches:
    #    toks = cache.split('/')
    #    filename = toks[len(toks)-1]
    #    nodename = filename.split('.')[0]
    #    cmds.file(cache,
    #              i=True,
    #              type='mayaBinary',
    #              ignoreVersion=True,
    #              reference=False,
    #              ra=True,
    #              mergeNamespacesOnClash=True,
    #              namespace=':',
    #              options='v=0;',
    #              mnr=True
    #              #pr=True,
    #              #importTimeRange='combine',
    #              )
    #recoverRlf(dynRules)
    #
    #idx = cmds.getAttr('initialShadingGroup.dagSetMembers', size=True)
    #for shape in cmds.ls(g=True, type='mesh'):
    #    idx = idx + 1;
    #    ConnectInitialShadingGroup2(shape, idx)

    #cmds.select('%s:*' % nodename, r=True)

#----------------------------------------------- printAttr helper
def printAttr(node, like=''):
    heading = 'Node: %s' % node
    #print heading

    # Let's print out the fancy heading
    #print '*' * (len(heading)+6)
    #print '** %s **' % heading
    #print '*' * (len(heading)+6)

    # Let's get all the attributes of the node
    attributes = cmds.listAttr(node)
    # Let's loop over the attributes now and get their name info
    for attribute in attributes:

        # Some attributes will have children. (like publishedNodeInfo)
        # We make sure we split out their parent and only use the child's name
        # because attributeQuery cannot handle attributes with the parentName.childName format.
        attribute = attribute.split('.')[-1]

        # Let's now get the long name, short name
        # and nice name (UI name) of the atribute.
        longName = cmds.attributeQuery(attribute, node=node, longName=True)
        shortName = cmds.attributeQuery(attribute, node=node, shortName=True)
        niceName = cmds.attributeQuery(attribute, node=node, niceName=True)

        # if we the 'like' parameter has been set, we'll check if
        # the nice name has that string in it;
        # else we skip this attribute.
        if like and like.lower() not in niceName.lower():
            continue

        # Now that we have all the info we need, let's print them out nicely
        #heading = '\nAttribute: %s' % attribute
        #print heading
        print '-' * len(heading)
        print 'Nice name  : %-30s' % niceName
        print 'Short name : %-30s' % shortName
        print 'Long name  : %-30s' % longName
#----------------------------------------------- View Release Note
def ViewReleaseNote():
    page = 'http://10.0.0.111:8090/display/rndfxmain/Tane+Release+Note'
    os.system('firefox %s' % page)



#----------------------------------------------- View Release Note
def variantSets_Replace(nodeAttr, new):
    # Store the original parent and restore it below
    origParent = cmds.setParent(q=True)

    frameLayoutName = 'AEpxrUsdReferenceAssemblyTemplate_variantSets_Layout'
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
    variantSetsDict = {}
    usdPrim = UsdMaya.GetPrim(node)
    from pxr import Usd, UsdUtils

    regVarSetNames = [regVarSet.name
                      for regVarSet in UsdUtils.GetRegisteredVariantSets()]

    if usdPrim:
        variantSets = usdPrim.GetVariantSets()
        variantSetNames = variantSets.GetNames()
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

        cmds.optionMenuGrp(omg, e=True, changeCommand=functools.partial(variantSets_changeCommmand, omg=omg, node=node, variantSetName=variantSetName))

    # Restore the original parent
    cmds.setParent(origParent)
