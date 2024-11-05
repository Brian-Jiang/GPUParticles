import maya.cmds as cmds
import maya.mel as mel
import maya.OpenMaya as OpenMaya
import maya.OpenMayaAnim as OpenMayaAnim
import maya.mel
import ctypes
import os
import sys
import shutil
from math import fabs
import xparser, cvxporter
import subprocess
import threading
import sys
print repr(sys.path)
sys.path.append(os.path.normpath(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], "Tools")))
print repr(sys.path)
import PyClient
import PyClient.assetmanagercommon
import PyClient.pyclientcommon
import time
from maya import OpenMaya
print OpenMaya.MFileIO
import maya.utils
import peuuid

#mFileIO = OpenMaya.MFileIO()
#mFileIO.importFile("F:/Programming/USC/ArkaneSVN/GameProject/AssetsIn/Maya/Grenade/Grenade.ma", None, 0, "PENameSpace")
def LoaderSelectPacakge():
    s = cmds.textScrollList('packages', query = True, selectItem = True)
    print " Selected Package: ", s
    
    cmds.textScrollList( 'metascripts', edit = True, removeAll = True )
    if (len(s) > 0):
        cmds.textScrollList( 'metascripts', edit = True, 
            append = PyClient.assetmanagercommon.getListOfAvailableAssetsForPacakgeOfType(s[0], "GameObjectMetaScripts") )
    else:
        print "Error: number of selected packages < 0"
    cmds.button( "add_object_btn", edit = True, enable = False)
def LoaderSelectMetaScript():
    s = cmds.textScrollList('metascripts', query = True, selectItem = True)
    print " Selected Meta Script: ", s
    cmds.button( "add_object_btn", edit = True, enable = True)

global selectionLock
selectionLock = None

global g_LoaderDebugOutput
g_LoaderDebugOutput = False

global g_doLiveUpdate
g_doLiveUpdate = False

global g_liveUpdateIPAndPort
g_liveUpdateIPAndPort = ("localhost", 1417)
def saveCurrentPEGameObjectScript(fnTransform,):
    global g_LoaderDebugOutput
    if g_LoaderDebugOutput:
        print 'Dbg: saveCurrentPEGameObjectScript():'
    name = cmds.textField( "editor", query = 1, text = 1)
    if cmds.objExists(name):
        #print "Dbg: Object %s exists. Looking up attribute metaScript" % name
        cmdText = cmds.cmdScrollFieldExecuter("pe_script_editor", query=1, text=1)
        #print "Dbg: Editor Text:", repr(cmdText)
        if 'metaScript' in cmds.listAttr(name, shortNames=1):
            cmds.setAttr('%s.%s'%(name,'metaScript'), cmdText, type='string')
        global g_doLiveUpdate
        if g_doLiveUpdate:
            # check position and orientation
            mMatrix = fnTransform.transformationMatrix()

            # Flip for left-handed coordinate system
            matrix = cvxporter.XExporter.flipMatrix( mMatrix )
            outMatrix = []
            for i in range( 4 ):
                for j in range( 4 ):
                    #print '%.6f' % matrix[i][j]
                    outMatrix.append(matrix[i][j])
            peuuidStr = cmds.getAttr('%s.%s'%(name,'peuuidStr'))
            #print peuuidStr
            outPEUUID = peuuidStr.replace(',', ' ').split()
            #print outPEUUID
            #print repr(cmdText)
            transferCmd = str(repr(cmdText))
            
            wholeCommand = xparser.generateGameObjectLoadingCommand(name.replace(':', '_'), transferCmd[2:-1], "Raw", outMatrix, outPEUUID)
            #print "Sending:", wholeCommand
            global g_liveUpdateIPAndPort
            sent = PyClient.pyclientcommon.sendCommandS(wholeCommand, g_liveUpdateIPAndPort)
            if not sent:
                print "Warning: No Game Detected, try selecting a correct target system and make sure a game is running"
                global g_doLiveUpdate
                g_doLiveUpdate = False
                cmds.checkBox( 'live_update_chkbx', edit=True, value=False )
    
def setSelectedPEGameObject(name, fnTransform):
    cmds.textField( "editor", edit = 1, text = name)
    cmds.cmdScrollFieldExecuter("pe_script_editor", edit=1, clear=1)
    if cmds.objExists(name):
        global g_LoaderDebugOutput
        if g_LoaderDebugOutput:
            print "Dbg: Object exists. Looking up attribute metaScript"
        if 'metaScript' in cmds.listAttr(name, shortNames=1):
            script = cmds.getAttr('%s.%s'%(name,'metaScript'))
            if g_LoaderDebugOutput:
                print script
            if script:
                cmds.cmdScrollFieldExecuter("pe_script_editor", edit=1, text=script)
