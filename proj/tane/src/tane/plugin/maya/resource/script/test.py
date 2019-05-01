# # import maya.standalone
# # maya.standalone.initialize(name='python')
#
# import os, sys
# import pprint
# import site
# import optparse
# import getpass
#
# import maya.cmds as cmds
# import maya.mel as mel
#
#
# ScriptRoot =  os.path.dirname( os.path.abspath(__file__) )
#
# def setupPlugins():
#     plugins = ['TaneForMaya']
#     unPlugins = ['bifrostvisplugin', 'bifrostshellnode', 'ZArachneForMaya']
#
#     for up in unPlugins:
#         if cmds.pluginInfo(up, q=True, l=True):
#             cmds.unloadPlugin(up)
#         print '### unload plugin:', up, cmds.pluginInfo(up, q=True, l=True)
#
#     for p in plugins:
#         if not cmds.pluginInfo(p, q=True, l=True):
#             cmds.loadPlugin(p)
#         print '### plugin:', p, cmds.pluginInfo(p, q=True, l=True)
#
#
# def checkOptionToSpool(opts):
#     '''
#     # Check option to make tractor task
#     '''
#     if opts.mesh:
#         if not cmds.ls(type='dxRig'):
#             opts.mesh = False
#     if opts.camera:
#         if cmds.ls('*_renderCam', type='camera'):
#             opts.camera = False
#     if opts.layout:
#         if cmds.ls(type='dxAssembly'):
#             opts.layout = False
#
#     return opts
#
# if __name__ == '__main__':
#     #optparser = usdCommonSetup.optParserSetup()
#     #( opts, args ) = optparser.parse_args( sys.argv )
#     # print pprint.pprint(opts)
#     #if not opts.srcfile:
#     #    os._exit( 0 )
#
#     from pymel.all import *
#     setupPlugins()
#
#     #cmds.loadPlugin("/netapp/backstage/pub/lib/Tane/2017_21.7/plug-ins/TaneForMaya.so")
#
#     data =;
#
#     {
#     print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!before convert"
#     oldFile = "/netapp/dexter/show/wkf/asset/env/pudacuoEnv/model/dev/scenes/old_2017/pudacuoEnv_model_v01_w15.mb";
#     # open file
#     cmds.file(oldFile, force=True, open=True)
#     cmds.select(cl=True)
#
#     # data;
#
#     }
#
#
#     #load tane new
#     {
#     print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!after convert"
#     oldFile = "/netapp/dexter/show/wkf/asset/env/pudacuoEnv/model/dev/scenes/old_2017/pudacuoEnv_model_v01_w15.mb";
#     # open file
#     cmds.file(oldFile, force=True, open=True)
#
#
#     #newFile = "/home/dohyeon.yang/Desktop/test.mb"
#     #cmds.file(newFile, force=True, open=True)
#
#
#
#     # quit
#     os._exit( 0 )