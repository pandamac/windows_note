#!/usr/bin/env python
#  -*- coding: utf-8 -*-
'''
server:
import socket #Server
import threading
import time
def tcplink(sock,addr):
    print 'Accespt new connection from %s:%s' % addr
    sock.send('Welcome!')
    while True:
        data = sock.recv(1024)
        time.sleep(1)
        if data == 'exit' or not data:
            break
        sock.send('Hello,%s' % data)
    sock.close()
    print 'Connection from %s:%s closed' % addr

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind(('127.0.0.1',9999))
s.listen(5)
print 'Waiting for connection'
while True:
    sock, addr = s.accept()
    t = threading.Thread(target=tcplink,args=(sock,addr))
    t.start()

import socket

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

s.connect(('data200.alictf.com',30000))

print s.recv(1024)
for data in ['panda','panda2','panda3']:
    s.send(data)
    print s.recv(1024)
s.send('exit')
s.close()
'''
