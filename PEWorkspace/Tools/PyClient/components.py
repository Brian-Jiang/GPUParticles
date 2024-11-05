title = 'Pmw.ScrolledListBox demonstration'

# Import Pmw from this directory tree.
import sys
import socket
import select
sys.path[:0] = ['../../..']
import bootstrap

#data components
import dc_handlev4
import dc_v64
import dc_string64

import Tkinter
import Pmw

def HandleAsHexStringInQuotes(h):
    return '"\'%s\' 0x%x 0x%x 0x%x"'%tuple(h)
def HandleAsLuaTableString(h):
    return '{[0]=\'%s\', 0x%x, 0x%x, 0x%x}'%tuple(h)
    
class AdditionalActions:
    def __init__(self, parent, dict, parentName):
        AdditionalActions.Insatnce = self
        self.dict = dict
        self.parent = parent
        self.frame = Pmw.Group(parent, tag_text='Additional Actions')
        self.frame.pack(fill = 'x', expand = 1)
        
        # show native component type
        self.uis = {}
        self.frame.pack(side=Tkinter.TOP)
class SkeletonAdditionalActions(AdditionalActions):
    def __init__(self, parent, dict, parentName):
        AdditionalActions.__init__(self, parent, dict, parentName)
        
        self.uis['ViewSkeletonBtn'] = Tkinter.Button(self.frame.interior(), text="View Skeleton", fg="black", command=self.ViewSkeletonBtn)
        self.uis['ViewSkeletonBtn'].pack(side = Tkinter.TOP, fill='x', expand = 1)
        self.frame.pack(side=Tkinter.TOP)
    def ViewSkeletonBtn(self):
        bootstrap.BootStrap.PyClient.SetSkinViewerPage(self.dict)
class TechniqueAdditionalActions(AdditionalActions):
    def __init__(self, parent, dict, parentName):
        AdditionalActions.__init__(self, parent, dict, parentName)
        
        self.uis['ViewTechniqueBtn'] = Tkinter.Button(self.frame.interior(), text="View Technique", fg="black", command=self.ViewTechniqueBtn)
        self.uis['ViewTechniqueBtn'].pack(side = Tkinter.TOP, fill='x', expand = 1)
        self.frame.pack(side=Tkinter.TOP)
    def ViewTechniqueBtn(self):
        bootstrap.BootStrap.PyClient.SetTechniqueViewerPage(self.dict)
class BasicInfo:
    def __init__(self, parent, dict, parentName):
        self.dict = dict
        self.parent = parent
        self.frame = Pmw.Group(parent, tag_text='Basic Info')
        self.frame.pack(fill = 'x', expand = 1)
        self.name = Pmw.ScrolledField(self.frame.interior(), entry_width = 10,
            entry_relief='groove', labelpos = 'w',
            label_text = '', text = parentName)
        self.name.pack(side = Tkinter.TOP, fill = 'x', expand = 1)
        
        # show Handle
        handleText = ''
        if dict.has_key('handle'):
            handleText = HandleAsHexStringInQuotes(dict['handle'])
        self.messagebar = Pmw.ScrolledField(self.frame.interior(), entry_width = 10,
            entry_relief='groove', labelpos = 'w',
            label_text = 'Handle', text = handleText)
        self.messagebar.pack(side = Tkinter.TOP, fill = 'x', expand = 1)
        
        # show native component type
        self.uis = {}
        
        if dict.has_key('compType'):
            self.uis['nativeComponentType_Label'] =  Tkinter.Label(self.frame.interior(),
                text = 'Native Component Type', anchor = 'w', justify='left', wraplength=SingleTab.TabWidth)
            self.uis['nativeComponentType_Label'].pack(side = Tkinter.TOP, expand=1, fill='x')
            
            nativeComponentType = dict['compType']
            self.uis['nativeComponentType'] = Pmw.ScrolledField(self.frame.interior(), entry_width = 10,
                entry_relief='groove', labelpos = 'w',
                label_text = '', text = nativeComponentType)
            self.uis['nativeComponentType'].pack(side = Tkinter.TOP, fill = 'x', expand = 1)
        
        if dict.has_key('nativeDCType'): 
            self.uis['nativeDataComponentType_Label'] =  Tkinter.Label(self.frame.interior(),
                text = 'Native Data Component Type', anchor = 'w', justify='left', wraplength=SingleTab.TabWidth)
            self.uis['nativeDataComponentType_Label'].pack(side = Tkinter.TOP, expand=1, fill='x')
        
            nativeDataComponentType = dict['nativeDCType']
            self.uis['nativeDataComponentType'] = Pmw.ScrolledField(self.frame.interior(), entry_width = SingleTab.TabWidth / 7,
                entry_relief='groove', labelpos = 'w',
                label_text = '', text = nativeDataComponentType)
            self.uis['nativeDataComponentType'].pack(side = Tkinter.TOP, fill = 'x', expand = 1)
        self.frame.pack(side=Tkinter.TOP)
    def destroy(self):
        self.messagebar.destroy()
    
