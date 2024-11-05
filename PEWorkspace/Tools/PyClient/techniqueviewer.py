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
class SingleAnim_UI:
    def __init__(self, parent, dict, parentName, tabWidth, entry, name):
        self.dict = dict
        self.parent = parent
        self.entry = entry
        self.uis = {}
        i = 0
        self.uis[str(entry)+'_frame'] = Tkinter.Frame(self.parent)
        self.uis[entry] = Pmw.ScrolledField( self.uis[str(entry)+'_frame'],
            entry_width = tabWidth / 10,
            entry_relief='groove', labelpos = 'w',
            label_text = str(entry),
            text = name)
        self.uis[entry].pack(side=Tkinter.LEFT)
        
        self.uis[str(entry)+'_btn'] =  Tkinter.Button(self.uis[str(entry)+'_frame'], text="Play", fg="red", command=self.setBtn)
        self.uis[str(entry)+'_btn'].pack(side=Tkinter.RIGHT)
        self.uis[str(entry)+'_frame'].pack(side=Tkinter.TOP, fill='x', expand=1)
    def setBtn(self):
        AnimPlayer_UI.Instance.setAnim(self.entry)
class AnimPlayer_UI:
    def __init__(self, parent, dict, parentName, tabWidth):
        AnimPlayer_UI.Instance = self
        self.dict = dict
        self.parent = parent
        self.tabWidth = tabWidth
        self.frame = Tkinter.Frame(parent)
        self.uis = {}
        self.frame.pack(side=Tkinter.TOP, fill='x', expand=1)
    def setAnim(self, num):
        print num
        SkinViewer.Instance.setAnimation(num)
    def SelectSkin(self, dict):
        self.dict = dict
        i = 0
        #find data component that stores animation speeds
        speedSet = None
        nameSet = None
        
        for dc in dict['dataComponents']:
            if dc['DCTypeOverride'] == "DCA_ANIM_SPEED_SET":
                speedSet = dc
            elif dc['DCTypeOverride'] == "DCA_ANIM_NAME_SET":
                nameSet = dc

        print nameSet
        if speedSet:
            size = speedSet['data']['v64']['size']
            for entry in xrange(size):
                name = nameSet['data']['string64'][entry]
                print name
                self.uis['single_anim_ui'+str(entry)] = SingleAnim_UI(self.frame, 
                "AnimPlayer_UI", 
                "AnimPlayer_UI", self.tabWidth, i, name)
                i += 1
        self.frame.pack(side=Tkinter.TOP, fill='x', expand=1)
    def dataAsString(self):
        res = 'size=%d,' % self.dict['v64']['size']
        for i in xrange(self.dict['v64']['size']):
            res = res + '[%d]=%s,' % (i+1, self.uis[i].get()) #lua indices start at 1
        return res
    def destroy(self):
        pass

