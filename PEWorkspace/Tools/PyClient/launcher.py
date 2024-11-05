title = 'Pmw.ScrolledListBox demonstration'

# Import Pmw from this directory tree.
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
import time

class Launcher:
    def selectProcess(self, process):
        print 'Process selected: ' + process
        self.process = process
        #need to refresh platform list
        #delete exisitng one (if any)
        if self.platformDropdown != None:
            self.platformDropdown.destroy()
            
        #Create Platform dropdown
        
        self.platformDropdown = Pmw.ComboBox(self.processFrame,
                label_text = "Platform",
                labelpos = 'nw',
                selectioncommand = self.selectPlatform,
                scrolledlist_items = self.conf['LaunchOptions'][process]['platforms'].keys(),
        )
        
        self.platformDropdown.pack(side=Tkinter.LEFT, fill='x', expand = 1)
        
        self.platformDropdown.selectitem(0)
        self.selectPlatform(self.platformDropdown.get())
        
        self.textEntry.delete('1.0', 'end')
        self.textEntry.insert(Tkinter.INSERT, self.conf['LaunchOptions'][process].get('description', 'No Description Available'))
        
    def selectPlatform(self, platform):
        print 'Platform selected: ' + platform
        self.platform = platform
        #need to refresh build configurations
        #delete exisitng one (if any)
        if self.buildConfDropdown != None:
            self.buildConfDropdown.destroy()
            
        #Create Build Configuration dropdown
        
        self.buildConfDropdown = Pmw.ComboBox(self.processFrame,
                label_text = "Build Configuration",
                labelpos = 'nw',
                selectioncommand = self.selectConfiguration,
                scrolledlist_items = self.conf['LaunchOptions'][self.process]['platforms'][platform], # get list of configurations for the process and platform
        )
        
        self.buildConfDropdown.pack(side=Tkinter.LEFT, fill='x', expand = 1)
        
        self.buildConfDropdown.selectitem(0)
        self.selectConfiguration(self.buildConfDropdown.get())
        
    def selectConfiguration(self, configuration):
        print 'Configuration selected: ' + configuration
        self.buildConfiguration = configuration
    def launchButton(self):
        print "Launching %s for %s with %s build configuration" % (self.process, self.platform, self.buildConfiguration)
        
        execExtension = ''
        if self.platform == 'Win32':
            execExtension = '.exe'
        elif self.platform == 'xbox 360':
            execExtension = '.xex'
        else:
            execExtension = '.exe'
        buildPath = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'Code', 'MilestoneBuild', self.platform, self.buildConfiguration)
        files = os.listdir(buildPath)
        filesToCopy = []
        for f in files:
            if f.lower().startswith(self.process.lower()):
                filesToCopy.append(f)
        if self.platform == 'Win32':
            destDir = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'Code', self.process)
            for f in filesToCopy:
                srcFilePath = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'Code', 'MilestoneBuild', self.platform, self.buildConfiguration, f)
                destPath = os.path.join(destDir, f)
                print "copying %s to %s" % (srcFilePath, destPath)
                os.system('copy "%s" "%s"' % (srcFilePath, destPath))
            
            execName = self.process+execExtension
            prevPath = os.getcwd();
            print "changing current folder from %s to %s" % (prevPath, destDir)
            os.chdir(destDir)
            print 'launching "%s"' % execName
            os.spawnv(os.P_NOWAIT, execName, (execName,"-a", "-b", "-c"))
            print "changing current folder back to %s" % (prevPath)
            os.chdir(prevPath)
        elif self.platform == 'xbox 360':
            #need to deploy the engine and other project lua scripts
            os.system("Deployment\DeployEngineAndAllProjectLuaFiles.bat")
            destDir = os.path.join(self.conf['X360WorkspacePath'], 'Code', self.process)
            for f in filesToCopy:
                if os.path.splitext(f)[-1].startswith('.x'): #for x 360, pdb, ilk, exe are generated and then xex and xdb are generated. but only xex and xdb are needed
                    srcFilePath = os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'Code', 'MilestoneBuild', self.platform, self.buildConfiguration, f)
                    destPath = os.path.join(destDir, f)
                    print "copying %s to %s" % (srcFilePath, destPath)
                    os.system('XbCp /Y /t /f "%s" "%s"' % (srcFilePath, destPath))
            
            execName = self.process+execExtension
            print 'launching "%s" on xbox 360' % os.path.join(destDir, execName)
            os.system("XbReboot %s" % os.path.join(destDir, execName))
        else:
            print "ERROR: can't launch for platform %s, run it through Visual Studio" % self.platform
            
        
    def __init__(self, parent, dict):
        self.lists = []
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        
        self.processFrame = Tkinter.Frame(parent)
        self.processFrame.pack(side=Tkinter.TOP, fill ='x')
        
        #store global configuration from GlobaConfig/Dirs.py
        self.conf = {}
        execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), self.conf)
        
        #dont create platform and build configuration dropdowns yet. They will be recreated each time user chooses process
        #also build configuration dropdown will be recreated every time user selects platform
        self.platformDropdown = None
        self.buildConfDropdown = None

        #Process dropdown
        self.processDropdown = Pmw.ComboBox(self.processFrame,
                label_text = "Process Name",
                labelpos = 'nw',
                selectioncommand = self.selectProcess,
                scrolledlist_items = self.conf['LaunchOptions'].keys(),
        )
        self.processDropdown.pack(side=Tkinter.LEFT, fill='x', expand = 1)
        
        self.textFrame = Tkinter.Frame(parent)
        
        self.textEntry = Tkinter.Text(self.textFrame, width = 600, height = 200)
        self.textEntry.pack(side=Tkinter.TOP)
        
        
        self.processDropdown.selectitem(0)
        self.selectProcess(self.processDropdown.get())
        
        self.btnFrame = Tkinter.Frame(parent)
        self.btnFrame.pack(side=Tkinter.TOP, fill ='x')
                
        # self.entry = Pmw.EntryField(
            # self.btnFrame,
            # labelpos = 'w',
            # label_text = "Level Name",
            # value = ".levela",
            # validate = None,
            # command = self.execute
        # )
        # self.entry.pack(side=Tkinter.LEFT, fill='x', expand = 1)
        
        # self.nv3dvisionVar = Tkinter.IntVar()
       
        # self.nv3dvision = Tkinter.Checkbutton(
           # self.btnFrame, text="NVIDIA 3D Vision",
           # variable=self.nv3dvisionVar)
        
        # self.nv3dvision.pack(side=Tkinter.LEFT, expand=1, fill='x')
        
        self.launchButtonWidget = Tkinter.Button(self.btnFrame, text="Launch", fg="red", command=self.launchButton)
        self.launchButtonWidget.pack(side=Tkinter.LEFT, expand = 1)
        self.textFrame.pack(side=Tkinter.TOP)
        