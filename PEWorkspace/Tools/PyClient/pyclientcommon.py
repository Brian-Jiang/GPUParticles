# setup paths for pyengine modules
import sys, os
sys.path.append(os.environ['PYENGINE_WORKSPACE_DIR'] + '\\Tools')

import socket
import select

def sendCommandS(text, IPAndPort):
    s = socket.socket()
    tryConnect = True
    connected = False
    while tryConnect:
        try:
            s.connect(IPAndPort)
            connected = True
            tryConnect = False
        except:
            return False
    if connected:
        if not text.endswith('\n'): text = text + '\n'
        lines = text.splitlines()
        n = len(lines)
        s.send(str(n) + '\n')
        s.send(text)
        s.close()
        return True
def getTargetSystemList():
    conf = {}
    execfile(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], 'GlobalConfig', 'Dirs.py'), conf)
    return conf['TargetSystems']
def selectTarget(target, customEntry):
    if 'custom' in target.lower():
        target = customEntry
    if 'localhost'.lower() in target.lower():
        return ('localhost', int(target[target.rfind(':')+1:]))
    elif 'XBox 360'.lower() in target.lower():
        return (target[target.rfind('(')+1 : target.rfind(')')], int(target[target.rfind(':')+1:]))
    elif 'PS3'.lower() in target.lower():
        return (target[target.rfind('(')+1 : target.rfind(')')], int(target[target.rfind(':')+1:]))
    elif 'IPad'.lower() in target.lower():
        return (target[target.rfind('(')+1 : target.rfind(')')], int(target[target.rfind(':')+1:]))
    elif '(' in target and ')' in target:
        return (target[target.rfind('(')+1 : target.rfind(')')], int(target[target.rfind(':')+1:]))
    else:
        print "Error: unknown target provided. setting to localhost:1417"
        return ('localhost', 1417)
    