class TechniqueViewer:
    def __init__(self, parent, dict):
        self.lists = []
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        self.btnFrame = Tkinter.Frame(parent, bg='blue') # create frame in self.genericFrame
        self.btnFrame.pack(side=Tkinter.BOTTOM, fill ='x')
        
        
        # Create a main PanedWidget with a few panes.
        self.paned = Pmw.PanedWidget(parent,
            orient='horizontal',
            hull_borderwidth = 1,
            hull_relief = 'sunken',
            #hull_width=300,
            #hull_height=400
            )

        #self.uis['anims_ui'] = AnimPlayer_UI(self.boxFrame.interior(), 
        #self.dict, 'SkinViewer', 200)
        
        #name = 'Pane ' + str(self.numPanes)
        self.vspane = self.paned.add('VSPane', min = .1, size = .25)
        self.gspane = self.paned.add('GSPane', min = .1, size = .25)
        self.pspane = self.paned.add('PSPane', min = .1, size = .25)
        self.cspane = self.paned.add('CSPane', min = .1, size = .25)
        self.spespane = self.paned.add('SpeSPane', min = .1, size = .25)
        
        
        self.vsTextEntry = Pmw.ScrolledText(self.vspane,
            borderframe = 1,
            labelpos = 'n',
            label_text='Vertex Shader',
            usehullsize = 1,
            hull_width = 4000,
            hull_height = 3000,
            text_padx = 10,
            text_pady = 10,
            text_wrap='none'
        )
        self.vsTextEntry.pack(side = 'bottom', expand = 1)
        
        self.gsTextEntry = Pmw.ScrolledText(self.gspane,
            borderframe = 1,
            labelpos = 'n',
            label_text='Geometry Shader',
            usehullsize = 1,
            hull_width = 4000,
            hull_height = 3000,
            text_padx = 10,
            text_pady = 10,
            text_wrap='none'
        )
        self.gsTextEntry.pack(side = 'bottom', expand = 1)

        self.psTextEntry = Pmw.ScrolledText(self.pspane,
            borderframe = 1,
            labelpos = 'n',
            label_text='Pixel Shader',
            usehullsize = 1,
            hull_width = 4000,
            hull_height = 3000,
            text_padx = 10,
            text_pady = 10,
            text_wrap='none'
        )
        self.psTextEntry.pack(side = 'bottom', expand = 1)

        self.csTextEntry = Pmw.ScrolledText(self.cspane,
            borderframe = 1,
            labelpos = 'n',
            label_text='Compute Shader',
            usehullsize = 1,
            hull_width = 4000,
            hull_height = 3000,
            text_padx = 10,
            text_pady = 10,
            text_wrap='none'
        )
        self.csTextEntry.pack(side = 'bottom', expand = 1)
        
        self.spesTextEntry = Pmw.ScrolledText(self.spespane,
            borderframe = 1,
            labelpos = 'n',
            label_text='SpeS Shader',
            usehullsize = 1,
            hull_width = 4000,
            hull_height = 3000,
            text_padx = 10,
            text_pady = 10,
            text_wrap='none'
        )
        self.spesTextEntry.pack(side = 'bottom', expand = 1)
        
        self.paned.pack(side= 'left', fill = 'both', expand = 1)
        
        
        
        #self.uis['handle_ui'] = dc_handlev4.HANDLE_UI(self.btnFrame, self.dict, "PARENT NAME?", 200)
        
        self.syncButton = Tkinter.Button(self.btnFrame, text="Set", fg="red", command=self.syncButton)
        self.syncButton.pack(expand = 1)
    def setAnimation(self, anim):
        text = 'DataComponentProtocols.setSkinAnim(%s, %s)' % (self.uis['handle_ui'].uis['handle_entry'].get(), anim)
        print text
        bootstrap.BootStrap.PyClient.sendCommand(text)
    def SelectTechnique(self, dict):
        print 'SelectTechnique():'
        print dict
        self.dict = dict
        answ = bootstrap.BootStrap.PyClient.executeCommandWithReturnString(
            'root.PE.Components.Effect.GetTechnique(%s)' % dc_handlev4.handleAsString(dict['handle'])
        )
        dict = eval(answ)
        self.SetTechniqueShaders(dict)
    def SetTechniqueShaders(self, dict):
        #print dict
        self.detailDict = dict
        # self.vsTextEntry = Pmw.ScrolledText(self.vspane,
            # borderframe = 1,
            # labelpos = 'n',
            # label_text='Vertex Shader',
            # usehullsize = 0,
            # hull_width = 1000,
            # hull_height = 800,
            # text_padx = 0,
            # text_pady = 0,
            # text_wrap='none'
        # )
        
        if dict.has_key('vs'):
            self.vsTextEntry.configure(label_text = dict['vs']['filename'])
            self.vsTextEntry.setvalue(dict['vs']['text'])
            self.vsTextEntry.pack(side = 'bottom', expand = 1)
            index = self.vsTextEntry.search(dict['vs']['vsname'], '0.0')
            self.vsTextEntry.see(index)
        # self.vsTextEntry.pack(side = 'bottom', expand = 1)
        
        # self.gsTextEntry = Pmw.ScrolledText(self.gspane,
            # borderframe = 1,
            # labelpos = 'n',
            # label_text='Geometry Shader',
            # usehullsize = 0,
            # hull_width = 1000,
            # hull_height = 800,
            # text_padx = 0,
            # text_pady = 0,
            # text_wrap='none'
        # )

        if dict.has_key('gs'):
            self.gsTextEntry.configure(label_text = dict['gs']['filename'])
            self.gsTextEntry.setvalue(dict['gs']['text'])
            index = self.gsTextEntry.search(dict['gs']['gsname'], '0.0')
            self.gsTextEntry.see(index)
        # self.gsTextEntry.pack(side = 'bottom', expand = 1)
            
        # self.psTextEntry = Pmw.ScrolledText(self.pspane,
            # borderframe = 1,
            # labelpos = 'n',
            # label_text='Pixel Shader',
            # usehullsize = 0,
            # hull_width = 1000,
            # hull_height = 800,
            # text_padx = 0,
            # text_pady = 0,
            # text_wrap='none'
        # )
        if dict.has_key('ps'):
            self.psTextEntry.configure(label_text = dict['ps']['filename'])
            self.psTextEntry.setvalue(dict['ps']['text'])
            index = self.psTextEntry.search(dict['ps']['psname'], '0.0')
            self.psTextEntry.see(index)
        #self.psTextEntry.pack(side = 'bottom', expand = 1)

        # self.csTextEntry = Pmw.ScrolledText(self.cspane,
            # borderframe = 1,
            # labelpos = 'n',
            # label_text='Compute Shader',
            # usehullsize = 0,
            # hull_width = 1000,
            # hull_height = 800,
            # text_padx = 0,
            # text_pady = 0,
            # text_wrap='none'
        # )
        
        if dict.has_key('cs'):
            self.csTextEntry.configure(label_text = dict['cs']['filename'])
            self.csTextEntry.setvalue(dict['cs']['text'])
            index = self.csTextEntry.search(dict['cs']['csname'], '0.0')
            self.csTextEntry.see(index)
        #self.csTextEntry.pack(side = 'bottom', expand = 1)
        
        if dict.has_key('spes'):
            self.spesTextEntry.configure(label_text = dict['spes']['filename'])
            self.spesTextEntry.setvalue(dict['spes']['text'])
            index = self.spesTextEntry.search(dict['spes']['spesname'], '0.0')
            self.spesTextEntry.see(index)
        
        #self.paned.pack(side= 'left', fill = 'both', expand = 1)
        
    def syncButton(self):
        print 'Synching..'
        cmd = 'root.PE.Components.Effect.SetTechnique(%s,{' % dc_handlev4.handleAsString(self.dict['handle'])
        if self.detailDict.has_key('vs'):
            cmd = cmd + "vs={"
            res = ''
            for tup in self.vsTextEntry.dump(index1 = '1.0', index2 = 'end', command = None, text = True):
                res = res + tup[1]
            res = res.replace('\n', '\\n')
            cmd = cmd + "text='%s'," % res
            cmd = cmd + "filename='%s'," % self.detailDict['vs']['filename']
            cmd = cmd + '},'
        if self.detailDict.has_key('gs'):
            cmd = cmd + "gs={"
            res = ''
            for tup in self.gsTextEntry.dump(index1 = '1.0', index2 = 'end', command = None, text = True):
                res = res + tup[1]
            res = res.replace('\n', '\\n')
            cmd = cmd + "text='%s'," % res
            cmd = cmd + "filename='%s'," % self.detailDict['gs']['filename']
            cmd = cmd + '},'
        if self.detailDict.has_key('ps'):
            cmd = cmd + "ps={"
            res = ''
            for tup in self.psTextEntry.dump(index1 = '1.0', index2 = 'end', command = None, text = True):
                res = res + tup[1]
            res = res.replace('\n', '\\n')
            cmd = cmd + "text='%s'," % res
            cmd = cmd + "filename='%s'," % self.detailDict['ps']['filename']
            cmd = cmd + '},'
        if self.detailDict.has_key('cs'):
            cmd = cmd + "cs={"
            res = ''
            for tup in self.csTextEntry.dump(index1 = '1.0', index2 = 'end', command = None, text = True):
                res = res + tup[1]
            res = res.replace('\n', '\\n')
            cmd = cmd + "text='%s'," % res
            cmd = cmd + "filename='%s'," % self.detailDict['cs']['filename']
            cmd = cmd + '},'
        if self.detailDict.has_key('spes'):
            cmd = cmd + "spes={"
            res = ''
            for tup in self.spesTextEntry.dump(index1 = '1.0', index2 = 'end', command = None, text = True):
                res = res + tup[1]
            res = repr(res)
            cmd = cmd + "text=%s," % res
            cmd = cmd + "filename='%s'," % self.detailDict['spes']['filename']
            cmd = cmd + '},'
        
        
        cmd = cmd + '})\n'
        #print 'Sending:', cmd
        answ = bootstrap.BootStrap.PyClient.sendCommand(cmd)
######################################################################

# Create demo in root window for testing.
if __name__ == '__main__':
    root = Tkinter.Tk()
    Pmw.initialise(root)
    root.title(title)

    exitButton = Tkinter.Button(root, text = 'Exit', command = root.destroy)
    exitButton.pack(side = 'bottom')
    widget = SkinViewer(root)
    root.mainloop()