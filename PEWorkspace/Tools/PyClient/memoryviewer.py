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

class MemoryViewer:
    def __init__(self, parent, dict):


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
        
        self.reportFrame = Tkinter.Frame(self.boxFrame.interior()) # create frame in self.genericFrame
        self.reportFrame.pack(side=Tkinter.TOP, fill ='x', expand = 1)
    
        self.reportFrame2 = Tkinter.Frame(self.reportFrame) # create frame in self.genericFrame
        self.reportFrame2.pack(side=Tkinter.LEFT)
    
        self.totalAllocated = Tkinter.Label(self.reportFrame2, text="Total Memory Allocated: %d MB")
        self.totalAllocated.pack(side=Tkinter.TOP)
        
        self.total = Tkinter.Label(self.reportFrame2, text=         "Total Memory Used: %d MB", justify = Tkinter.LEFT)
        self.total.pack(side=Tkinter.TOP)
            
        self.frames = []
        self.labelsToDestroy = []
        for x in range(4):
            f = Tkinter.Frame(self.boxFrame.interior()) # create frame in self.genericFrame
            f.pack(side=Tkinter.LEFT, fill ='x', expand = 1)
            self.frames.append(f)
        
        bs = Tkinter.Label(self.frames[0], text="  Block Size  ", justify=Tkinter.RIGHT)
        bs.pack(side=Tkinter.TOP, fill ='x')
        
        nb = Tkinter.Label(self.frames[1], text="  Total Number of Blocks  ", justify=Tkinter.RIGHT)
        nb.pack(side=Tkinter.TOP, fill ='x')
        
        nf = Tkinter.Label(self.frames[2], text="  Blocks Used  ", justify=Tkinter.RIGHT)
        nf.pack(side=Tkinter.TOP, fill ='x')
    
        np = Tkinter.Label(self.frames[3], text="  /% Blocks Used  ", justify=Tkinter.RIGHT)
        np.pack(side=Tkinter.TOP, fill ='x')
        
        self.boxFrame.pack(side=Tkinter.LEFT, fill = 'both', expand = 1)
        
        
        
        self.syncButton = Tkinter.Button(self.btnFrame, text="Sync", fg="red", command=self.syncButton)
        self.syncButton.pack()
    def syncButton(self):
        for l in self.labelsToDestroy:
            l.destroy()
        self.labelsToDestroy = []
        answ = bootstrap.BootStrap.PyClient.executeCommandWithReturnString(
            'CommandServer.memoryReport()'
        )
        dict = eval(answ)
        i = 0
        allocated = 0
        used = 0
        for p in dict['pools']:
            color = "#EFEFEF" if i % 2 != 0 else "#DFDFDF"
            allocated += p['bs'] * p['nb']
            used += p['bs'] * (p['nb']-p['nf'])
            bs = Tkinter.Label(self.frames[0], text=p['bs'], justify=Tkinter.RIGHT, bg=color)
            bs.pack(side=Tkinter.TOP, fill ='x')
            
            nb = Tkinter.Label(self.frames[1], text=p['nb'], justify=Tkinter.RIGHT, bg=color)
            nb.pack(side=Tkinter.TOP, fill ='x')
            
            nf = Tkinter.Label(self.frames[2], text=p['nb'] - p['nf'], justify=Tkinter.RIGHT, bg=color)
            nf.pack(side=Tkinter.TOP, fill ='x')
            
            np = Tkinter.Label(self.frames[3], text="%d %s" % (int(float(p['nb']-p['nf']) * 100 / float(p['nb'])), '%'), justify=Tkinter.RIGHT, bg=color)
            np.pack(side=Tkinter.TOP, fill ='x')
        
            self.labelsToDestroy += [bs, nb, nf, np]
            i += 1
        self.totalAllocated.config(text="Total Memory Allocated: %d MB" % (allocated / (1024*1024)))
        self.total.config(text="Total Memory Used: %d MB (%d%s)" % (used / (1024*1024), int(float(used)*100/float(allocated)), '%'))
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