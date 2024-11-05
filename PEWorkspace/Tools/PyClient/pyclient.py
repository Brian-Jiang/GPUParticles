# setup paths for pyengine modules

import sys, os


sys.path.append(os.environ['PYENGINE_WORKSPACE_DIR'] + '/Tools')
print "sys.path:", sys.path


from Tkinter import *
import Tkinter
import Pmw
import components
import skinviewer
import techniqueviewer
import memoryviewer
import bootstrap
import socket
import select
import launcher
import peuuidui
import assetmanager
import pyclientcommon
from projgenerator import ProjGeneratorUI

class PrintOne:
    def __init__(self, text):
        self.text = text

    def __call__(self, text):
        print self.text, text
class CodeTemplate:
    def __init__(self, frame, caption, textEntry):
        self.tkBtn =  Button(frame, text=caption, fg="red", command=self._produce)
        self.tkBtn.pack(side=TOP)
        self.textEntry = textEntry
    def _produce(self):
        self.produce() # to be defined in subclasses


class CodeTemplate_l_changeRenderMode(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "l_changeRenderMode", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'l_changeRenderMode(l_getGameContext())\n')

class CodeTemplate_l_clientConnectToTCPServer(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, 'l_clientConnectToTCPServer()', textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'root.PE.Components.ClientNetworkManager.l_clientConnectToTCPServer(l_getGameContext(), "127.0.0.1", 0)\n')

class CodeTemplate_OutputDebugString(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "OutputDebugString", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'outputDebugString("")\n')

