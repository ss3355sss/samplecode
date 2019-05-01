# encoding=utf-8
# !/usr/bin/env python

import os

import maya.cmds
import maya.mel
import maya.OpenMaya as om
import maya.OpenMayaUI as mui

usingPyside2 = False
try:
    from shiboken2 import wrapInstance
    from PySide2 import QtCore, QtGui, QtWidgets
    usingPyside2 = True
except ImportError:
    from shiboken import wrapInstance
    from PySide import QtCore, QtGui
    import PySide.QtGui as QtWidgets

from functools import partial
import undo

DISABLE_UPDATE_UI = False
SCENE_OPENED = False

class DisplayModeComboBox(QtWidgets.QFrame):
    state_changed = QtCore.Signal()
    index_to_mode = {
        0 : 0, # none
        1 : 2, # boundingBox
        2 : 3, # proxy
    }
    mode_to_index = {
        0 : 0, # none
        2 : 1, # boundingBox
        3 : 2, # proxy
    }

    def __init__(self, index=0, parent=None, node=None):
        super(DisplayModeComboBox, self).__init__(parent)
        self.display_mode_combo_box_annotation = 'Per Source Display Mode'
        self.node  = node
        self.index = index

        self.setFixedSize(QtCore.QSize(80, 26))

        layout = QtWidgets.QHBoxLayout()
        layout.setContentsMargins(3,3,3,3)
        layout.setSpacing(3)

        self.displayBox = QtWidgets.QComboBox()
        self.displayBox.addItem('None')
        self.displayBox.addItem('Bounding Box')
        self.displayBox.addItem('Proxy Mesh')
        self.displayBox.currentIndexChanged.connect(self.change_status)

        layout.addWidget(self.displayBox)

        self.setLayout(layout)
        self.setStyleSheet("""
                            QFrame{border: none; background-color: #444444}
                            """
                           )

    def change_status(self, index):
        #print 'DisplayModeComboBox::change_status'
        mode = DisplayModeComboBox.index_to_mode[index];
        old = maya.cmds.getAttr('%s.displayMode' % self.node)
        if mode != old:
            maya.cmds.setAttr('%s.displayMode' % self.node, mode)
        self.state_changed.emit()

    def set_index(self, index):
        #print 'DisplayModeComboBox::set_value'
        self.displayBox.setCurrentIndex(index)

class ColorButton(QtWidgets.QFrame):
    state_changed = QtCore.Signal()
    def __init__(self, node=None, parent=None):
        super(ColorButton, self).__init__(parent)
        #self.display_mode_combo_box_annotation = 'Per Source Display Mode'
        self.node = node

        self.setFixedSize(QtCore.QSize(72, 26))

        layout = QtWidgets.QHBoxLayout()
        layout.setContentsMargins(3,3,3,3)
        layout.setSpacing(3)

        self.colorButton = QtWidgets.QPushButton()
        self.colorButton.setFixedSize(20, 20)
        self.colorButton.setCheckable(True)
        self.colorButton.setFlat(True)

        color = maya.cmds.getAttr('%s.color' % self.node)[0]
        self.colorButton.setStyleSheet(
            'background-color:rgb(%f, %f, %f)' % (color[0] * 255, color[1] * 255, color[2] * 255)
        )
        self.colorButton.clicked.connect(self.change_status)
        layout.addWidget(self.colorButton)

        self.setLayout(layout)

        self.setStyleSheet("""
                          QFrame{border: none; background-color: #444444}
                          QPushButton{border: none; icon-size: 20px;}
                          QPushButton:checked{border: none; background-color: #5384A5}
                          QPushButton:!checked{border: none; background-color: #444444}
                          """
                         )

    def change_status(self):
        #print 'ColorButton::change_status'
        color = maya.cmds.getAttr('%s.color' % self.node)[0]

        qcolor = QtGui.QColor()
        qcolor.setRedF(color[0])
        qcolor.setGreenF(color[1])
        qcolor.setBlueF(color[2])
        dialog = QtWidgets.QColorDialog(qcolor)
        msg = dialog.exec_()

        if msg == 1:
            setColor = dialog.currentColor()
            maya.cmds.setAttr('%s.color' % self.node, setColor.redF(), setColor.greenF(), setColor.blueF(), type = 'double3')
            self.colorButton.setStyleSheet('background-color:rgb(%s, %s, %s)' % (setColor.red(), setColor.green(), setColor.blue()))
        self.state_changed.emit()

    def set_color(self, color):
        #print 'ColorButton::set_color'
        self.colorButton.setStyleSheet(
            'background-color:rgb(%f, %f, %f)' % (color[0] * 255, color[1] * 255, color[2] * 255)
        )

