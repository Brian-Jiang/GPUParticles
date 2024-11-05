import sys
import socket
import select
sys.path[:0] = ['../../..']

#data components
import dc_handlev4
import dc_v64
import bootstrap
import Tkinter
import Pmw
import os
import os.path
import peuuid
import exceptions
import assetmanagercommon

class MeshAssetActions:
    def __init__(self, parent, dict):
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        
        self.frame = Tkinter.Frame(parent)
        self.frame.pack(side=Tkinter.TOP, fill ='both', expand=1)

        self.createButton = Tkinter.Button(self.frame, text="View", fg="red", command=self.createButton)
        self.createButton.pack(side=Tkinter.LEFT, expand = 1)
    
    def createButton(self):
        print "Sending CREATE_MESH"
        cmd =       'handler = getGameObjectManagerHandle(l_getGameContext())\n'
        cmd = cmd + 'evt = root.PE.Events.Event_CREATE_MESH.Construct(l_getGameContext(), "%s", "%s", 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, peuuid.constructPEUUID(0, 0, 0, 0))\n' % (self.dict['filename'], self.dict['package'])
        cmd = cmd + 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n'

        bootstrap.BootStrap.PyClient.sendCommand(cmd)
    def destroy(self):
        self.frame.destroy()
class SkeletonAssetActions:
    def __init__(self, parent, dict):
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        
        self.frame = Tkinter.Frame(parent)
        self.frame.pack(side=Tkinter.TOP, fill ='both', expand=1)

        self.createButton = Tkinter.Button(self.frame, text="View", fg="red", command=self.createButton)
        self.createButton.pack(side=Tkinter.LEFT, expand = 1)
    
    def createButton(self):
        print "Sending CREATE_SKELETON"
        cmd =       'handler = getGameObjectManagerHandle(l_getGameContext())\n'
        cmd = cmd + 'evt = root.PE.Events.Event_CREATE_SKELETON.Construct(l_getGameContext(), "%s", "%s", 0, 0, 0, 1,0,0, 0,1,0, 0,0,1, peuuid.constructPEUUID(0, 0, 0, 0))\n' % (self.dict['filename'], self.dict['package'])
        cmd = cmd + 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n'

        bootstrap.BootStrap.PyClient.sendCommand(cmd)
    def destroy(self):
        self.frame.destroy()
class AnimationSetAssetActions:
    def __init__(self, parent, dict):
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        
        self.frame = Tkinter.Frame(parent)
        self.frame.pack(side=Tkinter.TOP, fill ='both', expand=1)

        self.createButton = Tkinter.Button(self.frame, text="View", fg="red", command=self.createButton)
        self.createButton.pack(side=Tkinter.LEFT, expand = 1)
    
    def createButton(self):
        print "Sending CREATE_ANIMSET"
        cmd =       'handler = getGameObjectManagerHandle(l_getGameContext())\n'
        cmd = cmd + 'evt = root.PE.Events.Event_CREATE_ANIM_SET.Construct(l_getGameContext(), "%s", "%s", peuuid.constructPEUUID(0, 0, 0, 0))\n' % (self.dict['filename'], self.dict['package'])
        cmd = cmd + 'root.PE.Components.Component.SendEventToHandle(handler, evt)\n'

        bootstrap.BootStrap.PyClient.sendCommand(cmd)
    def destroy(self):
        self.frame.destroy()
class LevelAssetActions:
    def __init__(self, parent, dict):
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        
        self.frame = Tkinter.Frame(parent)
        self.frame.pack(side=Tkinter.TOP, fill ='both', expand=1)

        self.createButton = Tkinter.Button(self.frame, text="View", fg="red", command=self.createButton)
        self.createButton.pack(side=Tkinter.LEFT, expand = 1)
    
    def createButton(self):
        print "Sending command to load level"
        cmd =       'LevelLoader.loadLevel("%s", "%s")\n'% (self.dict['filename'], self.dict['package'])
        bootstrap.BootStrap.PyClient.sendCommand(cmd)
    def destroy(self):
        self.frame.destroy()