def DebugOutputChange(val):
    global g_LoaderDebugOutput
    g_LoaderDebugOutput = cmds.checkBox( 'debug_output_chkbx', query=True, value=True )
def LiveUpdateChange(val):
    print "LiveUpdateChange"
    global g_doLiveUpdate
    g_doLiveUpdate = cmds.checkBox( 'live_update_chkbx', query=True, value=True )
def LoaderSelectTargetSystem():
    s = cmds.textScrollList('target_systems', query = True, selectItem = True)
    if len(s) > 0:
        ip,port = PyClient.pyclientcommon.selectTarget(s[0], "") # secodn parameter is custom entry and is '' for now, we need to add a custom ip entry field in level builder
        global g_liveUpdateIPAndPort
        g_liveUpdateIPAndPort = (ip,port)
        print ip, port
class SelectionChecker(threading.Thread):
    Instance = None
    def __init__(self):
        threading.Thread.__init__(self)
        self.curSelection = None
        self.prevTarnsform = None
        self.keepRunning = True
    
    def run(self):
        global selectionLock
        # window = cmds.window( "pyEnginePropertyWindow", title="pyEngine Property Window", widthHeight=(440, 440) , topLeftCorner=[100, 100])
        # scroll = cmds.scrollLayout("_mainScroll", height = 470  )
        # cmds.columnLayout( adjustableColumn=False, columnOffset=("both", 4) )
        # cmds.textField( "editor", width = 100, height = 100)
        # cmds.showWindow( "pyEnginePropertyWindow" )
        while self.keepRunning:
            global g_LoaderDebugOutput
            if g_LoaderDebugOutput:
                print "Thread Run()"
            time.sleep(1.0)
            selectionLock.acquire()
            selectionList = OpenMaya.MSelectionList()
            OpenMaya.MGlobal.getActiveSelectionList( selectionList )
            itList = OpenMaya.MItSelectionList( selectionList, OpenMaya.MFn.kTransform )
            path = OpenMaya.MDagPath()
            name = None
            while not itList.isDone():
                itList.getDagPath( path )
                if path.node().apiType() == OpenMaya.MFn.kTransform:
                    # could be a game object
                    fnTransform = OpenMaya.MFnTransform( path.node() )
                    name = fnTransform.partialPathName()
                    if name.startswith("PEObject"):
                        if g_LoaderDebugOutput:
                            print "Candidate"
                        if self.curSelection != name:
                            self.curSelection = name
                            if g_LoaderDebugOutput:
                                print "Dbg: New selection"
                            
                            maya.utils.executeInMainThreadWithResult(saveCurrentPEGameObjectScript, self.prevTarnsform if self.prevTarnsform != None else fnTransform)
                            maya.utils.executeInMainThreadWithResult( setSelectedPEGameObject, name, fnTransform )
                            continue
                        else:
                            maya.utils.executeInMainThreadWithResult(saveCurrentPEGameObjectScript, fnTransform)
                            self.prevTarnsform = fnTransform
                    else:
                        name = None
                itList.next()
            if name == None: # no game object is selected
                if self.curSelection: # but some object was previously selected
                    maya.utils.executeInMainThreadWithResult(saveCurrentPEGameObjectScript, self.prevTarnsform) # save it
                    maya.utils.executeInMainThreadWithResult( setSelectedPEGameObject, "", self.prevTarnsform ) # set empty selection
                    self.curSelection = None
                            
                    
            selectionLock.release()
def CreateSelectionCheckerThread(arg):
    print "Dbg: Launching Observer thread that will save currently edited script and change selection"
    if SelectionChecker.Instance != None:
        SelectionChecker.Instance.keepRunning = False # stop exisitng thread
    #start new thread
    SelectionChecker.Instance = SelectionChecker()
    SelectionChecker.Instance.start()
def KillSelectionCheckerThread():
    print "Dbg: Killing Observer thread"
    if SelectionChecker.Instance != None:
        SelectionChecker.Instance.keepRunning = False # stop exisitng thread
    
    
