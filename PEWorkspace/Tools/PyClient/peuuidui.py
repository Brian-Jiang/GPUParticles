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
import peuuid

class PEUUIDUI:
    def __init__(self, parent, dict):
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        
        self.btnFrame = Tkinter.Frame(parent)
        self.btnFrame.pack(side=Tkinter.TOP, fill ='x')
        
                
        self.entry = Pmw.EntryField(
            self.btnFrame,
            labelpos = 'w',
            label_text = "UUID as 4 UInt32",
            value = "",
            #validate = None,
            #command = self.execute
        )
        self.entry.pack(side=Tkinter.LEFT, fill='x', expand = 1)
        
        self.genButton = Tkinter.Button(self.btnFrame, text="Generate", fg="red", command=self.genButton)
        self.genButton.pack(side=Tkinter.LEFT, expand = 1)
        
    def genButton(self):
        self.entry.setentry(peuuid.conv.regisrtyTo4UInt32())
