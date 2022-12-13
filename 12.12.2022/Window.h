#pragma once

#include "Header.h"
#include "Socket.h"

class Window
{
	Window() = default;
	Window(const Window&) = delete;
	Window& operator= (const Window&) = delete;

	const unsigned DialogID = IDD_DIALOG_WINDOW;
	static BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

	BOOL OnInit(HWND, HWND, LPARAM);
	VOID OnCommand(HWND, int, HWND, UINT);
	VOID OnClose(HWND);

	struct Handls
	{
		HWND hDialog,
			hHost, hConnect,
			hComboIP, hButtIP,
			hChat, hEnter, hSend;
	} _handls;

	Socket* _socket = nullptr;
	HANDLE hOnConnection;

	void ChatHostMod();
	void ChatConnectMod();
	void PressButtIp();

	void CopyAddres();
	void ChatConnect();
	void ChatSendMesg();

	wstring GetComboWstr();
	string GetComboStr();

public:
	static Window& RUN();
	BOOL WinStartup(HWND);

	void ChatMsg(const wstring&) const;

	Socket* const GetSock() const;
	const Handls& GetHWNDs() const;

	void ConnectClose();
};