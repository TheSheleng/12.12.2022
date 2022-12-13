#include "Header.h"
#include "Window.h"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		MessageBox(NULL,
			L"Error by WSAStartup.",
			L"Error",
			MB_OK | MB_ICONERROR
		);
	}

	return Window::RUN().WinStartup(NULL);

	WSACleanup();
}