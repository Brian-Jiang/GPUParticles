
'''
http://www.howtocreate.co.uk/tutorials/jsexamples/listCollapseExample.html
'''
import os
import fbx
from time import gmtime, strftime

class Logger:
    def __init__(self, cmdLine, logDirParent, scriptFile):
        if not os.path.exists(logDirParent):
            print "Creating %s folder" % logDirParent
            os.mkdir(logDirParent)
        timestamp = strftime("%y_%m_%d_%H_%M_%S", gmtime())
        dirPath = os.path.join(logDirParent, timestamp) 
        if not os.path.exists(dirPath):
            print "Creating %s folder" % dirPath
            os.mkdir(dirPath)
        self.dirPath = dirPath
        self.offset = ""
        self.currentScope = None
        self.StartScope(cmdLine, newFileName='root_log.html', collapse = False)
        self.rootScope = self.currentScope
        self.rootScope['id'] = "rootID"
        self.indent = ""
        self.pathToScript = os.path.relpath(scriptFile, dirPath)
    @staticmethod
    def NewScope(name, parent, collapse, newFileName = ''):
        id = name.replace(" ", "") #generate id to be used by java script to find the menu
        id = id.replace("\n", "")
        id = id.replace("\t", "")
        if not collapse:
            id = id + '-pe-no-collapse'
        d = {'name':name, 'children' : [], 'parent':parent, 'newFileName':newFileName, 'id' : id, 'collapse' : collapse}
        return d
    def StartScope(self, name, doPrint = False, newFileName = '', collapse = True):
        newScope = Logger.NewScope(name, self.currentScope, collapse, newFileName)
        if (doPrint):   
            print name
        if self.currentScope:
            self.currentScope['children'].append(newScope)
        self.currentScope = newScope
    def EndScope(self, msg = "", doPrint = False):
        self.currentScope = self.currentScope['parent'] 
        if msg != "":
            self.AddLine(msg, doPrint)
        
    def AddLine(self, l, doPrint = False):
        l = l.replace("<", "&lt;")
        l = l.replace(">", "&gt;")
        
        self.currentScope['children'].append(l[:])
        if doPrint:
            print l
    def WriteScopeHtml(self, f, scope, uncollapsed):
        makeTopLevelLiTag = False
        makeNewFile = False
        oldFile = f
        oldUncollapsed = uncollapsed
        if scope['newFileName'] != '':
            makeNewFile = True
            uncollapsed = []
            if f:
                f.write('<a href="%s" target="_blank">%s</a><br>\n' % (scope['newFileName'],scope['name']))
            
            f = open(os.path.join(self.dirPath, scope['newFileName']), "w")
            f.write("<html>\n")
            f.write("<body>\n")
        elif scope['parent']:
            makeTopLevelLiTag = True
            f.write("<li>%s\n" % scope['name']) # we need this in all sub scopes except main one
            
        f.write("<ul%s>\n" % ((' id="' + scope['id'] + '"') if scope.has_key('id') else ""))
        if not scope['collapse']:
            uncollapsed.append(scope['id'])
            
        for c in scope['children']:
            if type(c) is str or type(c) is unicode:
                f.write("%s<br>\n" % c)
            else:
                self.WriteScopeHtml(f, c, uncollapsed)
        f.write("</ul>\n")
        if makeTopLevelLiTag:
            f.write("</li>\n")
        
        if makeNewFile:
            f.write('<script type="text/javascript" src="%s"></script>\n' % (self.pathToScript,))
            f.write('<script type="text/javascript">\n')
            f.write('window.onload = function () {\n')
            f.write('//choose one of these options\n')
            f.write('//option 1 - no auto-child-collapse\n')
            f.write("compactMenu('rootID',false,'&plusmn; ');\n")
            f.write('//option 2 - auto-child-collapse\n')
            f.write("//compactMenu('rootID',true,'&plusmn; ');\n")
            f.write("// stateToFromStr('rootID',retrieveCookie('menuState'));\n")
            f.write('}\n')
            f.write('window.onunload = function () {\n')
            f.write("//setCookie('menuState',stateToFromStr('rootID'));\n")
            f.write('}\n')
            f.write("</script>\n")
            
            f.write("</body>\n")
            f.write("</html>\n")
            f.close()
            f = oldFile
        
    def ProduceHtml(self):
        
        self.WriteScopeHtml(None, self.rootScope, [])
    #other utils
    @staticmethod
    def FbxObjectToString(o):
        if type(o) is fbx.FbxDouble3:
            return "fbx.FbxDouble3(%f, %f, %f)" % (o[0], o[1], o[2])
        elif type(o) is fbx.FbxMatrix:
            return "fbx.FbxMatrix(%.5f, %.5f, %.5f, %.5f ,%.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f)" % (
                o.Get(0,0), o.Get(0,1), o.Get(0,2), o.Get(0,3),
                o.Get(1,0), o.Get(1,1), o.Get(1,2), o.Get(1,3),
                o.Get(2,0), o.Get(2,1), o.Get(2,2), o.Get(2,3),
                o.Get(3,0), o.Get(3,1), o.Get(3,2), o.Get(3,3)
            )
        else:
            # str(<fbx.FbxVector4 instance>) already works with fbx sdk, produces output like fbx.FbxVector4(0.000000, 0.000000, 0.000000, 1.000000)
            return str(o)
        