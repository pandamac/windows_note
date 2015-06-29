if __name__ == '__main__':
	import socket
	sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	sock.bind(('localhost',8001))
	sock.listen(5)
	while True:
		connection,address = sock.accept()
		print 'server connected by',address
		while True:
			data = connection.recv(1024)
			if not data: break  

			connection.send('Echo=>'+data)
	connection.close()