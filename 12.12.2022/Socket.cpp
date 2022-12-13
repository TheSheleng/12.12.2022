#include "Socket.h"

Socket::Socket(HWND hPar)
{
	if ((_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		MessageBox(hDialog,
			L"Error by socket create.",
			L"Error",
			MB_OK | MB_ICONERROR
		);
	}

	addr.sin_family = AF_INET;

	hDialog = hPar;
}

Socket::~Socket()
{
	closesocket(_socket);
}

const wstring& Socket::_Recv()
{
	CHAR str[COMM_MSG_SIZE];
	if (recv(*_commSock, str, COMM_MSG_SIZE, 0) > 0)
	{
		msgBuff.resize(MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0));
		MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)msgBuff.data(), msgBuff.size());
	}
	else msgBuff = L"";

	return msgBuff;
}

BOOL Socket::_Send(const wstring& wstr)
{
	CHAR str[COMM_MSG_SIZE];
	WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wstr.size(), str, wstr.length(), NULL, NULL);

	if (send(*_commSock, str, COMM_MSG_SIZE, 0) <= 0)
	{
		MessageBox(hDialog,
			L"Error by send msg.",
			L"Error",
			MB_OK | MB_ICONERROR
		);

		return FALSE;
	}
	return TRUE;
}

Host::Host(HWND hPar) : Socket(hPar)
{
	_commSock = &_aSocket;
	Bind();
}

Host::~Host()
{
	closesocket(_aSocket);
}

BOOL Host::Bind()
{
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
	addr.sin_port = PORT;

	if (bind(_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		MessageBox(hDialog,
			L"Error by binding.",
			L"Error",
			MB_OK | MB_ICONERROR
		);
		
		return FALSE;
	}

	return TRUE;
}

BOOL Host::Listen()
{
	if (listen(_socket, 1) == SOCKET_ERROR)
	{
		MessageBox(hDialog,
			L"Error by listening.",
			L"Error",
			MB_OK | MB_ICONERROR
		);

		return FALSE;
	}

	_aSocket = accept(_socket, NULL, NULL);

	while (_aSocket == SOCKET_ERROR)
	{
		_aSocket = accept(_socket, NULL, NULL);
	}

	return TRUE;
}

BOOL Host::SetConnection()
{
	return Listen();
}

Client::Client(HWND hPar, const string& addres) : Socket(hPar)
{
	_commSock = &_socket;
	this->addres = addres;
}

BOOL Client::SetConnection()
{
	inet_pton(AF_INET, addres.c_str(), &addr.sin_addr);
	addr.sin_port = PORT;
	connect(_socket, (SOCKADDR*)&addr, sizeof(addr));

	return TRUE;
}