class SidRatioSlider(QtWidgets.QFrame):
    state_changed = QtCore.Signal()
    def __init__(self, lid, node, parent=None):
        super(SidRatioSlider, self).__init__(parent)
        #self.display_mode_combo_box_annotation = 'Per Source Display Mode'
        self.node = node
        self.lid  = lid

        self.setFixedSize(QtCore.QSize(140, 26))

        layout = QtWidgets.QHBoxLayout()
        layout.setContentsMargins(3,3,3,3)
        layout.setSpacing(3)

        self.ratioSlider = QtWidgets.QSlider(QtCore.Qt.Horizontal)
        self.ratioSlider.setMinimum(0)
        self.ratioSlider.setMaximum(100)

        self.ratioSlider.valueChanged.connect(self.change_status)
        layout.addWidget(self.ratioSlider)

        self.ratioValueLineEdit = QtWidgets.QLineEdit()
        self.ratioValueLineEdit.editingFinished.connect(self.change_LineEdit_status)

        layout.addWidget(self.ratioValueLineEdit)

        self.setLayout(layout)
        self.setStyleSheet(
            """
            QFrame{border: none; background-color: #444444}
            """
        )

    def change_status(self, ratio):
        #print 'SidRatioSlider::change_status'
        old = maya.cmds.getAttr('%s.sidRatioArray[%d]' % (self.node, self.lid))
        if ratio != old:
            maya.cmds.setAttr('%s.sidRatioArray[%d]' % (self.node, self.lid), ratio)
        self.ratioValueLineEdit.setText(str(ratio))
        self.state_changed.emit()

    def change_LineEdit_status(self):
        ratio = int(self.ratioValueLineEdit.text())
        old = maya.cmds.getAttr('%s.sidRatioArray[%d]' % (self.node, self.lid))
        if ratio != old:
            maya.cmds.setAttr('%s.sidRatioArray[%d]' % (self.node, self.lid), ratio)
        self.set_ratio(ratio)
        self.state_changed.emit()

    def set_ratio(self, ratio):
        #print 'SidRatioSlider::set_ratio'
        #print 'SidRatioSlider::set_ratio::ratio = %s' % ratio
        self.ratioSlider.setValue(ratio)
        self.ratioValueLineEdit.setText(str(ratio))

