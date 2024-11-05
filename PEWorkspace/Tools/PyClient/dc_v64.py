# This is UI representation of DC_HANDLEV4 data component

# Import Pmw from this directory tree.
import sys
import socket
import select

sys.path[:0] = ['../../..']

import Tkinter
import Pmw

import bootstrap
import dc_handlev4

class V64_UI:
    def __init__(self, parent, dict, parentName, tabWidth, parentDC):
        self.dict = dict
        self.parent = parent
        self.frame = Tkinter.Frame(parent)
        self.parentDC = parentDC
        self.uis = {}
        i = 0
        for entry in xrange(dict['v64']['size']):
            self.uis[entry] = Pmw.Counter(self.frame,
                labelpos = 'w',
                label_text = str(entry),
                label_justify = 'left',
                entryfield_value = str(dict['v64'][i]),
                datatype = {'counter' : 'real', 'separator' : '.'},
                # entryfield_validate = {'validator' : 'real',
                    # 'separator' : '.'},
                entryfield_validate = self._custom_validate,
                increment = 0.1)
            i += 1
            self.uis[entry].pack(side=Tkinter.TOP, fill='x', expand = 1)
        self.frame.pack(side=Tkinter.TOP, fill='x', expand=1)
    def dataAsString(self):
        res = 'size=%d,' % self.dict['v64']['size']
        for i in xrange(self.dict['v64']['size']):
            res = res + '[%d]=%s,' % (i+1, self.uis[i].get()) #lua indices start at 1
        return res
    def _custom_validate(self, text):
        self.execute()
        return 1
    def execute(self):
        self.parentDC.valueChanged()
    def destroy(self):
        pass
class DC_V64_UI:
    def __init__(self, parent, dict, parentName, tabWidth):
        self.dict = dict
        self.parent = parent
        self.frame = Pmw.Group(parent, tag_text='DC_V64 Modifier')
        self.frame.pack(fill = 'x', expand = 1)
        
        self.uis = {}
        self.autoSyncVar = Tkinter.IntVar()
        
        self.uis['v64_ui'] = V64_UI(self.frame.interior(), dict['data'], parentName, tabWidth, self)
        self.uis['Sync'] = Tkinter.Button(self.frame.interior(), text="Sync", fg="red", command=self.btnSyncCmd)
        self.uis['Sync'].pack(side=Tkinter.LEFT, expand=1, fill='x')
        self.uis['save'] = Tkinter.Button(self.frame.interior(), text="Save", fg="red", command=self.btnSaveCmd)
        self.uis['save'].pack(side=Tkinter.LEFT, expand=1, fill='x')
        self.uis['set'] = Tkinter.Button(self.frame.interior(), text="Set", fg="red", command=self.btnSetCmd)
        self.uis['set'].pack(side=Tkinter.LEFT, expand=1, fill='x')
        self.autoSyncCB = Tkinter.Checkbutton(
            self.frame.interior(), text="AutoSync",
            variable=self.autoSyncVar,
            command=self.setAutoSync)
        self.autoSyncCB.pack(side=Tkinter.LEFT, expand=1, fill='x')
        #self.frame.pack(side=Tkinter.TOP, fill = 'x', expand = 1)
    def dataAsString(self):
        return '{%s}'%self.uis['v64_ui'].dataAsString()
    def setAutoSync(self):
        print self.autoSyncVar.get()
    def valueChanged(self):
        if self.autoSyncVar.get():
            self.btnSetCmd()
    def btnSetCmd(self):
        text = 'DataComponentProtocols.setDataComponent(%s, %s)' % (dc_handlev4.handleAsString(self.dict['handle']), self.dataAsString())
        bootstrap.BootStrap.PyClient.sendCommand(text)
    def btnSyncCmd(self):
        pass
    def btnSaveCmd(self):
        text = 'DataComponentProtocols.saveDataComponent(%s)' % dc_handlev4.handleAsString(self.dict['handle'])
        bootstrap.BootStrap.PyClient.sendCommand(text)
    def destroy(self):
        pass
        #self.messagebar.destroy()
