import socket
import random
import time
import SocketServer
import os
import subprocess

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


class MyTCPHandler(SocketServer.BaseRequestHandler):
    """
    The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):
        # self.request is the TCP socket connected to the client
        
        #PyEngine sends info in lines. First line si alwasy number of lines sent (excluding first line). Usually it is 1
        answ = ''
        while 1:
            answ = answ + self.request.recv(1024*1024)
            print answ
            lines = answ.splitlines()
            numLinesNeeded = int(lines[0]) - 1
            have = len(lines) - 1
            print 'input received; %d bytes' % len(answ)
            print 'Lines needed', numLinesNeeded, 'have', have
            if have >= numLinesNeeded and answ.endswith('\n'):
                break
            
        print 'Input:', answ
        
        # we expect a single line dictionary
        if len(lines) < 2:
            print "Received less than 2 lines. This is error because first line is alwas the #lines following"
        else:
            d = eval(lines[1])
            print 'file to create/overwrite:', d['filename']
            
            print 'PyEngine2.0 Technique Name associated with this SPE Shader:', d['techName']
            
            print 'Text:'
            print d['text']
            
        #creating file..
        f = open(d['filename'], 'w')
        f.write(d['text'])
        f.close()
        
        # compile SPE program
        #subprocess.Popen("c++ -o %s" % (d['filename'],))
        
        #launch main program
        # lookup how to provide cmd line argument at http://docs.python.org/library/subprocess.html#module-subprocess
        #subprocess.Popen
        
        # send reply when done compiling and process is launched. send: "DataComponentProtocols.l_setTechniqueReady(%s)" % d['techName']
        # send it with pyengine_send_cmd, but you will need to change ip. probably retrieve it here..
        

if __name__ == "__main__":
    HOST, PORT = "localhost", 9999

    # Create the server, binding to localhost on port 9999
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    server.serve_forever()
