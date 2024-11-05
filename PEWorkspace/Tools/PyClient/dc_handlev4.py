# This is UI representation of DC_HANDLEV4 data component

# Import Pmw from this directory tree.
import sys
import socket
import select
sys.path[:0] = ['../../..']

import Tkinter
import Pmw

import bootstrap

def handleAsString(h):
    return '"\'%s\' 0x%x 0x%x 0x%x"'%tuple(h)
def handleAsNums(h):
    return [int(x, 16) for x in h[1:-1].split(' ')]
class HANDLE_UI:
    def __init__(self, parent, dict, parentName, tabWidth):
        self.dict = dict
        self.parent = parent
        self.frame = Tkinter.Frame(parent)
        self.handleText = handleAsString(dict['handle'])
        
        self.uis = {}
        self.uis['handle_entry'] = Pmw.EntryField(self.frame,
            labelpos = 'w',
            label_text = 'Handle:',
            value = self.handleText,
            validate = None,
            command = self.execute)
        self.uis['handle_entry'].pack(side=Tkinter.TOP, fill='x', expand = 1)
        self.frame.pack(side=Tkinter.TOP, fill='x', expand=1)
    def dataAsString(self):
        return 'handle=%s,' % self.uis['handle_entry'].get()
    def execute(self):
        pass
    def setValue(self, handleTxt):
        self.uis['handle_entry'].setvalue(handleTxt)
    def destroy(self):
        pass
        #self.messagebar.destroy()
class V4_UI:
    def __init__(self, parent, dict, parentName, tabWidth, parentDC):
        self.dict = dict
        self.parent = parent
        self.frame = Tkinter.Frame(parent)
        
        self.uis = {}
        i = 0
        self.parentDC = parentDC
        for entry in (('x_entry', 'X'), ('y_entry','Y'), ('z_entry','Z'), ('w_entry','W')):
            self.uis[entry[0]] = Pmw.Counter(self.frame,
                labelpos = 'w',
                label_text = entry[1],
                label_justify = 'left',
                entryfield_value = str(dict['v4'][i]),
                datatype = {'counter' : 'real', 'separator' : '.'},
                entryfield_validate = self._custom_validate,
                entryfield_command = self.execute,
                increment = 0.1)
            i += 1
            self.uis[entry[0]].pack(side=Tkinter.TOP, fill='x', expand = 1)
        self.frame.pack(side=Tkinter.TOP, fill='x', expand=1)
        
    def _custom_validate(self, text):
        self.execute()
        return 1
    def dataAsString(self):
        return 'x=%s, y=%s, z=%s, w=%s,'% (self.uis['x_entry'].get(), self.uis['y_entry'].get(), self.uis['z_entry'].get(), self.uis['w_entry'].get())
    def execute(self):
        self.parentDC.valueChanged()
    def setValue(self):
        x = float(self.uis['x_entry'].get())
        y = float(self.uis['y_entry'].get())
        z = float(self.uis['z_entry'].get())
        w = float(self.uis['w_entry'].get())
        self.dict['v4'] = [x,y,z,w]
    def destroy(self):
        pass
        #self.messagebar.destroy()
class DC_HANDLEV4_UI:
    def __init__(self, parent, dict, parentName, tabWidth):
        self.dict = dict
        self.parent = parent
        self.frame = Pmw.Group(parent, tag_text='DC_HANDLEV4 Modifier')
        self.frame.pack(fill = 'x', expand = 1)
        
        self.uis = {}
        self.autoSyncVar = Tkinter.IntVar()
        
        self.uis['handle_ui'] = HANDLE_UI(self.frame.interior(), dict['data'], parentName, tabWidth)
        self.uis['v4_ui'] = V4_UI(self.frame.interior(), dict['data'], parentName, tabWidth, self)
        self.uis['Sync'] = Tkinter.Button(self.frame.interior(), text="Sync", fg="red", command=self.btnSyncCmd)
        self.uis['Sync'].pack(side=Tkinter.LEFT, expand=1, fill='x')
        self.uis['set'] = Tkinter.Button(self.frame.interior(), text="Set", fg="red", command=self.btnSetCmd)
        self.uis['set'].pack(side=Tkinter.LEFT, expand=1, fill='x')
        self.autoSyncCB = Tkinter.Checkbutton(
            self.frame.interior(), text="AutoSync",
            variable=self.autoSyncVar,
            command=self.setAutoSync)
        self.autoSyncCB.pack(side=Tkinter.LEFT, expand=1, fill='x')
        #self.frame.pack(side=Tkinter.TOP, fill = 'x', expand = 1)
    def setAutoSync(self):
        print self.autoSyncVar.get()
    def valueChanged(self):
        if self.autoSyncVar.get():
            self.btnSetCmd()
    def dataAsString(self):
        self.uis['v4_ui'].setValue()
        return '{%s %s}'%(self.uis['handle_ui'].dataAsString(), self.uis['v4_ui'].dataAsString())
    
    def btnSyncCmd(self):
        pass
    def btnSetCmd(self):
        text = 'DataComponentProtocols.setDataComponent(%s, %s)' % (handleAsString(self.dict['handle']), self.dataAsString())
        bootstrap.BootStrap.PyClient.sendCommand(text)
    def destroy(self):
        pass
        #self.messagebar.destroy()