# frame
#   new package frame
#     new package entry
#     new package btn
#   packageFrame
#     packageList
#     packageContents
#     assetTypeContents
class AssetManager:
    def __init__(self, parent, dict):
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        self.curPackage = None
        self.assetActions = []
        self.frame = Tkinter.Frame(parent)
        self.frame.pack(side=Tkinter.TOP, fill ='both', expand=1)

        self.newPackageFrame = Tkinter.Frame(self.frame)
        self.newPackageFrame.pack(side=Tkinter.TOP, fill ='x')
        
        self.entry = Pmw.EntryField(
            self.newPackageFrame,
            labelpos = 'w',
            label_text = "new package name",
            value = "",
            #validate = None,
            #command = self.execute
        )
        self.entry.pack(side=Tkinter.LEFT, fill='x', expand = 1)
        
        self.createButton = Tkinter.Button(self.newPackageFrame, text="Generate", fg="red", command=self.createButton)
        self.createButton.pack(side=Tkinter.LEFT, expand = 1)
 
        #packageFrame
        self.packageFrame = Tkinter.Frame(self.frame, bg='blue')
        self.packageFrame.pack(side=Tkinter.TOP, fill ='x')
        
        # Create the packageList.
        self.packageList = Pmw.ScrolledListBox(self.packageFrame,
            items=(),
            labelpos='nw',
            label_text='Packages',
            listbox_height = 10,
            selectioncommand=self.packageSelectionCommand,
            dblclickcommand=self.defCmd,
            usehullsize = 1,
            hull_width = 200,
            hull_height = 400,
        )
        self.packageList.pack(side = Tkinter.LEFT, fill = 'y', padx = 5)#, pady = 5)

        # Create the packageContents.
        self.packageContent = Pmw.ScrolledListBox(self.packageFrame,
            items=(),
            labelpos='nw',
            label_text='Package Content',
            listbox_height = 10,
            selectioncommand=self.packageContentSelectionCommand,
            dblclickcommand=self.defContentCmd,
            usehullsize = 1,
            hull_width = 150,
            hull_height = 400,
        )
        self.packageContent.pack(side = Tkinter.LEFT, fill = 'y', padx = 5)#, pady = 5)

        # Create the packageContents.
        self.assetTypeContents = Pmw.ScrolledListBox(self.packageFrame,
            items=(),
            labelpos='nw',
            label_text='Assets',
            listbox_height = 10,
            selectioncommand=self.packageAssetTypeSelectionCommand,
            dblclickcommand=self.defAssetTypeCmd,
            usehullsize = 1,
            hull_width = 2000,
            hull_height = 400,
        )
        self.assetTypeContents.pack(side = Tkinter.LEFT, fill = 'y', padx = 5)#, pady = 5)

        self.syncPackages()
       
        self.deployEngineButton = Tkinter.Button(self.newPackageFrame, text="Deploy Engine to XBox 360", fg="red", command=self.deployEngineButton)
        self.deployEngineButton.pack(side=Tkinter.LEFT, expand = 1)

        self.deployButton = Tkinter.Button(self.newPackageFrame, text="Deploy to XBox 360", fg="red", command=self.deployButton)
        self.deployButton.pack(side=Tkinter.LEFT, expand = 1)
 
        self.exportButton = Tkinter.Button(self.newPackageFrame, text="Export Package", fg="red", command=self.exportButton)
        self.exportButton.pack(side=Tkinter.LEFT, expand = 1)
 
        self.importButton = Tkinter.Button(self.newPackageFrame, text="Import Package", fg="red", command=self.importButton)
        self.importButton.pack(side=Tkinter.LEFT, expand = 1)
 
        self.actionsFrame = Tkinter.Frame(self.frame)
        self.actionsFrame.pack(side=Tkinter.TOP, fill ='x')
        
        # Create the window excluded from showbusycursor.
        self.errorPopup = Pmw.MessageDialog(self.frame,
        title = 'Error',
        message_text =
            "Error making the directory. make sure the directory doesn't already exist!\n",
        )
        self.errorPopup.withdraw()
        
        #Pmw.setbusycursorattributes(self.errorPopup.component('hull'),
        #    exclude = 1)
    def createButton(self):
        assetsTop = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsOut')
        packagePath = os.path.join(assetsTop, self.entry.get())
        try:
            os.mkdir(packagePath)
        except exceptions.OSError:
            self.errorPopup.show()
        conf = {}
        execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
        for dirname in conf['AssetDirs']:
            try:
                subpath = os.path.join(packagePath, dirname)
                os.mkdir(subpath)
            except exceptions.OSError:
                self.errorPopup.show()
        self.syncPackages()
    def deployButton(self):
       assetmanagercommon.deployPackageToXBox360(self.curPackage)
    def deployEngineButton(self):
       assetmanagercommon.deployEngine("xbox360")
    def exportButton(self):
       assetmanagercommon.exportPackage(self.curPackage)
       self.syncPackages()
        
    def importButton(self):
       assetmanagercommon.importPackage(self.curPackage)
       self.syncPackages()
        
    def packageSelectionCommand(self):
        sels = self.packageList.getcurselection()
        if len(sels) == 0:
            print 'No selection'
        else:
            index = list(self.packageList.get(0, 'end')).index(sels[0])
            self.syncPackageContent(sels[0])
    def defCmd(self):
        sels = self.packageList.getcurselection()
        if len(sels) == 0:
            print 'No selection for double click'
        else:
            print 'Double click:', sels[0]
    def packageContentSelectionCommand(self):
        sels = self.packageContent.getcurselection()
        if len(sels) == 0:
            print 'No selection'
        else:
            index = list(self.packageContent.get(0, 'end')).index(sels[0])
            self.syncAssetsForType(self.curPackage, sels[0])
            
    def defContentCmd(self):
        sels = self.packageContent.getcurselection()
        if len(sels) == 0:
            print 'No selection for double click'
        else:
            print 'Double click:', sels[0]
    def packageAssetTypeSelectionCommand(self):
        sels = self.assetTypeContents.getcurselection()
        if len(sels) == 0:
            print 'No selection'
        else:
            self.clearAssetActions()
            index = list(self.assetTypeContents.get(0, 'end')).index(sels[0])
            if self.curAssetType == 'Meshes':
                self.assetActions.append(MeshAssetActions(self.actionsFrame, {'filename' : sels[0], 'package' : self.curPackage}))
            elif self.curAssetType == 'Skeletons':
                self.assetActions.append(SkeletonAssetActions(self.actionsFrame, {'filename' : sels[0], 'package' : self.curPackage}))
            elif self.curAssetType == 'AnimationSets':
                self.assetActions.append(AnimationSetAssetActions(self.actionsFrame, {'filename' : sels[0], 'package' : self.curPackage}))
            elif self.curAssetType == 'Levels':
                self.assetActions.append(LevelAssetActions(self.actionsFrame, {'filename' : sels[0], 'package' : self.curPackage}))
                
                
    def defAssetTypeCmd(self):
        sels = self.assetTypeContents.getcurselection()
        if len(sels) == 0:
            print 'No selection for double click'
        else:
            print 'Double click:', sels[0]
    def destroyAssetActions(self):
        for a in self.assetActions:
            a.destroy()
        self.assetActions = []
    def syncPackages(self):
        self.curPackage = None
        self.packageList.clear()
        self.packageContent.clear()
        self.assetTypeContents.clear()
        for p in assetmanagercommon.getListOfAvailablePackages():
            self.packageList.insert('end', p)
        for z in assetmanagercommon.getListOfAvailablePackageZips():
            self.packageList.insert('end', z)
    def syncPackageContent(self, packageName):
        self.curPackage = packageName
        self.curAssetType = None
        self.packageContent.clear()
        self.assetTypeContents.clear()
        if not packageName.endswith('.zip'):
            assetsTop = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'AssetsOut')
            packagePath = os.path.join(assetsTop, packageName)
            conf = {}
            execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
            contents = filter(lambda x: x in conf['AssetDirs'], os.listdir(packagePath))
            for c in contents:
                self.packageContent.insert('end', c)
    def syncAssetsForType(self, packageName, assetType):
        self.curAssetType = assetType
        self.assetTypeContents.clear()
        self.destroyAssetActions()
        for a in assetmanagercommon.getListOfAvailableAssetsForPacakgeOfType(packageName, assetType):
            self.assetTypeContents.insert('end', a)
    def clearAssetActions(self):
        for a in self.assetActions: a.destroy()
        self.assetActions = []
    