class Components:
    LastComponents = None
    def __init__(self, parent, dict, parentName):
        Components.LastComponents = self
        self.lists = []
        
        self.name = Pmw.ScrolledField(parent, entry_width = 10,
            entry_relief='groove', labelpos = 'w',
            label_text = '', text = parentName)
        self.name.pack(side = Tkinter.TOP, fill = 'x', expand = 1)
        
        # Create the ScrolledListBox.
        self.box = Pmw.ScrolledListBox(parent,
            items=(),
            labelpos='nw',
            label_text='Components',
            listbox_height = 6,
            selectioncommand=self.selectionCommand,
            dblclickcommand=self.defCmd,
            usehullsize = 1,
            hull_width = SingleTab.TabWidth,
            hull_height = 200,
        )
        self.dict = dict
        self.parent = parent
        

        # Pack this last so that the buttons do not get shrunk when
        # the window is resized.
        self.box.pack(side = Tkinter.TOP, 
        fill = 'both', expand = 1, padx = 5)#, pady = 5)

        
        # Do this after packing the scrolled list box, so that the
        # window does not resize as soon as it appears (because
        # alignlabels has to do an update_idletasks).
    
        #Pmw.alignlabels((hmode, vmode))
        self.components = []
        self.names = []
        
        # Add entries to the listbox.
        if dict.has_key('components'):
            self.components = dict['components']
            i = 0
            for c in self.components:
                self.names.append(str(i) + ':' + c['name'])
                self.box.insert('end', str(i) + ':' + c['name'])
                i += 1
            
    def selectValueByHandle(self, h):
        if isinstance(h, str):
            i = 0
            for c in self.components:
                if HandleAsHexStringInQuotes(c['handle']) == h:
                    self.box.setvalue(self.names[i])
                i += 1
        if isinstance(h, list):
            names = []
            i = 0
            for c in self.components:
                hstr = HandleAsHexStringInQuotes(c['handle'])
                for _h in h:
                    if hstr == _h:
                        names.append(self.names[i])
                i += 1
            self.box.setvalue(names)
        
    def destroy(self):
        self.dict['selected'] = None
        self.box.destroy()
    def selectionCommand(self):
        sels = self.box.getcurselection()
        if len(sels) == 0:
            print 'No selection'
        else:
            index = list(self.box.get(0, 'end')).index(sels[0])
            ComponentViewer.Instance.setSelection(self.dict['level'], index, sels[0], "Component")
    def defCmd(self):
        sels = self.box.getcurselection()
        if len(sels) == 0:
            print 'No selection for double click'
        else:
            print 'Double click:', sels[0]
