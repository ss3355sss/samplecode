# encoding=utf-8
# !/usr/bin/env python

import maya.cmds as cmds
import maya.mel as mel
import maya.OpenMayaUI as mui
import maya.OpenMaya as om
import maya.OpenMayaFX as omfx

try:
    from shiboken2 import wrapInstance
    from PySide2 import QtCore, QtGui, QtWidgets
except ImportError:
    from shiboken import wrapInstance
    from PySide import QtCore, QtGui
    import PySide.QtGui as QtWidgets

#get the string resources
kDisconnectedNodes      = mel.eval('getPluginResource("Tane", "kDisconnectedNodes")')
kConnectedTheseNodes    = mel.eval('getPluginResource("Tane", "kConnectedTheseNodes")')
kIncompatibleTypes      = mel.eval('getPluginResource("Tane", "kIncompatibleTypes")')
kTypeNotSelected        = mel.eval('getPluginResource("Tane", "kTypeNotSelected")')
kDragDropType           = mel.eval('getPluginResource("Tane", "kDragDropType")')
kShowInOutliner         = mel.eval('getPluginResource("Tane", "kShowInOutliner")')
kBreakConnection        = mel.eval('getPluginResource("Tane", "kBreakConnection")')
kConnect                = mel.eval('getPluginResource("Tane", "kConnect")')
kConnected              = mel.eval('getPluginResource("Tane", "kConnected")')
kTo                     = mel.eval('getPluginResource("Tane", "kTo")')
kIncompatibleNodes      = mel.eval('getPluginResource("Tane", "kIncompatibleNodes")')
kAnd                    = mel.eval('getPluginResource("Tane", "kAnd")')
kDisconnected           = mel.eval('getPluginResource("Tane", "kDisconnected")')
kFrom                   = mel.eval('getPluginResource("Tane", "kFrom")')
kNotConnected           = mel.eval('getPluginResource("Tane", "kNotConnected")')
kNoMeshFound            = mel.eval('getPluginResource("Tane", "kNoMeshFound")')
kDisabled               = mel.eval('getPluginResource("Tane", "kDisabled")')
kClone                  = mel.eval('getPluginResource("Tane", "kClone")')
kCreate                 = mel.eval('getPluginResource("Tane", "kCreate")')
kCreated                = mel.eval('getPluginResource("Tane", "kCreated")')
kNo                     = mel.eval('getPluginResource("Tane", "kNo")')
kSelected               = mel.eval('getPluginResource("Tane", "kSelected")')
kFalloffNoInputs        = mel.eval('getPluginResource("Tane", "kFalloffNoInputs")')
kDisplayAsWireframe     = mel.eval('getPluginResource("Tane", "kDisplayAsWireframe")')

