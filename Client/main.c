#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib,"Ws3_32.lib")
#include "socketShared.h"
#include "SocketTools.h"
#include "clientFunctions.h"



SOCKET m_socket;
char* client_Messages[] = { "CLIENT_REQUEST","CLIENT_VERSUS","CLIENT_SETUP","CLIENT_PLAYER_MOVE","CLIENT_DISCONNECT"};
char* server_Messages[] = { "SERVER_MAIN_MENU","SERVER_APPROVED","SERVER_DENIED","SERVER_INVITE","SERVER_SETUP_REQUEST","SERVER_PLAYER_MOVE_REQUEST",
"SERVER_GAME_RESULTS","SERVER_WIN","SERVER_DRAW","SERVER_NO_OPPONENTS","SERVER_OPPONENT_QUIT"};
//Reading data coming from the server
static DWORD RecvDataThread(void)
{
	TransferResult_t RecvRes;

	while (1)
	{
		char* AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, m_socket);

		if (RecvRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("Server closed connection. Bye!\n");
			return 0x555;
		}
		else
		{
			printf("%s\n", AcceptedStr);
		}

		free(AcceptedStr);
	}

	return 0;
}

//Sending data to the server
static DWORD SendDataThread(void)
{
	char SendStr[256];
	TransferResult_t SendRes;

	while (1)
	{
		gets_s(SendStr, sizeof(SendStr)); //Reading a string from the keyboard

		if (STRINGS_ARE_EQUAL(SendStr, "quit"))
			return 0x555; //"quit" signals an exit from the client side

		SendRes = SendString(SendStr, m_socket);

		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			return 0x555;
		}
	}
}
void mainClient() {
	SOCKADDR_IN clientService;
	HANDLE hThread[3];
	DWORD* p_thread_ids[3];
	int result;
	int ret_val;
	// Initialize Winsock.
	WSADATA wsaData; //Create a WSADATA object called wsaData.
	//The WSADATA structure contains information about the Windows Sockets implementation.

	//Call WSAStartup and check for errors.
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed: %d\n", result);
		return 1;
	}
	//Call the socket function and return its value to the m_socket variable. 
	// For this application, use the Internet address family, streaming sockets, and the TCP/IP protocol.

	// Create a socket.
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// Check for errors to ensure that the socket is a valid socket.
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	

	//For a client to communicate on a network, it must connect to a server.
	// Connect to a server.

	//Create a sockaddr_in object clientService and set  values.
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(SERVER_ADDRESS_STR); //Setting the IP address to connect to
	clientService.sin_port = htons(SERVER_PORT); //Setting the port to connect to.

	 // Call the connect function, passing the created socket and the sockaddr_in structure as parameters.
	// Check for general errors.
	
	ret_val = connect_to_server(m_socket, &clientService, sizeof(clientService));
	if (ret_val == SOCKET_ERROR) {
		WSACleanup();
		return;	
	}
	printf("Connected to server on %s:%d. \n", SERVER_ADDRESS_STR, SERVER_PORT);

	//need to wrap in a function:
	//int createThreadsForClient(HANDLE * hTread, DWORD * p_thread_ids) {

		hThread[0] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)SendDataThread,
			NULL,
			0,
			NULL
		);
		if (hThread[0] == NULL)
		{
			printf("failed create send thread %d", GetLastError());
			//return GetLastError();
			WSACleanup();
			return;
		}

		hThread[1] = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)RecvDataThread,
			NULL,
			0,
			NULL
		);
		if (hThread[1] == NULL)
		{
			printf("failed create recieve thread %d", GetLastError());
			//return GetLastError();
			WSACleanup();
			return;
		}
		//hThread[2] = CreateThread(
		//	NULL,
		//	0,
		//	user_interface,
		//	inputmode,
		//	0,
		//	NULL
		//);

		//if (hThread[2] == NULL)
		//{
		//	printf("cannot open user interface Thread.\n");
		//	WSACleanup();
		//	return;
		//}
	
	WaitForMultipleObjects(3, hThread, FALSE, INFINITE);
	////when we finish the app. finished the work with all the threads
	result = WSAcleanup();
	if (result != 0) {
		printf("WSAStartup failed: %d\n", result);
		return -1;
	}
	TerminateThread(hThread[0], 0x555);
	TerminateThread(hThread[1], 0x555);

	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	//CloseHandle(hThread[2]);

	closesocket(m_socket);

	WSACleanup();

	return;

}