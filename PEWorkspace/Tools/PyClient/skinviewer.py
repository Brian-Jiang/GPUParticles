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
    def __init__(self, parent, dict, parentName, tabWidth, animSet, entry, name):
        self.dict = dict
        self.parent = parent
        self.animSet = animSet
        self.entry = entry
        self.uis = {}
        i = 0
        self.uis[str(entry)+'_frame'] = Tkinter.Frame(self.parent)
        self.uis[entry] = Pmw.ScrolledField( self.uis[str(entry)+'_frame'],
            entry_width = tabWidth / 10,
            entry_relief='groove', labelpos = 'w',
            label_text = str(animSet) + ":" + str(entry),
            text = name)
        self.uis[entry].pack(side=Tkinter.LEFT)
        
        self.uis[str(entry)+'_btn'] =  Tkinter.Button(self.uis[str(entry)+'_frame'], text="Play", fg="red", command=self.setBtn)
        self.uis[str(entry)+'_btn'].pack(side=Tkinter.RIGHT)
        self.uis[str(entry)+'_frame'].pack(side=Tkinter.TOP, fill='x', expand=1)
    def setBtn(self):
        AnimPlayer_UI.Instance.setAnim(self.animSet, self.entry)
    def destroy(self):
        for existingKeys in self.uis.keys():
            self.uis[existingKeys].destroy()
        self.uis = {}
class AnimPlayer_UI:
    def __init__(self, parent, dict, parentName, tabWidth):
        AnimPlayer_UI.Instance = self
        self.dict = dict
        self.parent = parent
        self.tabWidth = tabWidth
        self.frame = Tkinter.Frame(parent)
        self.uis = {}
        self.frame.pack(side=Tkinter.TOP, fill='x', expand=1)
    def setAnim(self, animSet, num):
        print num
        SkinViewer.Instance.setAnimation(animSet, num)
    def SelectSkin(self, dict):
        self.dict = dict
        i = 0
        #find data component that stores animation speeds
        speedSet = None
        nameSet = dict['anims']
        
        size = len(dict['anims'])
        for existringKey in self.uis.keys():
            self.uis[existringKey].destroy()
        self.uis = {}
        for entry in xrange(size):
            name = nameSet[entry]['name']
            animSetIndex = nameSet[entry]['animSetIndex']
            index = nameSet[entry]['index']
            print name
            self.uis['single_anim_ui'+str(entry)] = SingleAnim_UI(self.frame, 
            "AnimPlayer_UI", 
            "AnimPlayer_UI", self.tabWidth, animSetIndex, index, name)
            i += 1
        self.frame.pack(side=Tkinter.TOP, fill='x', expand=1)
    def dataAsString(self):
        res = 'size=%d,' % self.dict['v64']['size']
        for i in xrange(self.dict['v64']['size']):
            res = res + '[%d]=%s,' % (i+1, self.uis[i].get()) #lua indices start at 1
        return res
    def destroy(self):
        for existringKey in self.uis.keys():
            self.uis[existringKey].destroy()
        self.uis = {}

class SkinViewer:
    def __init__(self, parent, dict):
        SkinViewer.Instance = self
        self.lists = []
        self.parentWidget = parent
        self.uis = {}
        self.dict = dict
        self.btnFrame = Tkinter.Frame(parent, bg='blue') # create frame in self.genericFrame
        self.btnFrame.pack(side=Tkinter.LEFT, fill ='x')
        
        #self.boxFrame = Tkinter.Frame(parent, bg='green') # create frame in self.genericFrame
        self.boxFrame = Pmw.ScrolledFrame(parent,
            labelpos = 'n', #label_text = '',
            usehullsize = 1,
            hull_width = 400,
            hull_height = 220,
        )
        self.uis['anims_ui'] = AnimPlayer_UI(self.boxFrame.interior(), 
        self.dict, 'SkinViewer', 200)
        
        self.boxFrame.pack(side=Tkinter.LEFT, fill = 'both', expand = 1)
        
        self.uis['handle_ui'] = dc_handlev4.HANDLE_UI(self.btnFrame, self.dict, "PARENT NAME?", 200)
        
        self.syncButton = Tkinter.Button(self.btnFrame, text="Sync", fg="red", command=self.syncButton)
        self.syncButton.pack()
    def setAnimation(self, animSet, anim):
        text =        'evtH = root.PE.Events.Event_PLAY_ANIMATION.Construct(%s, %s)\n' % (animSet, anim) # construct event
        text = text + 'root.PE.Components.Component.SendEventToHandle(%s, evtH)\n' % self.uis['handle_ui'].uis['handle_entry'].get()
        print text
        bootstrap.BootStrap.PyClient.sendCommand(text)
    def SelectSkin(self, dict):
        self.uis['handle_ui'].setValue(dc_handlev4.handleAsString(dict['handle']))
        
        text = "root.PE.Components.Skeleton.GetSkeleton(%s)" % dc_handlev4.handleAsString(dict['handle'])
        res = bootstrap.BootStrap.PyClient.executeCommandWithReturnString(text)
        dict = eval(res)
        self.uis['anims_ui'].SelectSkin(dict)
    def syncButton(self):
        print 'Synching..'
        s = socket.socket()
        s.connect(('localhost', 8888))
        s.send("1\nsynchComponentsWithPyClient()\n")
        
        print 'sent command. will select now..'
        inputready,outputready,exceptready = select.select([s],[],[]) 
        print 'selecting socket for input; res:', inputready
        if len(inputready) > 0:
            answ = ''
            while not answ.endswith('\n'):
                print "Not all input received. Have only %d bytes" % len(answ)
                answ = answ + s.recv(1024*1024)
            print 'input received; %d bytes' % len(answ)
            #print answ
            dict = eval(answ)
            self.setList(dict[0])
        s.close()
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