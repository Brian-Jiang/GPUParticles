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
import assetmanagercommon

class ProjGeneratorUI:
    def __init__(self, parent, dict):
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        
        self.btnFrame = Tkinter.Frame(parent)
        self.btnFrame.pack(side=Tkinter.TOP, fill ='x')
        
        #Process dropdown
        projects = assetmanagercommon.getListOfAvailableProjects()
        self.projDropdown = Pmw.ComboBox(self.btnFrame,
                label_text = "Choose Project to Clone",
                labelpos = 'nw',
                selectioncommand = self.selectProject,
                scrolledlist_items = projects,
        )
        self.projDropdown.pack(side=Tkinter.LEFT, fill='x', expand = 1)
        
        
        self.entry = Pmw.EntryField(
            self.btnFrame,
            labelpos = 'w',
            label_text = "Project Name",
            value = '',
            #validate = None,
            #command = self.execute
        )
        self.entry.pack(side=Tkinter.LEFT, fill='x', expand = 1)
        
        self.genButton = Tkinter.Button(self.btnFrame, text="Clone", fg="red", command=self.genButton)
        self.genButton.pack(side=Tkinter.LEFT, expand = 1)
        if len(projects):
            self.projDropdown.selectitem(0)
        self.selectProject(self.projDropdown.get())
        
    def selectProject(self, project):
        print 'Project selected: ' + project
        self.projectToClone = project
        self.entry.setentry(self.projectToClone+'_Clone')
    
    def genButton(self):
        name = self.entry.get()
        assetmanagercommon.cloneProject(self.projectToClone, name)
