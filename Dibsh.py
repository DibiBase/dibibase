#! /usr/bin/python3

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host ="127.0.0.1"
port =9042
s.connect((host,port))
print('connected to Test Cluster at 127.0.0.1:9042')
print('Dibsh 1.0.0 | Dibibase 1.0.0')
def ts(str):
   s.send(r.encode()) 
   data = ''
   data = s.recv(1024).decode()
   print (data)

while 2:
   r = input('Dibsh>')
   if r == "exit":
	 		quit()
   ts(s)