class ObjectsWidget(QtWidgets.QTreeWidget):
    """ Input mesh tree widget """
    #obj_dropped  = QtCore.Signal(int) # signal fired after a drop event
    #obj_selected = QtCore.Signal(int) # signare fired after a item select event

    obj_dropped  = QtCore.Signal(basestring) # signal fired after a drop event
    obj_selected = QtCore.Signal(basestring) # signare fired after a item select event

    def __init__(self, parent=None):
        QtWidgets.QTreeWidget.__init__(self, parent)

        self.delete_annotation              = "Delete"
        self.refresh_annotation             = "Refresh"
        self.reveal_annotation              = "Reveal in Outliner"
        #self.add_selected_objs_annotation   = "Add Seleted Object"

        self.add_proxy_menu_annotation      = "Add Source By Type"
        self.add_abcProxy_annotation        = "Add AbcProxy"
        self.add_usdProxy_annotation        = "Add UsdProxy"
        self.add_vdbProxy_annotation        = "Add VdbProxy"

        self.font = QtGui.QFont()
        self.font.setPointSize(12)
        self.setFont(self.font)
        self.setDragEnabled(False)
        self.setDragDropMode(QtWidgets.QAbstractItemView.InternalMove)
        self.setAcceptDrops(True)
        self.node = None
        self._last_selected_item = None
        self.setIndentation(1)

        if usingPyside2:
            self.tree_header = self.header()
            self.tree_header.setSectionsMovable(False)
            #self.tree_header.setSectionResizeMode(0, QtWidgets.QHeaderView.Fixed)
            #self.tree_header.setSectionResizeMode(1, QtWidgets.QHeaderView.Fixed)
        else:
            self.tree_header = self.header()
            self.tree_header.setMovable(False)
            #self.tree_header.setResizeMode(0, QtWidgets.QHeaderView.Fixed)
            #self.tree_header.setResizeMode(1, QtWidgets.QHeaderView.Fixed)

        self.itemSelectionChanged.connect(self.selection_changed)

        self.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.customContextMenuRequested.connect(self.show_context_menu)

        self.deleteAction = QtWidgets.QAction(self.delete_annotation, self)
        self.deleteAction.triggered.connect(self.delete_item)

        #self.refreshAction = QtWidgets.QAction(self.refresh_annotation, self)
        #self.refreshAction.triggered.connect(self.refresh_item)

        self.revealAction = QtWidgets.QAction(self.reveal_annotation, self)
        self.revealAction.triggered.connect(self.reveal_item_in_outliner)

        self.menu = QtWidgets.QMenu(self)
        self.menu.addAction(self.deleteAction)
        #self.menu.addAction(self.refreshAction)
        self.menu.addAction(self.revealAction)

        #self.addObjectsAction = QtWidgets.QAction(self.add_selected_objs_annotation, self)
        #self.addObjectsAction.triggered.connect(self.add_objects)

        self.addProxyMenu = QtWidgets.QMenu(self.add_proxy_menu_annotation)
        self.addAbcProxyAction = self.addProxyMenu.addAction(self.add_abcProxy_annotation)
        self.addAbcProxyAction.triggered.connect(self.add_abcProxy)
        self.addUsdProxyAction = self.addProxyMenu.addAction(self.add_usdProxy_annotation)
        self.addUsdProxyAction.triggered.connect(self.add_usdProxy)
        self.addVdbProxyAction = self.addProxyMenu.addAction(self.add_vdbProxy_annotation)
        self.addVdbProxyAction.triggered.connect(self.add_vdbProxy)

        # add action
        self.global_menu = QtWidgets.QMenu(self)
        #self.global_menu.addAction(self.addObjectsAction)
        self.global_menu.addMenu(self.addProxyMenu)

    @staticmethod
    def _find_outliner():
        #print '\tObjectsWidget::_find_outliner'
        for panel in maya.cmds.getPanel(vis=1):
            if 'outliner' in panel:
                return panel

    def reveal_item_in_outliner(self):
        #print '\tObjectsWidget::reveal_item_in_outliner'
        outliner = self._find_outliner()
        if outliner is None:
            maya.cmds.OutlinerWindow()
            outliner = maya.cmds.getPanel(typ='outlinerPanel')[0]

        logicalIDs = []
        for item in self.selectedItems():
            logicalIDs.append(int(item.text(0).split(':')[0]))

        nodes = []
        for lid in sorted(logicalIDs, reverse=True):
            node = maya.cmds.listConnections('%s.inSource[%d]' % (self.node, lid))[0]
            nodes.append(node)

        maya.cmds.select(nodes, r=True)
        maya.cmds.evalDeferred("from maya import cmds; cmds.outlinerEditor('%s', e=1, sc=1)" % outliner)

    def refresh_item(self):
        print '\tObjectsWidget::refresh_item::Refresh an input group'
        pass

        #for item in self.selectedItems():
        #    id = int(item.text(0).split(':')[0])
        #    ae_utils.refresh_mesh_group(self.node, id)
        #self.obj_dropped.emit(id)

    @undo.chunk('TN_Connect_Group')
    def add_objects(self):
        #print '\tObjectsWidget::add_objects::Add selected objects to the repro node'
        pass
        #maya.cmds.undoInfo(ock=True)
        #objs = maya.cmds.ls(sl=True)
        #for obj in objs:
        #    if maya.cmds.objectType(obj) == "mesh":
        #        obj = maya.cmds.listRelatives(obj,p=True)[0]
        #    if maya.cmds.listRelatives(obj, ad=True, type="mesh"):
        #        #index = ae_utils.connect_mesh_group(self.node, obj)
        #        index = 'myTest'
        #        if index is not None:
        #            self.obj_dropped.emit(index)
        #
        #maya.cmds.undoInfo(cck=True)

    @undo.chunk('TN_Connect_Group')
    def add_abcProxy(self):
        #print '\tObjectsWidget::add_abcProxy'
        maya.cmds.undoInfo(ock=True)

        proxy = self.add_proxy('TN_AbcProxy')

        self.obj_dropped.emit(proxy)
        maya.cmds.undoInfo(cck=True)

    @undo.chunk('TN_Connect_Group')
    def add_usdProxy(self):
        #print '\tObjectsWidget::add_usdProxy'
        maya.cmds.undoInfo(ock=True)

        proxy = self.add_proxy('TN_UsdProxy')

        self.obj_dropped.emit(proxy)
        maya.cmds.undoInfo(cck=True)

    @undo.chunk('TN_Connect_Group')
    def add_vdbProxy(self):
        pass

    @undo.chunk('TN_Delete_Group')
    def delete_item(self):
        #print '\tObjectsWidget::delete_item::Remove a object from the repro node'

        maya.cmds.undoInfo(ock=True, chunkName="reproDeleteObject "+ self.node)

        logicalIDs = []
        for item in self.selectedItems():
            logicalIDs.append(int(item.text(0).split(':')[0]))

        for lid in sorted(logicalIDs, reverse=True):
            xform = maya.cmds.listConnections('%s.inSource[%d]' % (self.node, lid))[0]
            maya.cmds.delete(xform)

        self.obj_dropped.emit(None)

        maya.cmds.undoInfo(cck=True)
        self._last_selected_item = None

    def show_context_menu(self, pos):
        #print '\tObjectsWidget::show_context_menu'
        """ Show context menu """
        item = self.itemAt( pos )
        # if the mouse is on an item show also the delete action
        if item:
            self.menu.exec_( self.mapToGlobal(pos) )
        else:
            self.global_menu.exec_( self.mapToGlobal(pos) )

    def resizeEvent(self, event):
        #print '\tObjectsWidget::resizeEvent'
        """ Resize event, adjust the lod button size """
        QtWidgets.QTreeWidget.resizeEvent(self, event)

        ver_sb = self.verticalScrollBar()
        first_section_offset = 350
        if ver_sb.isVisible():
            #print '\tObjectsWidget::ver_sb.isVisible()'
            first_section_offset += ver_sb.width()

        #removing two pixels to prevent horizontal scroll
        total_width = self.size().width() - 2 # 2 = horizontal scroll
        self.setColumnWidth(0, total_width - first_section_offset)
        self.setColumnWidth(1, first_section_offset / 4.0)
        self.setColumnWidth(2, first_section_offset / 4.0)
        self.setColumnWidth(3, first_section_offset / 4.0)
        self.setColumnWidth(4, first_section_offset / 4.0)

    def set_node(self, node):
        #print '\tObjectsWidget::set_node:Set node'
        self.node = node

    @undo.chunk('TN_Drop_Node')
    def dropEvent(self, event):
        print '\tObjectsWidget::dropEvent::Drop event'
        pass
        #bg_item = self.itemAt( event.pos() )
        ## handle replace item
        #dropped_index = None
        #if bg_item and bg_item.treeWidget() is self:
        #    dropped_index = int(bg_item.text(0).split(':')[0])
        #
        #items = []
        #selected_items = self.selectedItems() or []
        ## check if the event has text data
        #if event.mimeData().text():
        #    objs = event.mimeData().text().split("\n")
        #    index = 0
        #    #for obj in objs:
        #    #    maya.cmds.undoInfo(ock=True)
        #    #    index_tmp = ae_utils.connect_mesh_group(self.node, obj, dropped_index)
        #    #    maya.cmds.undoInfo(cck=True)
        #    #    if index_tmp is not None:
        #    #        index = index_tmp
        #    self.obj_dropped.emit(index)
        #
        ##caching selection before drop
        #selection = [item.text(0).split(":")[-1] for item in self.selectedItems()]
        #
        ## let qt add the item widget to the tree
        #QtWidgets.QTreeWidget.dropEvent(self, event)
        #
        ## if the id of every item is not in ascending order it means that is a internal move event
        #lastId = -1
        #reorderIds = False
        #for i in range(self.topLevelItemCount()):
        #    item = self.topLevelItem(i)
        #    id = int(item.text(0).split(':')[0])
        #    if id < lastId:
        #        reorderIds = True
        #        lastId = id
        #        break
        #    lastId = id
        #if reorderIds:
        #    new_order = []
        #    for i in range(self.topLevelItemCount()):
        #        item = self.topLevelItem(i)
        #        id = int(item.text(0).split(':')[0])
        #        new_order.append(id)
        #    maya.cmds.undoInfo(ock=True)
        #    ae_utils.reorder_mesh_group_node(self.node, new_order)
        #    maya.cmds.undoInfo(cck=True)
        #    self.obj_dropped.emit(lastId)
        #elif not event.mimeData().text():
        #    if selected_items:
        #        id = int(selected_items[0].text(0).split(':')[0])
        #        self.obj_dropped.emit(id)
        #    else:
        #        self.obj_dropped.emit(None)
        #
        ##reapplying selection after drop
        #self.selectionModel().clearSelection()
        #for s in selection:
        #    found_items = self.findItems(':' + s, QtCore.Qt.MatchEndsWith)
        #    if not found_items:
        #        continue
        #    found_items[0].setSelected(True)

    def dragEnterEvent(self, qevent):
        print '\tObjectsWidget::dragEnterEvent::Drag enter event'
        pass
        qevent.accept()

    def selection_changed(self):
        #print '\tObjectsWidget::selection_changed::Selection changed'
        items = self.selectedItems() or []
        if items and self._last_selected_item not in [item.text(0) for item in items]:
            item = items[0]
            while item.parent():
                item = item.parent()
            id = int(item.text(0).split(':')[0])
            self.obj_selected.emit(id)

        if len(items) == 1:
            self._last_selected_item = items[0].text(0)

    def remove_last_selected(self):
        #print '\tObjectsWidget::remove_last_selected'
        self._last_selected_item = None

    def set_display_type(self, index, button):
        #print '\tObjectsWidget::set_display_type::Set display type (Mesh, Proxy, Lod)'
        maya.cmds.setAttr("%s.instancedGroup[%d].displayType" % (self.node, index), button.index)
        selected_items = self.selectedItems()
        curret_item_is_selected = False
        for item in selected_items:
            id = int(item.text(0).split(':')[0])
            if index == id:
                curret_item_is_selected = True
        if not curret_item_is_selected:
            return

        for item in selected_items:
            id = int(item.text(0).split(':')[0])
            if index == id:
                continue
            has_proxies = len(maya.cmds.getAttr("%s.instancedGroup[%d].proxyGroup" % (self.node, id), mi=True) or []) > 0
            if ((button.index > 0) and has_proxies) or (button.index == 0):
                maya.cmds.setAttr("%s.instancedGroup[%d].displayType" % (self.node, id), button.index)
                # update ui
                item_button = self.itemWidget(item, 1).widget
                item_button.set_index(button.index)

    def paintEvent(self, event):
        #print '\tObjectsWidget::paintEvent'
        super(ObjectsWidget, self).paintEvent(event)

        #hack for fixing a qt bug where drawing the selected row would overlap with the border
        painter = QtGui.QPainter(self.viewport())
        painter.fillRect(0, 0, 1, self.size().height(), QtGui.QColor(43, 43, 43))

    def getLastNode(self, node):
        list = maya.cmds.listConnections('%s.output' % node, d=True, s=False, sh=False)
        if not list:
            return
        node = list[0]
        return list[0]

    def add_proxy(self, nodeType):
        #create abcProxy node
        proxy = maya.cmds.TN_CreateNode(nt=nodeType)
        maya.cmds.setAttr('%s.displayMode' % proxy, 0)

        proxy = maya.cmds.listRelatives(proxy, p=True)[0]
        #connect to environment.inSource
        maya.cmds.connectAttr('%s.outSource' % proxy, '%s.inSource' % self.node, na=True, )

        #parent node
        parent = self.getLastNode(self.node)
        maya.cmds.parent(proxy, parent)

        return proxy