def LoaderAddObject(arg):
    p = cmds.textScrollList('packages', query = True, selectItem = True)
    s = cmds.textScrollList('metascripts', query = True, selectItem = True)
    
    print "Loading script:", s[0], "from package:", p[0]
    d = PyClient.assetmanagercommon.runMetaScript(p[0], s[0])
    
    pathFilename = PyClient.assetmanagercommon.appendPathToAssetsIn(d['t']['mayaRep']).replace('\\', '/')
    mFileIO = OpenMaya.MFileIO()
    
    print mFileIO.importFile(pathFilename, None, 0, "PEObject0")
    lastTransformAdded = None
    maxId = -1
    lastPath = None
    itDag = OpenMaya.MItDag( OpenMaya.MItDag.kDepthFirst, OpenMaya.MFn.kTransform )
    while not itDag.isDone():
        path = OpenMaya.MDagPath()
        itDag.getPath( path )
        fnTransform = OpenMaya.MFnTransform( path.node() )
        name = fnTransform.partialPathName()
        if name.startswith("PEObject"):
            id = int(name[len('PEObject'):name.find(':')])
            print "PEObject Detected: ", name, 'ID:', id
            if id > maxId:
                maxId = id
                lastTransformAdded = fnTransform
                lastPath = path
                
        itDag.next()
    if maxId == -1 or not lastTransformAdded:
        print "Error: Could not find PEObject that was added last"
        return
    print "Adding notes to the last transform"
    dagModifier = OpenMaya.MDagModifier()
    print dagModifier
    #notes = dagModifier.createNode('notes')
    if cmds.objExists(name):
        print "Dbg: Object exists. Adding attribute metaScript"
        cmds.select(name)
        cmds.addAttr( shortName='metaScript', dt='string')
        cmds.setAttr('%s.%s'%(name,'metaScript'), d['t']['callerScript'], type='string')
        script = cmds.getAttr('%s.%s'%(name,'metaScript'))
        print repr(script)
        
        cmds.addAttr( shortName='peuuidStr', dt='string')
        peuuidStr = peuuid.conv.regisrtyTo4UInt32()
        if peuuidStr.endswith(','):
            peuuidStr = peuuidStr[:-1]
        cmds.setAttr('%s.%s'%(name,'peuuidStr'), peuuidStr, type='string')
        peuuidStr = cmds.getAttr('%s.%s'%(name,'peuuidStr'))
        print repr(peuuidStr)
    else:
        print "Error: object %s does not exist" % name
 
def LoaderUI():
    global selectionLock
    selectionLock = threading.Lock()
    if cmds.window( "pyengineLoaderUI", exists=True ):
        cmds.deleteUI( "pyengineLoaderUI", window=True )
    if cmds.windowPref( "pyengineLoaderUI", exists=True ):
        cmds.windowPref( "pyengineLoaderUI", remove=True )

    window = cmds.window( "pyengineLoaderUI", title="PrimeEngine Level Builder", widthHeight=(440, 440) , topLeftCorner=[100, 100])
    
    scroll = cmds.scrollLayout("mainScroll", height = 470  )
    
    cmds.columnLayout( adjustableColumn=False, columnOffset=("both", 4) )
    
    
    cmds.rowLayout(numberOfColumns=2, columnWidth=[(1,200), (2, 200)], columnAlign2=('left', 'left'))
    cmds.text( 'label_select_package', label="Select Package", width = 100)
    cmds.text( 'label_select_gameobjmeta', label="Select Meta Script", width = 100)
    cmds.setParent( ".." )
    
    cmds.rowLayout(numberOfColumns=2, columnWidth=[(1,200), (2, 200)], columnAlign2=('left', 'left'))
    cmds.textScrollList( 'packages', width = 200, height = 100, 
        append = PyClient.assetmanagercommon.getListOfAvailablePackages(), 
        selectCommand = LoaderSelectPacakge)
    cmds.textScrollList( 'metascripts', width = 200, height = 100, selectCommand = LoaderSelectMetaScript)
    cmds.setParent( ".." )
    
    cmds.rowLayout(numberOfColumns=2, columnWidth=[(1,200), (2, 200)], columnAlign2=('left', 'left'))
    cmds.textScrollList( 'target_systems', width = 200, height = 50, 
        append = PyClient.pyclientcommon.getTargetSystemList(), 
        selectCommand = LoaderSelectTargetSystem)
    cmds.checkBox( 'live_update_chkbx', label = "Live Update", changeCommand = LiveUpdateChange)
    cmds.setParent( ".." )
    
    cmds.button( "add_object_btn", label="Add Object", enable = 0, command=LoaderAddObject, align="center", width = 150 )
    cmds.button( "test_btn", label="Restart Selection Thread", enable = 1, command=CreateSelectionCheckerThread, align="center", width = 150 )
    
    cmds.rowLayout(numberOfColumns=2, columnWidth=[(1,200), (2, 200)], columnAlign2=('left', 'left'))
    cmds.textField( "editor", width = 100)
    cmds.checkBox( 'debug_output_chkbx', label = "Debug Output", changeCommand = DebugOutputChange)
    cmds.setParent( ".." )
    
    cmds.cmdScrollFieldExecuter("pe_script_editor", text = "", width = 400, height = 300)
    cmds.showWindow( "pyengineLoaderUI" )
    
    CreateSelectionCheckerThread(1)