class AETN_QLineEditExtend(QtWidgets.QLineEdit):
    def __init__(self, desiredNodeType, node, attr, sourceAttr, postCmd, parent=None):
        super(AETN_QLineEditExtend, self).__init__()
        self.setAcceptDrops(True)
        self.setMaximumHeight(22)
        self.setReadOnly(True)
        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.customContextMenuRequested.connect(self.openMenu)
        self.setMouseTracking(True)
        self.desiredNodeType = desiredNodeType
        self.droppedNode = ""
        self.node = node
        self.attr = attr
        self.disableDropZone = False
        self.connected = False
        self.menu = QtWidgets.QMenu();
        self.sourceAttrRaw = sourceAttr
        self.sourceAttr = None
        self.postCmd = postCmd
        self.checkConnections()
        self.objectName = "TN_DropZone"
        self.wireframeAction = QtWidgets.QAction(kDisplayAsWireframe, self, checkable=True)
        self.wireframeAction.triggered.connect(self.setMeshToWireframe,)

    def setConnectedStyle(self):
        self.setStyleSheet("background-color:#F1F1A5; color: black;")

    def setBlankStyle(self):
        self.setStyleSheet("")

    def setDisabledStyle(self):
        self.setStyleSheet("background-color:#5C6874; color: black;")

    def checkConnections(self):
        connections = cmds.listConnections('%s.%s' % (self.node,self.attr), sh=True)

        #connections0 = cmds.listConnections('%s.%s' % (self.node,self.attr))
        #print '%s.%s' % (self.node,self.attr)
        #print connections
        #print connections0

        if not connections:
            self.setText(kNotConnected)
            self.droppedNode = ""
            self.connected = False
            self.setBlankStyle()
        elif (len(connections) > 0):
            self.droppedNode = connections[0]
            self.setText(connections[0])
            self.connected = True
            self.setConnectedStyle()

    def disableDropZoneDropZone(self):
        self.setText(kDisabled)
        self.droppedNode = ""
        self.setDisabledStyle()

    def enterEvent(self,event):
        self.setCursor(QtGui.QCursor())
        connections = cmds.listConnections('%s.%s' % (self.node,self.attr), sh=True)
        if not connections and not self.disableDropZone:
            self.setText("%s %s" % (kDragDropType, self.desiredNodeType))

    def leaveEvent(self,event):
        #print 'leaveEvent'
        self.setCursor(QtCore.Qt.ArrowCursor)
        connections = cmds.listConnections('%s.%s' % (self.node,self.attr), sh=True)
        self.checkConnections()

    def dragEnterEvent(self, e):
        #print 'dragEnterEvent'
        if e.mimeData().hasFormat('text/plain'):
            e.accept()
        else:
            e.ignore()

    def dropEvent(self, e):
        #print 'dropEvent'
        self.droppedNode = e.mimeData().text()
        self.connectNode()

    def dragMoveEvent(self, e):
        #print 'dragMoveEvent'
        if e.mimeData().hasFormat('text/plain'):
            e.accept()
        else:
            e.ignore()

    def openMenu(self, pos):
        if self.disableDropZone:
            return
        self.checkConnections()
        self.menu.clear()

        createMapHelperAction = False
        if self.postCmd == 'MapSwitchToUVMode' or self.postCmd == 'PosMapSwitchToUVMode' or self.postCmd == 'ScaleMapSwitchToUVMode' or self.postCmd == 'RotMapSwitchToUVMode':
            createMapHelperAction = True

        if (self.connected == True):
            #print 'self.connected == True'
            showInOutliner = QtWidgets.QAction(kShowInOutliner, self)
            showInOutliner.triggered.connect(self.showInOutliner)
            self.menu.addAction(showInOutliner)
            disconnectAttr = QtWidgets.QAction(kBreakConnection, self)
            disconnectAttr.triggered.connect(self.disconnectNode)
            self.menu.addAction(disconnectAttr)
            if self.desiredNodeType == 'mesh':
                self.menu.addAction(self.wireframeAction)
        else:
            sel = cmds.ls( selection=True )
            if sel:
                #print 'sel'
                connectAttr = QtWidgets.QAction(kConnect, self)
                connectAttr.triggered.connect(lambda: self.connectNode(sel[0]))
                self.menu.addAction(connectAttr)

            if self.desiredNodeType == 'transform' and not createMapHelperAction: #creating transforms is trivial, so lets support it.
                #print 'self.desiredNodeType == transform'
                createLocAction = QtWidgets.QAction(kCreate, self)
                createLocAction.triggered.connect(self.createLocator)
                self.menu.addAction(createLocAction)

            elif createMapHelperAction: #this means it's a map helper, we need to support creating those.
                #print "createMapHelperAction"
                createLocAction = QtWidgets.QAction(kCreate, self)
                createLocAction.triggered.connect(self.createMapHelper)
                self.menu.addAction(createLocAction)

        self.menu.exec_(QtGui.QCursor.pos())

    def showInOutliner(self):
        if (self.droppedNode != ""):
            allOutliners = cmds.getPanel(typ='outlinerPanel')   #get the names of all the outliners
            outliner = str(allOutliners[0])                     #assume we want the first one
            allPannels = cmds.getPanel(vis=1)                   #get all the pannels
            if any('outliner' in s for s in allPannels): #is there an outliner in the house?
                cmds.select( clear=True )
                cmds.select( self.droppedNode )
                cmds.evalDeferred("import maya.cmds as cmds; cmds.outlinerEditor('%s', e=1, sc=1)" % outliner) # show in the outliner
            else:
                cmds.OutlinerWindow() #bring up an outliner
                cmds.select( clear=True )
                cmds.select( self.droppedNode )
                cmds.evalDeferred("import maya.cmds as cmds; cmds.outlinerEditor('%s', e=1, sc=1)" % outliner) # show in the outliner

    def goToNode(self):
        if (self.droppedNode != ""):
            command = 'evalDeferred \"showEditorExact(\\"'+ self.droppedNode + '\\")\"'
            mel.eval(command)

    def isDesiredNode(self, nodeType):
        #print 'isDesiredNode'
        wantedNodeTypes = self.desiredNodeType.split(',')
        destinationAttr = self.sourceAttrRaw.split(',')
        for i in range (0, len(wantedNodeTypes), 1):
            if wantedNodeTypes[i] == nodeType:
                self.sourceAttr = destinationAttr[i]
                return True
        return False

    def connectNode(self, selNode=""):
        #check if a node has been specified in the function call
        if len(selNode) > 0:
            self.droppedNode = selNode


        #to check if the pre connection command passes
        stat = True
        if (len(self.postCmd) > 0):
            stat = self.runPreCommand(self.postCmd)
        if not stat:
            return

        nodeType = cmds.nodeType( self.droppedNode )

        #if the user selected a transform (and that isn't what we want) then get it's shape node
        if (nodeType == "transform" and self.desiredNodeType != "transform"):
            shapes = cmds.listRelatives(self.droppedNode, pa=True)
            #print shapes
            self.droppedNode = shapes[0] #get the first found shape
            nodeType = cmds.nodeType( self.droppedNode )

        isWanted = self.isDesiredNode(nodeType)

        if isWanted:
            self.setText(self.droppedNode)
            self.setConnectedStyle()
            cmds.connectAttr('%s.%s' % (self.droppedNode, self.sourceAttr), '%s.%s' % (self.node, self.attr), force=True )

            if (len(self.postCmd) > 0):
                self.runPostCommand(self.postCmd)
        else:
            message = cmds.format(kIncompatibleTypes, stringArg=(nodeType, self.desiredNodeType))
            command = 'evalDeferred \"TN_InViewMessage(\\"'+ message + '\\", \\"Error\\")\"'
            mel.eval(command)
        self.menu.clear()

    def disconnectNode(self):
        #print "disconnectNode"
        nodeType = cmds.nodeType( self.droppedNode )
        self.isDesiredNode(nodeType)
        cmds.disconnectAttr('%s.%s' % (self.droppedNode,self.sourceAttr), '%s.%s' % (self.node,self.attr))
        self.menu.clear()
        self.setText(kNotConnected)
        self.setBlankStyle()

        message = cmds.format(kDisconnectedNodes, stringArg=(self.droppedNode, self.node))
        command = 'evalDeferred \"TN_InViewMessage(\\"'+ message + '\\", \\"Info\\")\"'
        mel.eval(command)
        if (len(self.postCmd) > 0):
            self.runPostDisconnectCommand(self.postCmd)

    def setMeshToWireframe(self):
        if self.wireframeAction.isChecked():
            cmds.setAttr( '%s.overrideEnabled' % (self.droppedNode), 1 )
            cmds.setAttr( '%s.overrideShading' % (self.droppedNode), 0 )
            cmds.setAttr( '%s.overrideColor' % (self.droppedNode), 9 )
        else:
            cmds.setAttr( '%s.overrideEnabled' % (self.droppedNode), 0 )
            cmds.setAttr( '%s.overrideShading' % (self.droppedNode), 1 )
            cmds.setAttr( '%s.overrideColor' % (self.droppedNode), 0 )

    def runPostCommand(self, commandId):
        pass
        #print self.setAcceptDrops
        #print self.setMaximumHeight
        #print self.setReadOnly
        #print self.setContextMenuPolicy
        #print self.customContextMenuRequested
        #print self.setMouseTracking
        #print self.desiredNodeType
        #print self.droppedNode
        #print self.node
        #print self.attr
        #print self.disableDropZone
        #print self.connected
        #print self.menu
        #print self.sourceAttrRaw
        #print self.sourceAttr
        #print self.postCmd
        #print self.checkConnections()
        #print self.objectName
        #print '---------'
        #print 'self.node = %s' % self.node
        #print 'self.attr = %s' % self.attr
        #if commandId == 'SwitchToMeshMode':
        #cmds.setAttr( '%s.inBody' % (self.node), 4 )
        # elif commandId == 'ExplodeNodeConnect':
        #     command = 'evalDeferred \"explodeButtonCMDS(\\"'+ self.node + '\\", \\"'+ self.droppedNode + '\\",1)\"'
        #     mel.eval(command)
        # elif commandId == 'OffsetSwitchToClosestPointMode':
        #     cmds.setAttr( '%s.offsetType' % (self.node), 5 )
        # elif commandId == 'OrientSwitchToAimMode':
        #     cmds.setAttr( '%s.targetMode' % (self.node), 1 )
        #     cmds.setAttr( '%s.orientMode' % (self.node), 2 )
        # elif commandId == 'OrientSwitchToMeshMode':
        #     cmds.setAttr( '%s.orientMode' % (self.node), 3 )
        # elif commandId == 'DelaySwitchToFTLMode':
        #     cmds.setAttr( '%s.delayMode' % (self.node), 2 )
        # elif commandId == 'MapSwitchToUVMode':
        #     cmds.setAttr( '%s.mapDirection' % (self.node), 1 )
        # elif commandId == 'SwitchToVtxSetMode':
        #     cmds.setAttr( '%s.arrangement' % (self.node), 4 )
        #     cmds.setAttr( '%s.meshType' % (self.node), 7 )
        # elif commandId == 'TransformNodeConnect':
        #     cmds.connectAttr(  '%s.rotate' % (self.droppedNode), '%s.rotationAmount' % (self.node), force=True )
        #     cmds.connectAttr(  '%s.scale' % (self.droppedNode), '%s.scaleAmount' % (self.node), force=True )
        # elif commandId == 'CurveWarpAimCurve':
        #     #special command for the curveWarp node - which is not technically a MASH node.
        #     cmds.setAttr( '%s.aimMode' % (self.node), 3 )
        # elif commandId == 'ReplicatorEnableCurve':
        #     cmds.setAttr( '%s.useCurve' % (self.node), 1)
        # elif commandId == 'FalloffShapeIn':
        #     nodeType = cmds.nodeType( self.droppedNode )
        #     if nodeType == 'mesh':
        #         cmds.setAttr( '%s.falloffShape' % (self.node), 6 )
        #     elif nodeType == 'nParticle':
        #         cmds.setAttr( '%s.falloffShape' % (self.node), 5 )
        #     elif nodeType == 'nurbsCurve':
        #         cmds.setAttr( '%s.falloffShape' % (self.node), 4 )
        # elif commandId == 'PosMapSwitchToUVMode':
        #     cmds.setAttr( '%s.posMapDirection' % (self.node), 1 )
        # elif commandId == 'RotMapSwitchToUVMode':
        #     cmds.setAttr( '%s.rotMapDirection' % (self.node), 1 )
        # elif commandId == 'ScaleMapSwitchToUVMode':
        #     cmds.setAttr( '%s.scaleMapDirection' % (self.node), 1 )
        # elif commandId == 'WorldNodeConnectMatrix':
        #     transforms = cmds.listRelatives(self.droppedNode, parent=True, fullPath=True)
        #     cmds.connectAttr(  '%s.worldMatrix[0]' % (transforms[0]), '%s.groundMatrix' % (self.node), force=True )

    def runPostDisconnectCommand(self, commandId):
        pass

        #print "runPostDisconnectCommand"
        #if commandId == 'TransformNodeConnect':
        #    cmds.disconnectAttr(  '%s.rotate' % (self.droppedNode), '%s.rotationAmount' % (self.node))
        #    cmds.disconnectAttr(  '%s.scale' % (self.droppedNode), '%s.scaleAmount' % (self.node))
        #    cmds.select( clear=True )
        #    cmds.select( self.node )
        #elif commandId == 'ReplicatorEnableCurve':
        #    cmds.setAttr( '%s.useCurve' % (self.node), 0)
        #elif commandId == 'FalloffShapeIn':
        #    cmds.setAttr( '%s.falloffShape' % (self.node), 1 )
        #elif commandId == 'MapSwitchToUVMode':
        #    cmds.setAttr( '%s.mapDirection' % (self.node), 2 )
        #elif commandId == 'PosMapSwitchToUVMode':
        #    cmds.setAttr( '%s.posMapDirection' % (self.node), 2 )
        #elif commandId == 'RotMapSwitchToUVMode':
        #    cmds.setAttr( '%s.rotMapDirection' % (self.node), 2 )
        #elif commandId == 'ScaleMapSwitchToUVMode':
        #    cmds.setAttr( '%s.scaleMapDirection' % (self.node), 2 )

    def runPreCommand(self, commandId):
        return True
        #createMapHelperAction = False
        #if self.postCmd == 'MapSwitchToUVMode' or self.postCmd == 'PosMapSwitchToUVMode' or self.postCmd == 'ScaleMapSwitchToUVMode' or self.postCmd == 'RotMapSwitchToUVMode':
        #   createMapHelperAction = True
        #
        #if createMapHelperAction:
        #   error = False
        #   shapes = cmds.listRelatives(self.droppedNode)
        #   if not shapes or cmds.nodeType( shapes[0] ) != "mesh":
        #       error = True
        #
        #   if error:
        #       command = 'evalDeferred \"TN_InViewMessage(\\"'+ kNoMeshFound + '\\", \\"Error\\")\"'
        #       mel.eval(command)
        #   return not error
        #return True

    def createLocator(self):
        pass
        #newTransform = cmds.spaceLocator( p=(0, 0, 0), n='%s_loc'% (self.node)  )
        #cmds.setAttr( '%s.overrideColor' % (newTransform[0]), 9 )
        #cmds.setAttr( '%s.overrideEnabled' % (newTransform[0]), 1 )
        #self.droppedNode = newTransform[0]
        #self.connectNode()
        #self.setConnectedStyle()

    def createMapHelper(self):
        pass
        #if self.postCmd == 'MapSwitchToUVMode':
        #    mel.eval('mapHelperButtonCMDS("'+self.node+'", 1)')
        #elif self.postCmd == 'PosMapSwitchToUVMode':
        #    mel.eval('mapHelperButtonCMDS("'+self.node+'", 10)')
        #elif self.postCmd == 'RotMapSwitchToUVMode':
        #    mel.eval('mapHelperButtonCMDS("'+self.node+'", 20)')
        #elif self.postCmd == 'ScaleMapSwitchToUVMode':
        #    mel.eval('mapHelperButtonCMDS("'+self.node+'", 30)')
        #if (len(self.postCmd) > 0):
        #    self.runPostCommand(self.postCmd)