class CenterWidget(QtWidgets.QWidget):
    def __init__(self, parent, wrapped_widget):
        super(CenterWidget, self).__init__(parent)

        self._widget = wrapped_widget

        l = QtWidgets.QVBoxLayout()
        l.setContentsMargins(0, 0, 0, 0)
        l.addWidget(wrapped_widget, 0, QtCore.Qt.AlignCenter)

        self.setLayout(l)

    @property
    def widget(self):
        return self._widget

def get_maya_window():
    ptr = mui.MQtUtil.mainWindow()
    if ptr is not None:
        return wrapInstance(long(ptr), QtWidgets.QMainWindow)

def refresh_all_aetemplates(force=False):
    """ Refresh all the repro aetemplates"""
    widgets = get_maya_window().findChildren(AETN_Environment_InSourceTemplate) or []
    for widget in widgets:
        global SCENE_OPENED
        SCENE_OPENED = True
        widget.update_data(widget.node, force=force)
        SCENE_OPENED = False

def sceneChanged(data):
    #print 'sceneChanged'
    """ The scene is changed, due to a new scene or open scene command """
    global SCENE_OPENED
    SCENE_OPENED = True
    refresh_all_aetemplates(force=True)

class AETN_Environment_InSourceTemplate(QtWidgets.QWidget):
    camera_has_changed = QtCore.Signal(bool)

    TREE_STYLE_SHEET = '''
                     QTreeWidget {outline:0; border: 2px solid #2B2B2B; border-left: 1px solid #2B2B2B; border-top: 1px solid #2B2B2B;  icon-size: 20px;}
                     QHeaderView::section{min-height: 20; background: #444444; padding-left: 2px; border: 1px solid #2B2B2B;}
                     QHeaderView::section:first{padding-left: 22px;}
                     QTreeView::item {height:20; border: 1px solid #2B2B2B; border-right-color:transparent; border-left: 0px solid #2B2B2B;}
                     QTreeView::item:selected {background: #5384A5}
                     QTreeView::item:!selected {background: #5D5D5D}
                     '''

    def __init__(self, node, parent=None):
        #print 'AEMASH_ReproTemplate::__init__'
        super(AETN_Environment_InSourceTemplate, self).__init__(parent)

        self.objects_annotation     = "Objects"         #maya.mel.eval('getPluginResource("MASH_Repro", "kObjectsLabel")')
        self.display_annotation     = "Display Mode"    #maya.mel.eval('getPluginResource("MASH_Repro", "kDisplayLabel")')
        self.color_annotation       = "Color"
        self.ratio_annotation       = "Ratio"

        self.openCallback = om.MSceneMessage.addCallback(om.MSceneMessage.kAfterFileRead, sceneChanged)
        self.newCallback = om.MSceneMessage.addCallback(om.MSceneMessage.kAfterNew, sceneChanged)

        self.updating_UI = True
        self.node = node

        self.layout = QtWidgets.QVBoxLayout()
        self.layout.setContentsMargins(QtCore.QMargins(9, 0, 0, 0))

        splitter = QtWidgets.QSplitter(QtCore.Qt.Vertical)
        #splitter.setStyleSheet("QSplitter::handle {image: url(%s); height: 20px;}" % MASH_REPRO_ICONS.get_icon_path("ae_MASH_ReproDrag_hor"))
        self.layout.addWidget(splitter)

        # build widget
        self.numColums = 4
        self.objs_widget = ObjectsWidget()
        self.objs_widget.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.objs_widget.setColumnCount(self.numColums)
        self.objs_widget.set_node(node)
        self.font1 = QtGui.QFont()
        self.font1.setPointSize(9)

        # add proxy name
        self.objs_widget.headerItem().setFont(0, self.font1)
        self.objs_widget.headerItem().setText(0, self.objects_annotation)
        # add display mode
        self.objs_widget.headerItem().setFont(1, self.font1)
        self.objs_widget.headerItem().setText(1, self.display_annotation)
        # add color
        self.objs_widget.headerItem().setFont(2, self.font1)
        self.objs_widget.headerItem().setText(2, self.color_annotation)
        # add ratio
        self.objs_widget.headerItem().setFont(3, self.font1)
        self.objs_widget.headerItem().setText(3, self.ratio_annotation)
        self.objs_widget.setSelectionMode(QtWidgets.QAbstractItemView.ExtendedSelection)
        self.objs_widget.setStyleSheet(self.TREE_STYLE_SHEET)
        splitter.addWidget(self.objs_widget)

        self.setLayout(self.layout)

        self.update_ui()

        self.objs_widget.obj_dropped.connect(self.update_ui)
        self.objs_widget.obj_selected.connect(self.update_proxies)

        self.updating_UI = False
        self.setObjectName("AETN_Environment_InSourceTemplate")

    def __del__(self):
        #print 'AEMASH_ReproTemplate::__del__::Destructor, remove the scene callback'

        om.MSceneMessage.removeCallback(self.openCallback)
        om.MSceneMessage.removeCallback(self.newCallback)

    def set_node(self, node):
        #print 'AEMASH_ReproTemplate::set_node::Set node'

        self.node = node
        self.objs_widget.set_node(node)

    def camera_changed(self, camera):
        #print 'AEMASH_ReproTemplate::camera_changed::Camera changed'
        if self.updating_UI:
            return

        if maya.cmds.objExists(camera) and maya.cmds.objectType(camera,i="camera"):
            self.camera_has_changed.emit(True)
        else:
            self.camera_has_changed.emit(False)

    def update_proxies(self, isntance_index=-1):
        #print 'AEMASH_ReproTemplate::update_proxies::Update the proxies list'
        #self.proxy_widget.instance_index = isntance_index
        #self.proxy_widget.clear()
        #num_groups = maya.cmds.getAttr("%s.instancedGroup[%d].proxyGroup" % (self.node, isntance_index), mi=True) or []
        #for group in num_groups:
        #    child_ids = maya.cmds.getAttr("%s.instancedGroup[%d].proxyGroup[%d].proxy" % (self.node, isntance_index, group), mi=True) or []
        #    if not child_ids:
        #        continue
        #    # get group name
        #    groups_connections = maya.cmds.listConnections("%s.instancedGroup[%d].proxyGroup[%d].proxyGroupMessage" % (self.node, isntance_index, group),s=1) or []
        #    if len(groups_connections) == 0:
        #        continue
        #    group_item_name = groups_connections[0]
        #
        #    group_item = QtWidgets.QTreeWidgetItem(self.proxy_widget)
        #    group_item.setFlags(QtCore.Qt.ItemIsDragEnabled | QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable)
        #    spinBox = QtWidgets.QDoubleSpinBox(self)
        #    spinBox.setStyleSheet("QDoubleSpinBox{border: none;}")
        #    spinBox.setToolTip(self.lod_spinbox_annotation)
        #    spinBox.setMaximum(100000000)
        #    value = maya.cmds.getAttr("%s.instancedGroup[%d].proxyGroup[%d].proxyLod" % (self.node, isntance_index, group))
        #    spinBox.setValue(value)
        #    spinBox.setButtonSymbols(QtWidgets.QAbstractSpinBox.NoButtons)
        #    spinBox.editingFinished.connect(partial(self.proxy_widget.set_lod_distance, group, spinBox))
        #    spinBox.setFixedSize(QtCore.QSize(72, 26))
        #    self.proxy_widget.setItemWidget(group_item, 1, CenterWidget(self, spinBox))
        #    group_item.setText(0, "%d: %s" % (group, group_item_name))
        #    group_item.setIcon(0, QtGui.QIcon(MASH_REPRO_ICONS.vertical_drag()))

        self._disable_object_tree_buttons()

    def update_ui(self, index=None):
        #print 'AEMASH_ReproTemplate::update_ui::Update the objects list'
        self.objs_widget.clear()
        if self.node is None:
            return

        if not maya.cmds.objExists(self.node):
            self.node = None
            return

        #print 'AEMASH_ReproTemplate::index = %s' % index
        #print 'AEMASH_ReproTemplate::node = %s' % self.node

        lid    = maya.cmds.TN_ConnectionInfo(nn=self.node, tg='lid') or []
        shape  = maya.cmds.TN_ConnectionInfo(nn=self.node, tg='srcShape') or []
        srcs = dict(zip(lid, shape))
        self.objs_widget.remove_last_selected()

        for lid, shape in srcs.items():
            #print 'AEMASH_ReproTemplate::pid = %d, (%d, %s)' % (pid, lid, shape)

            itemName = maya.cmds.listRelatives(shape, p=True)[0]
            item = QtWidgets.QTreeWidgetItem(self.objs_widget)
            item.setFlags(QtCore.Qt.ItemIsDragEnabled | QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable)

            #build display mode widget
            displayModeComboBox     = DisplayModeComboBox(node=shape, parent=self)
            displayMode             = maya.cmds.getAttr('%s.displayMode' % shape)
            displayModeComboBox.set_index(DisplayModeComboBox.mode_to_index[displayMode])

            #build color widget
            colorButton             = ColorButton(node=shape, parent=self)
            color                   = maya.cmds.getAttr('%s.color' % shape)[0]
            colorButton.set_color(color)

            #build sidRatio widget
            sidRatioSlider          = SidRatioSlider(lid=lid, node=self.node, parent=self)
            ratio                   = maya.cmds.getAttr('%s.sidRatioArray[%d]' % (self.node, lid))
            sidRatioSlider.set_ratio(ratio)

            #add widgets
            self.objs_widget.setItemWidget(item, 1, CenterWidget(self, displayModeComboBox))
            self.objs_widget.setItemWidget(item, 2, CenterWidget(self, colorButton))
            self.objs_widget.setItemWidget(item, 3, CenterWidget(self, sidRatioSlider))

            #set name widget
            item.setText(0, "%d: %s" % (lid, itemName))
            #item.setIcon(0, QtGui.QIcon(MASH_REPRO_ICONS.vertical_drag()))

            if index == shape:
                item.setSelected(True)

        self._disable_object_tree_buttons()

    def _disable_object_tree_buttons(self):
        pass

    def update_data(self, node, force=False):
        #print 'AEMASH_ReproTemplate::update_data::Update the ui data'

        global SCENE_OPENED
        if SCENE_OPENED:
            #print 'AEMASH_ReproTemplate::update_data::Scene Opened!!'
            self.set_node(node)
            self.update_ui()
            SCENE_OPENED = False

        if node != self.node or force:
            #print 'AEMASH_ReproTemplate::update_data::node != self.node or force!!'

            self.set_node(node)
            self.update_ui()

def build_qt_widget(lay, node):
    """ Build the Repro AETemplate """
    widget = AETN_Environment_InSourceTemplate(node)
    ptr = mui.MQtUtil.findLayout(lay)
    if ptr is not None:
        maya_widget = wrapInstance(long(ptr), QtWidgets.QWidget)
        maya_layout = maya_widget.layout()
        maya_layout.addWidget(widget)

def update_qt_widget(layout, node):
    """ Update the Repro AETemplate """
    ptr = mui.MQtUtil.findLayout(layout)
    if ptr is not None:
        maya_widget = wrapInstance(long(ptr), QtWidgets.QWidget)
        maya_layout = maya_widget.layout()
        for c in range(maya_layout.count()):
            widget = maya_layout.itemAt(c).widget()
            if isinstance(widget, AETN_Environment_InSourceTemplate):
                force = True
                widget.update_data(node, force)
                break