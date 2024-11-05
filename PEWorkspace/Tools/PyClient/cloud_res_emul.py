import socket
import random
import time
import SocketServer
import os
import subprocess

def send_sample_data():
    '''
    This function prepares the command that is sent to pyengine server
    the generated function argument is a Lua table.
    Lua table syntax: t = {name = val, [index] = val2, [index2] = val3, [more values]}
    the generated text is executed by pyengine, so it looks like:
    setps3ParticleSystem({size=<size>, ...})
    '''
    text = 'DataComponentProtocols.setSpeShaderData({ size=1200, techName="SpeEffect_Tech",'
    for ip in xrange(1200):
        text = text + '%f,%f,%f,' % \
            (random.random() * 1000, random.random() * 1000 + 1000, random.random() * 1000)
    text = text + '})\n'
    #text = 'DataComponentProtocols.setps3ParticleSystem({ size=100})\n'

    pyengine_send_cmd(text)
    
    
def pyengine_send_cmd(text):
    #print text
    try:
        s = socket.socket()
        s.connect(('localhost', 8888))
        lines = text.splitlines()
        n = len(lines)
        s.send(str(n) + '\n')
        s.send(text)
        s.close()
    except:
        pass

if __name__ == '__main__':
    print "Starting PS3 Emulation.."
    while 1:
        send_sample_data()
        time.sleep(0.03)