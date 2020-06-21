#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	//Initialize winsock
	WSADATA wsaDATA;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsaDATA);
	if (wsok != 0) {
		cerr << "Can't Initialize winsock";
		return;
	}

	//Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Can't create a socket!";
	}

	// Bind a socket to an ip and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening,(sockaddr*)&hint, sizeof(hint));
	
	// Tell winsock the socket is listening
	listen(listening, SOMAXCONN);

	//Wait for connection
	sockaddr_in client;
	int client_size = sizeof(client);

	SOCKET clientsocket = accept(listening, (sockaddr*)&client,&client_size);

	char host[NI_MAXHOST];	   //Remote Name
	char service[NI_MAXHOST];  // Service Port

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client),host , NI_MAXHOST, service, NI_MAXSERV, 0) == 0){
		cout << host << " connected on port" << service<< endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port" << ntohs(client.sin_port);
	}

	//	Close listening socket
	closesocket(listening);

	//	While loop: Interact with the client
	char buff[4096];
	while (true) {
		ZeroMemory(buff, 4096);
		
		// Wait for Client data
		int bytesRecieved = recv(clientsocket, buff, 4096, 0);
		if (bytesRecieved == SOCKET_ERROR) {
			cerr << "Error in Recv()... Quiting"<<endl;
			break;
		}
		if (bytesRecieved == 0) {
			cout << "Client Disconnected"<< endl;
			break;
		}

		// Echo message back to client
		send(clientsocket, "From server", bytesRecieved + 1, 0);
		send(clientsocket, buff , bytesRecieved + 1, 0);

	}
	//	Close a socket
	closesocket(clientsocket);
	//	Cleanup wimsock
	WSACleanup();
}