class DataComponents:
    def __init__(self, parent, dict, parentName):
        self.lists = []
        # Create the ScrolledListBox.
        self.box = Pmw.ScrolledListBox(parent,
            items=(),
            labelpos='nw',
            label_text='Data Components',
            listbox_height = 6,
            selectioncommand=self.selectionCommand,
            dblclickcommand=self.defCmd,
            usehullsize = 1,
            hull_width = SingleTab.TabWidth,
            hull_height = 200,
        )
        self.dict = dict
        self.parent = parent

        # Pack this last so that the buttons do not get shrunk when
        # the window is resized.
        self.box.pack(side = Tkinter.TOP, 
        fill = 'both', expand = 1, padx = 5)#, pady = 5)

        # Do this after packing the scrolled list box, so that the
        # window does not resize as soon as it appears (because
        # alignlabels has to do an update_idletasks).
    
        #Pmw.alignlabels((hmode, vmode))
        
        
        # Add entries to the listbox.
        if dict.has_key('dataComponents'):
            components = dict['dataComponents']
            i = 0
            for c in components:
                self.box.insert('end', str(i) + ':' + c['type'] + ':' + c['DCTypeOverride'])
                i += 1
    def destroy(self):
        self.dict['selected'] = None
        self.box.destroy()
    def selectionCommand(self):
        sels = self.box.getcurselection()
        if len(sels) == 0:
            print 'No selection'
        else:
            index = list(self.box.get(0, 'end')).index(sels[0])
            ComponentViewer.Instance.setSelection(self.dict['level'], index, sels[0], "DataComponent")
    def defCmd(self):
        sels = self.box.getcurselection()
        if len(sels) == 0:
            print 'No selection for double click'
        else:
            print 'Double click:', sels[0]
class HandlerQueues:
    def __init__(self, parent, dict, parentName):
        self.lists = []
        
        # Create the ScrolledListBox.
        self.box = Pmw.ScrolledListBox(parent,
            items=(),
            labelpos='nw',
            label_text='HandlerQueues',
            listbox_height = 6,
            selectioncommand=self.selectionCommand,
            dblclickcommand=self.defCmd,
            usehullsize = 1,
            hull_width = SingleTab.TabWidth,
            hull_height = 200,
        )
        self.dict = dict
        self.parent = parent

        # Pack this last so that the buttons do not get shrunk when
        # the window is resized.
        self.box.pack(side = Tkinter.TOP, 
        fill = 'both', expand = 1, padx = 5)#, pady = 5)

        # Do this after packing the scrolled list box, so that the
        # window does not resize as soon as it appears (because
        # alignlabels has to do an update_idletasks).
    
        #Pmw.alignlabels((hmode, vmode))
        
        
        # Add entries to the listbox.
        if dict.has_key('handlerQueues'):
            components = dict['handlerQueues']
            i = 0
            for c in components:
                self.box.insert('end', str(i) + ':' + c['evtPEUUID'])
                i += 1
    def destroy(self):
        self.dict['selected'] = None
        self.box.destroy()
    def selectionCommand(self):
        sels = self.box.getcurselection()
        if len(sels) == 0:
            print 'No selection'
        else:
            index = list(self.box.get(0, 'end')).index(sels[0])
            ComponentViewer.Instance.setSelection(self.dict['level'], index, sels[0], "HandlerQueue")
    def defCmd(self):
        sels = self.box.getcurselection()
        if len(sels) == 0:
            print 'No selection for double click'
        else:
            print 'Double click:', sels[0]
class Handlers:
    def __init__(self, parent, dict, parentName):
        self.lists = []
        label = Pmw.ScrolledField(parent, entry_width = 10,
            entry_relief='groove', labelpos = 'w',
            label_text = '', text = 'Handlers for %s' % dict['evtPEUUID'])
        label.pack(side = Tkinter.TOP, fill = 'x', expand = 1)
        
        # Create the ScrolledListBox.
        self.box = Pmw.ScrolledListBox(parent,
            items=(),
            labelpos='nw',
            label_text='Handlers for %s' % dict['evtPEUUID'],
            listbox_height = 6,
            selectioncommand=self.selectionCommand,
            dblclickcommand=self.defCmd,
            usehullsize = 1,
            hull_width = SingleTab.TabWidth,
            hull_height = 200,
        )
        self.dict = dict
        self.parent = parent

        # Pack this last so that the buttons do not get shrunk when
        # the window is resized.
        self.box.pack(side = Tkinter.TOP, 
        fill = 'both', expand = 1, padx = 5)#, pady = 5)

        # Do this after packing the scrolled list box, so that the
        # window does not resize as soon as it appears (because
        # alignlabels has to do an update_idletasks).
    
        #Pmw.alignlabels((hmode, vmode))
        handleStrings = []
        for handler in dict['handlers']:
            if isinstance(handler, list):
                self.box.insert('end', HandleAsHexStringInQuotes(handler))
                handleStrings.append(HandleAsHexStringInQuotes(handler))
            elif isinstance(handler, str):
                self.box.insert('end', handler)
        Components.LastComponents.selectValueByHandle(handleStrings)
    def destroy(self):
        self.dict['selected'] = None
        self.box.destroy()
    def selectionCommand(self):
        sels = self.box.getcurselection()
        if len(sels) == 0:
            print 'No selection'
        else:
            index = list(self.box.get(0, 'end')).index(sels[0])
            Components.LastComponents.selectValueByHandle(sels[0])
    def defCmd(self):
        sels = self.box.getcurselection()
        if len(sels) == 0:
            print 'No selection for double click'
        else:
            print 'Double click:', sels[0]