class AETN_SingleInputQtWidget(QtWidgets.QWidget):
    def __init__(self, node, wantedType, attr, sourceAttr, postCmd, parent=None):
        super(AETN_SingleInputQtWidget, self).__init__(parent)
        self.node = node
        self.dropZone = AETN_QLineEditExtend(wantedType, node, attr, sourceAttr, postCmd)
        self.layout = QtWidgets.QBoxLayout(QtWidgets.QBoxLayout.TopToBottom, self)
        self.layout.setContentsMargins(5,3,11,3)
        self.layout.setSpacing(5)
        self.layout.addWidget(self.dropZone)

    #update connections
    def set_node(self, node, wantedType, attr, sourceAttr, postCmd):
        self.node = node
        self.dropZone.node = node
        self.dropZone.desiredNodeType = wantedType
        self.dropZone.attr = attr
        self.dropZone.sourceAttr = sourceAttr
        self.dropZone.checkConnections()
        self.dropZone.postCmd = postCmd

def build_qt_widget(lay, node, wantedType, attr, sourceAttr, postCmd):
    widget = AETN_SingleInputQtWidget(node, wantedType, attr, sourceAttr, postCmd)
    ptr = mui.MQtUtil.findLayout(lay)
    if ptr is not None:
        maya_widget = wrapInstance(long(ptr), QtWidgets.QWidget)
        maya_layout = maya_widget.layout()
        maya_layout.addWidget(widget)

def update_qt_widget(layout, node, wantedType, attr, sourceAttr, postCmd):
    ptr = mui.MQtUtil.findLayout(layout)
    if ptr is not None:
        maya_widget = wrapInstance(long(ptr), QtWidgets.QWidget)
        maya_layout = maya_widget.layout()
        for c in range(maya_layout.count()):
            widget = maya_layout.itemAt(c).widget()
            if widget.metaObject().className() == "AETN_SingleInputQtWidget":
                widget.set_node(node, wantedType, attr, sourceAttr, postCmd)
                break
