#pragma once

#include "Header.h"

#define COMM_MSG_SIZE 256

class Socket
{
protected:
	const short PORT = htons(20000);

	SOCKET _socket;
	SOCKET* _commSock;
	sockaddr_in addr;

	HWND hDialog;

	wstring msgBuff;

public:
	Socket(HWND);
	~Socket();

	virtual BOOL SetConnection() = 0;
	const wstring& _Recv();
	BOOL _Send(const wstring&);
};

class Host : public Socket
{
	SOCKET _aSocket;

public:
	Host(HWND);
	~Host();
	BOOL Bind();
	BOOL Listen();

	BOOL SetConnection() override;
};

class Client : public Socket
{
	string addres;
public:
	Client(HWND, const string&);
	BOOL SetConnection() override;
};