class SingleTab:
    TabWidth = 250
    def __init__(self, parent, dict, parentName):
        self.lists = []
        # Create the ScrolledListBox.
        color = 'red'
        if dict['level'] == 1:
            color = 'green'
        if dict['level'] == 2:
            color = 'blue'
        
        self.frame = Tkinter.Frame(parent)#, bg=color ) # create frame in self.genericFrame
        
        self.dict = dict
        self.parent = parent
        self.components = Components(self.frame, dict, parentName)
        self.dataComponents = DataComponents(self.frame, dict, parentName)
        self.handlerQueues = HandlerQueues(self.frame, dict, parentName)
        self.basicInfo = BasicInfo(self.frame, dict, parentName)
        self.additionalActions = []
        print "Looking for additional actions.."
        compType = dict.get("compType", "")
        print "compType:", compType
        classes = AdditionalActions.Map.get(compType, [])
        classes = classes + AdditionalActions.Map.get(dict.get('DCTypeOverride', ''), [])
        for cls in classes:
            print "Creating instance of:", cls
            self.additionalActions.append(cls(self.frame, dict, parentName))
        
        self.frame.pack(side=Tkinter.LEFT, fill = 'both', expand = 1)
    def destroy(self):
        self.dict['selected'] = None
        self.frame.destroy()
class SingleDataTab:
    DCTypeToDCUIMap = {
        'DC_HANDLEV4' : dc_handlev4.DC_HANDLEV4_UI,
        'DC_V64'      : dc_v64.DC_V64_UI,
        'DC_STRING64' : dc_string64.DC_STRING64_UI,
    }
    def __init__(self, parent, dict, parentName):
        self.lists = []
        
        # Create the ScrolledListBox.
       
        self.frame = Tkinter.Frame(parent)#, bg=color ) # create frame in self.genericFrame
        
        self.dict = dict
        self.parent = parent
        # create the data component modifier
        self.basicInfo = BasicInfo(self.frame, dict, parentName)
        self.dataComponentModifier = None
        if SingleDataTab.DCTypeToDCUIMap.has_key(dict['nativeDCType']):
            cls = SingleDataTab.DCTypeToDCUIMap[dict['nativeDCType']]
            self.dataComponentModifier = cls(self.frame, dict, parentName, SingleTab.TabWidth)
        self.additionalActions = []
        classes = AdditionalActions.Map.get(dict.get('DCTypeOverride', ''), [])
        for cls in classes:
            print "Creating instance of:", cls
            self.additionalActions.append(cls(self.frame, dict, parentName))
        
        self.frame.pack(side=Tkinter.LEFT, fill = 'x', expand = 1)
    def destroy(self):
        self.dict['selected'] = None
        self.frame.destroy()
class SingleHandlerTab:
    
    def __init__(self, parent, dict, parentName):
        self.lists = []
        
        # Create the ScrolledListBox.
       
        self.frame = Tkinter.Frame(parent)#, bg=color ) # create frame in self.genericFrame
        
        self.dict = dict
        self.parent = parent
        
        self.handlersUI = Handlers(self.frame, dict, parentName)
        
        self.frame.pack(side=Tkinter.LEFT, fill = 'x', expand = 1)
    def destroy(self):
        self.dict['selected'] = None
        self.frame.destroy()