class CodeTemplate_CreateSoldier(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "CreateSoldier", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'handler = getGameObjectManagerHandle()\n')
        self.textEntry.insert(INSERT, '--Soldier mesh types: 0 = SOLDIER, 1 = TRENCHCOAT\n')
        self.textEntry.insert(INSERT, '--make_CreateSoldierEvtData("<soldiername>", <meshtype>, <guntype>, <x>, <y>, <z>)\n')
        self.textEntry.insert(INSERT, 'evt = make_CreateSoldierEvtData("Scott", 0, GunTypes.M98, 0, 100, 0)\n')
        self.textEntry.insert(INSERT, 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n')
        
class CodeTemplate_CreateLight(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "CreateLight", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, '--Light Types: point, directional, spot\n')
        self.textEntry.insert(INSERT, 'handler = getGameObjectManagerHandle()\n')
        self.textEntry.insert(INSERT, '--make_CreateLightEvtData("<lightname>", <lighttype>, <x>, <y>, <z>)\n')
        self.textEntry.insert(INSERT, 'evt = make_CreateLightEvtData("MyLight", "directional", 0, 100, 0)\n')
        self.textEntry.insert(INSERT, 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n')
        
class CodeTemplate_CreateScenery(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "CreateScenery", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'handler = getGameObjectManagerHandle()\n')
        self.textEntry.insert(INSERT, '--make_CreateSceneryEvtData("<sceneryname>", GOTypes.<GOType>, <x>, <y>, <z>)\n')
        self.textEntry.insert(INSERT, 'evt = make_CreateSceneryEvtData("OldCar", GOTypes.Car, 0, 100, 0)\n')
        self.textEntry.insert(INSERT, 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n')
        
class CodeTemplate_CreateJunk(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "CreateJunk", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'handler = getGameObjectManagerHandle()\n')
        self.textEntry.insert(INSERT, '--make_CreateJunkEvtData("<junkname>", <MechComponentType>, <x>, <y>, <z>)\n')
        self.textEntry.insert(INSERT, 'evt = make_CreateJunkEvtData("MyJunk", "engine", 0, 100, 0)\n')
        self.textEntry.insert(INSERT, 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n')
        
class CodeTemplate_CreateNavPoint(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "CreateNavPoint", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'handler = getGameObjectManagerHandle()\n')
        self.textEntry.insert(INSERT, '--make_CreateNavPointEvtData(<x>, <y>, <z>, <type>)\n')
        self.textEntry.insert(INSERT, 'evt = make_CreateNavPointEvtData(0, 100, 0, "cover")\n')
        self.textEntry.insert(INSERT, 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n')
        
class CodeTemplate_CreatePlayer(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "CreatePlayer", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'handler = getGameObjectManagerHandle()\n')
        self.textEntry.insert(INSERT, '--make_CreatePlayerEvtData("<playername>", <x>, <y>, <z>)\n')
        self.textEntry.insert(INSERT, 'evt = make_CreatePlayerEvtData("Playa", 0, 100, 0)\n')
        self.textEntry.insert(INSERT, 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n')
        
class CodeTemplate_CreateMech(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "CreateMech", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'handler = getGameObjectManagerHandle()\n')
        self.textEntry.insert(INSERT, '--make_CreateMechObjEvtData("<name>", <x>, <y>, <z>)\n')
        self.textEntry.insert(INSERT, 'evt = make_CreateMechObjEvtData("MechJohn", 0, 100, 0)\n')
        self.textEntry.insert(INSERT, 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n')
        
class CodeTemplate_GetNearest(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "GetNearest", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'hObj = getClosestByGOType(GOTypes.<GOType>)\n')

class CodeTemplate_ApplySkinShader(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "ApplySkinShader", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'applySkinEffectGeneral(hObj, "<effect name>")\n')
        
class CodeTemplate_RemoveMeshShaders(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "RemoveMeshShaders", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'popMeshEffects(hObj)\n')
        
class CodeTemplate_RemoveSkinShaders(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "RemoveSkinShaders", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'popSkinEffects(hObj)')
        
class CodeTemplate_SendEvt_MOVE(CodeTemplate):
    def __init__(self, frame, textEntry):
        CodeTemplate.__init__(self, frame, "SendEvt_MOVE", textEntry)
    def produce(self):
        #self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(INSERT, 'handler = "<handle>"\n')
        self.textEntry.insert(INSERT, 'evt = root.PE.Events.Event_MOVE.Construct(<x>, <y>, <z>)\n')
        self.textEntry.insert(INSERT, 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n')

class PyClient:
    Instance = None
    def __init__(self, master):
        # master
        #   combobox target machine
        #   notebook
        #       commandPage
        #           choiceFrame
        #               CodeTemplate_OutputDebugString
        #           genericFrame
        #               textEntry
        #           btnFrame
        #       ComponentViewer
        bootstrap.BootStrap.PyClient = self
        self.parent = master
        
        # Create and pack the dropdown ComboBox.
        
        conf = {}
        execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
        
        self.dropdown = Pmw.ComboBox(master,
                label_text = "Target System (GlobalConfig\Dirs.py['TargetSystems']):",
                labelpos = 'nw',
                selectioncommand = self.selectTarget,
                scrolledlist_items = conf['TargetSystems'],
        )
        self.dropdown.selectitem(0)
        self.selectTarget(self.dropdown.get())
        self.dropdown.pack(side = 'top', anchor = 'n',
                fill = 'x', expand = 1, padx = 8, pady = 8)
        self.customEntry = Pmw.EntryField(
            master,
            labelpos = 'nw',
            label_text = "(Custom Address):Port",
            value = "",
            #validate = None,
            command = self.selectCustomTarget
        )
        self.customEntry.pack(side='top', anchor = 'n', fill='x', expand = 1, padx = 8, pady = 8)

        
        self.notebook = Pmw.NoteBook(master,
            tabpos = 'n',
            createcommand = PrintOne('Create'),
            lowercommand = PrintOne('Lower'),
            raisecommand = PrintOne('Raise'),
            hull_width = 720,
            hull_height = 540,
        )
        self.commandPage = self.notebook.add('CommandPage')
        self.componentViewerPage = self.notebook.add('Component Viewer')
        
        self.componentViewer = components.ComponentViewer(self.componentViewerPage)
        #self.componentViewerPage.pack(side=LEFT, fill='both', expand=1)
        
        self.skinViewerPage = self.notebook.add('Skin Viewer')
        
        self.skinViewerPageNum = 2
        self.skinViewer = skinviewer.SkinViewer(self.skinViewerPage, {'handle' : [0,1,2,3], 
            'anims' : {
                'size':2, 
                0: {'name':'name0'},
                1: {'name:':'name1'}
            }
        })
        
        self.techniqueViewerPage = self.notebook.add('Technique Viewer')
        self.techniqueViewerPageNum = 3
        self.techniqueViewer = techniqueviewer.TechniqueViewer(self.techniqueViewerPage,
            {}
        )

        self.memoryViewerPage = self.notebook.add('Memory Viewer')
        self.memoryViewerPageNum = 4
        self.memoryViewer = memoryviewer.MemoryViewer(self.memoryViewerPage,
            {}
        )
        
        self.launcherPage = self.notebook.add('Launcher')
        self.launcherPageNum = 5
        self.launcher = launcher.Launcher(self.launcherPage,
            {}
        )
        
        self.peuuiduiPage = self.notebook.add('PyEngine UUID')
        self.peuuiduiPageNum = 6
        
        peuuidui.PEUUIDUI(self.peuuiduiPage,
            {}
        )
        
        self.assetManagerPage = self.notebook.add('Asset Manager')
        self.assetManagerPageNum = 7
        
        assetmanager.AssetManager(self.assetManagerPage,
            {}
        )
        
        self.projGeneratorPage = self.notebook.add('Project Generator')
        self.projGeneratorPageNum = 8
        
        ProjGeneratorUI(self.projGeneratorPage,
            {}
        )
        
        
        
        self.choiceFrame = Frame(self.commandPage) # create frame in master
        self.choiceFrame.pack(side=LEFT)
        
        self.debugFrame = Frame(self.choiceFrame) # create frame in master
        self.debugFrame.pack(side=TOP)
        
        self.createFrame = Frame(self.choiceFrame) # create frame in master
        self.createFrame.pack(side=TOP)
        
        self.shaderFrame = Frame(self.choiceFrame) # create frame in master
        self.shaderFrame.pack(side=TOP)
        
        self.moveFrame = Frame(self.choiceFrame) # create frame in master
        self.moveFrame.pack(side=TOP)
        
        side=Tkinter.LEFT
        
        
        
        self.genericFrame = Frame(self.commandPage) # create frame in master
        self.genericFrame.pack(side=LEFT)
        
        
        self.btnFrame = Frame(self.genericFrame) # create frame in self.genericFrame
        self.btnFrame.pack(side=BOTTOM)
        
        # create button in self.btnFrame
        self.button = Button(self.btnFrame, text="QUIT", fg="red", command=master.quit)
        self.button.pack(side=LEFT, expand=True, fill=X)

        # create button in self.btnFrame
        self.hi_there = Button(self.btnFrame, text="Send", command=self.say_hi)
        self.hi_there.pack(side=LEFT, expand=True, fill=X) 
        # create button in self.btnFrame
        self.clear = Button(self.btnFrame, text="Clear", command=self.clear)
        self.clear.pack(side=LEFT, expand=True, fill=X) 
        
        self.textEntry = Text(self.genericFrame)
        self.textEntry.pack(side=BOTTOM)
 
        CodeTemplate_l_clientConnectToTCPServer(self.debugFrame, self.textEntry)
        CodeTemplate_l_changeRenderMode(self.debugFrame, self.textEntry)
        CodeTemplate_OutputDebugString(self.debugFrame, self.textEntry)
        CodeTemplate_CreateSoldier(self.createFrame, self.textEntry)
        CodeTemplate_CreateLight(self.createFrame, self.textEntry)
        CodeTemplate_CreateScenery(self.createFrame, self.textEntry)
        CodeTemplate_CreateJunk(self.createFrame, self.textEntry)
        CodeTemplate_CreateNavPoint(self.createFrame, self.textEntry)
        CodeTemplate_CreatePlayer(self.createFrame, self.textEntry)
        CodeTemplate_CreateMech(self.createFrame, self.textEntry)
        CodeTemplate_GetNearest(self.debugFrame, self.textEntry)
        CodeTemplate_ApplySkinShader(self.shaderFrame, self.textEntry)
        CodeTemplate_RemoveMeshShaders(self.shaderFrame, self.textEntry)
        CodeTemplate_RemoveSkinShaders(self.shaderFrame, self.textEntry)
        CodeTemplate_SendEvt_MOVE(self.moveFrame, self.textEntry)
        # Pack the notebook last so that the buttonbox does not disappear
        # when the window is made smaller.
        self.notebook.pack(fill = 'both', expand = 1, padx = 5, pady = 5)
        
        

   
    def createOnTheFly(self):
        dialog = Pmw.MessageDialog(self.parent,
            title = 'On the fly dialog',
            defaultbutton = 0,
            buttons = ('Yes', 'No'),
            message_text = 'PyEngine application is not running at this point. Do you want to launch a new instance of the application?')
        dialog.iconname('PyEngine Notification')
        result = dialog.activate()

        if result == 'Yes':
            conf = {}
            execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
            
            execConfig = conf['ViewerConfiguration']
            execDir = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], execConfig)
            execDirectory = conf['ViewerDirectory']
            execLaunchDir = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], execDirectory)
            execName = conf['ViewerExecutable']
            
            print "copying %s to %s" % (os.path.join(os.path.normpath(execDir), execName), os.path.join(os.path.normpath(execLaunchDir), execName))
            os.system('copy "%s" "%s"' % (os.path.join(os.path.normpath(execDir), execName), os.path.join(os.path.normpath(execLaunchDir), execName)))
            prevPath = os.getcwd();
            
            print "changing current folder from %s to %s" % (prevPath, os.path.normpath(execLaunchDir))
            os.chdir(os.path.normpath(execLaunchDir))
            print 'launching "%s"' % execName
            os.spawnv(os.P_NOWAIT, execName, (execName,"-a", "-b", "-c"))
            print "changing current back to %s" % prevPath
            os.chdir(prevPath)
        return result
    def waitToLaunchDialog(self):
        dialog = Pmw.MessageDialog(self.parent,
            title = 'Wait',
            defaultbutton = 0,
            buttons = ('Done', 'Cancel'),
            message_text = 'Press Done when the PyEngine application has loaded')
        dialog.iconname('PyEngine Wait Notification')
        result = dialog.activate()

        return result
    @staticmethod
    def sendCommandS(text):
        s = socket.socket()
        tryConnect = True
        connected = False
        while tryConnect:
            try:
                print "addr: ", "localhost"
                s.connect(("localhost", 1417))
                connected = True
                tryConnect = False
            except:
                print "NO GAME RUNNING"
        if connected:
            if not text.endswith('\n'): text = text + '\n'
            lines = text.splitlines()
            n = len(lines)
            s.send(str(n) + '\n')
            s.send(text)
            s.close()
            
    def sendCommand(self, text, createApp = False):
        print 'sendCommand(',text, '):'
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print "addr: ", self.targetIP
        s.connect((self.targetIP, self.targetPort)) #before was just 'localhost'
        if not text.endswith('\n'): text= text + '\n'
        
        lines = text.splitlines()
        n = len(lines)
        s.send(str(n) + '\n')
        s.send(text)
        s.close()
        
        return
        s = socket.socket()
        tryConnect = True
        connected = False
        while tryConnect:
            try:
                print "addr: ", self.targetIP
                s.connect((self.targetIP, self.targetPort))
                connected = True
                tryConnect = False
            except:
                if createApp:
                    answer = self.createOnTheFly()
                    tryConnect = answer == 'Yes'
                    if answer == 'Yes':
                        self.waitToLaunchDialog()
                tryConnect = False
        if connected:
            if not text.endswith('\n'): text = text + '\n'
            lines = text.splitlines()
            n = len(lines)
            s.send(str(n) + '\n')
            s.send(text)
            s.close()
    def say_hi(self):
        self.sendCommand(self.getText())
        
    def executeCommandWithReturnString(self, cmd):
        print 'executeCommandWithReturnString(', cmd, '):'
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print "addr: ", self.targetIP
        s.connect((self.targetIP, self.targetPort)) #before was just 'localhost'
        if not cmd.endswith('\n'): cmd = cmd + '\n'
        
        s.send("1\n%s" % (cmd,))
        
        inputready,outputready,exceptready = select.select([s],[],[]) 
        print 'selecting socket for input; res:', inputready
        answ = ''
        if len(inputready) > 0:
            while 1:
                answ = answ + s.recv(1024*1024)
                #print 'answ', answ
                lines = answ.splitlines()
                numLinesNeeded = int(lines[0]) - 1
                print 'Lines needed', numLinesNeeded, 'have', len(lines)
                if len(lines)-1 >= numLinesNeeded and answ.endswith('\n'):
                    break
            print 'input received; %d bytes' % len(answ)
            #print answ
        s.close()
        return answ[answ.find('\n')+1:]
    def clear(self):
        self.textEntry.delete('1.0', 'end') 
    def getLines(self):
        all = self.getText()
        lines = all.split('\n')
        return lines
    def getText(self):
        res = ''
        for tup in self.textEntry.dump(index1 = '1.0', index2 = 'end', command = None, text = True):
            res = res + tup[1]
        return res
    def do_TextEntered(self):
        print 'Text entered'
    def SetSkinViewerPage(self, dict):
        self.notebook.selectpage(self.skinViewerPageNum)
        self.skinViewer.SelectSkin(dict)
    def SetTechniqueViewerPage(self, dict):
        self.notebook.selectpage(self.techniqueViewerPageNum)
        self.techniqueViewer.SelectTechnique(dict)
    def selectTarget(self, target):
        self.targetIP, self.targetPort = pyclientcommon.selectTarget(self.dropdown.get() if hasattr(self, 'dropdown') else '', self.customEntry.get() if hasattr(self, 'customEntry') else '')
        print 'Target IP, Port:', self.targetIP, self.targetPort
    def selectCustomTarget(self):
        self.dropdown.selectitem(-1)
        self.targetIP, self.targetPort = pyclientcommon.selectTarget(self.dropdown.get() if hasattr(self, 'dropdown') else '', self.customEntry.get() if hasattr(self, 'customEntry') else '')
        print 'Target IP, Port:', self.targetIP, self.targetPort

sys.path.append(os.environ['PYENGINE_WORKSPACE_DIR'] + '/Tools')
print "sys.path:", sys.path


os.system("XbCp test")
components.InitAdditionalActionMap()

root = Tk(className='PyClient')

app = PyClient(root)

root.mainloop()