class ComponentViewer:
    def __init__(self, parent):
        ComponentViewer.Instance = self
        self.lists = []
        self.parentWidget = parent
        self.deepestDict = None
        self.lastHighlightedDict = None
        self.dict = {}
        self.btnFrame = Tkinter.Frame(parent, bg='blue') # create frame in self.genericFrame
        self.btnFrame.pack(side=Tkinter.LEFT, fill ='x')
        
        #self.boxFrame = Tkinter.Frame(parent, bg='green') # create frame in self.genericFrame
        self.boxFrame = Pmw.ScrolledFrame(parent,
            labelpos = 'n', #label_text = '',
            usehullsize = 1,
            hull_width = 400,
            hull_height = 220,
        )
        self.boxFrame.pack(side=Tkinter.LEFT, fill = 'both', expand = 1)
        
        
        self.syncButton = Tkinter.Button(self.btnFrame, text="Sync", fg="red", command=self.syncButton)
        self.syncButton.pack()
        
        self.debugButton = Tkinter.Button(self.btnFrame, text="Debug", fg="red", command=self.debugButton)
        self.debugButton.pack()
        
        self.highlightVar = Tkinter.IntVar()
        self.highlighCB = Tkinter.Checkbutton(
            self.btnFrame, text="Highlight",
            variable=self.highlightVar,
            command=self.setHighlightCB)
        self.highlighCB.pack(side=Tkinter.LEFT, expand=1, fill='x')
        self.highlighCB.config(state = Tkinter.DISABLED)
        #self.frame.pack(side=Tkinter.TOP, fill = 'x', expand = 1)
    def dehighlightLast(self):
        print 'dehighlightLast'
        if self.lastHighlightedDict != None: # in either case we disable what was highlighted before. 
            print "Sending pop"
            cmd =       "handler = %s\n" % dc_handlev4.handleAsString(self.lastHighlightedDict['handle'])
            cmd = cmd + "evtH = root.PE.Events.Event_POP_SHADERS.Construct()\n"
            cmd = cmd + "root.PE.Components.Component.SendEventToHandle(handler, evtH)\n"
 
            bootstrap.BootStrap.PyClient.sendCommand(cmd)
        self.lastHighlightedDict = None
    def highlightCurrent(self):
        if self.deepestDict != None and self.highlightVar.get() and self.deepestDict.has_key('compType'):
            self.lastHighlightedDict = self.deepestDict
            
            cmd =       "handler = %s\n" % dc_handlev4.handleAsString(self.deepestDict['handle'])
            cmd = cmd + "evtH = root.PE.Events.Event_CHANGE_TO_DEBUG_SHADER.Construct()\n"
            cmd = cmd + "root.PE.Components.Component.SendEventToHandle(handler, evtH)\n"

            bootstrap.BootStrap.PyClient.sendCommand(cmd)
        
    def setHighlightCB(self):
        self.dehighlightLast()
        self.highlightCurrent()
        
    def setList(self, dict):
        # Create the ScrolledListBox.
        self.dict = dict
        curLevel = dict
        self.curDepth = 0
        #todo remove widgets
        self.lists = []
        self.lists.append(SingleTab(self.boxFrame.interior(), dict, dict['name']))
    def setSelection(self, level, index, selectionName, typeStr):
        if len(self.lists) == 0:
            return
        dict = self.dict
        curLevel = 0
        #remove all lists that are deeper than level
        #print '---------- setSelection(level:', level, 'index:', index
        #print 'destorying:', self.lists[level + 1:]
        for l in self.lists[level + 1:]:
            l.destroy()
        self.lists = self.lists[:level+1]
        for t in self.lists:
            if isinstance(t, SingleTab):
                Components.LastComponents = t.components
        #print self.lists
        if len(self.lists) == 0:
            return
        
        dict = self.lists[-1].dict
        dict['selected'] = index
        self.precedingDict = None
        #print dict
        if typeStr == "DataComponent":
            if dict.has_key('dataComponents'):
                if (dict['selected'] != None):
                    dict = dict['dataComponents'][dict['selected']]
                    self.lists.append(SingleDataTab(self.boxFrame.interior(), dict, selectionName))
        elif typeStr == "Component":
            if dict.has_key('components'):
                if (dict['selected'] != None):
                    self.precedingDict = dict
                    dict = dict['components'][dict['selected']]
                    self.lists.append(SingleTab(self.boxFrame.interior(), dict, selectionName))
                    #print 'adding another level', dict
        elif typeStr == "HandlerQueue":
            if dict.has_key('handlerQueues') and len(dict['handlerQueues']) > dict['selected']:
                self.precedingDict = dict
                dict = dict['handlerQueues'][dict['selected']]
                print dict
                self.lists.append(SingleHandlerTab(self.boxFrame.interior(), dict, selectionName))
                #for hqueue in dict['handlerQueues']:
                #    print hqueue
                # if (dict['selected'] != None):
                    # self.precedingDict = dict
                    # dict = dict['components'][dict['selected']]
                    # self.lists.append(SingleTab(self.boxFrame.interior(), dict, selectionName))
            print 'adding handler queue at level', level
        self.deepestDict = dict
        self.dehighlightLast()
        self.highlightCurrent()
    def syncButton(self):
        self.setSelection(-1, 0, "", "")
        self.dehighlightLast()
        self.highlighCB.deselect()
        answ = bootstrap.BootStrap.PyClient.executeCommandWithReturnString(
            'HierarchyBuilder.synchComponentsWithPyClient()')
        dict = eval(answ)
        self.setList(dict[0])
    def debugButton(self):
        if self.deepestDict != None and self.deepestDict.has_key('compType'):
            self.lastHighlightedDict = self.deepestDict
            if self.precedingDict[self.precedingDict['selected']].has_key("components"):
                #selected object is component
                cmd =       "handler = getGameObjectManagerHandle()\n"
                cmd = cmd + "evtH = root.PE.Events.Event_SET_DEBUG_TARGET_HANDLE.Construct(%s, %s)" % (HandleAsLuaTableString(self.precedingDict[self.precedingDict['selected']]['handle']), 55)
                cmd = cmd + "root.PE.Components.Component.SendEventToHandle(handler, evtH)\n"
                bootstrap.BootStrap.PyClient.sendCommand(cmd)
        
    '''
    def showYView(self):
        print self.box.yview()

    def pageDown(self):
        self.box.yview('scroll', 1, 'page')

    def centerPage(self):
        top, bottom = self.box.yview()
        size = bottom - top
        middle = 0.5 - size / 2
        self.box.yview('moveto', middle)
    '''
def InitAdditionalActionMap():
    AdditionalActions.Map = {
        'SkeletonInstance' : [SkeletonAdditionalActions,],
        'Effect' : [TechniqueAdditionalActions,],
    }

######################################################################

# Create demo in root window for testing.
if __name__ == '__main__':
    root = Tkinter.Tk()
    Pmw.initialise(root)
    root.title(title)

    exitButton = Tkinter.Button(root, text = 'Exit', command = root.destroy)
    exitButton.pack(side = 'bottom')
    widget = ComponentViewer(root)
    widget.setList( 
    {
        'name' : 'GameObjectRoot',
        'level' : 0,
        'selected' : None,
        'components' : [
            {
                'level' : 1,
                'selected' : None,
                'name' : 'SceneNode',
                'components' : [
                    {
                        'level' : 2,
                        'selected' : None,
                        'name' : 'Mesh',
                    }
                ],
            },
            {
                'level' : 1,
                'selected' : None,
                'name' : 'SceneNode',
                'components' : [
                    {
                        'level' : 2,
                        'selected' : None,
                        'name' : 'Skin',
                        'components' : [
                            {
                                'level' : 3,
                                'selected' : None,
                                'name' : 'Gun',
                            }
                        ]
                    }
                ],
            },
        ]
    })
    root.mainloop()