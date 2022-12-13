#include "Window.h"

BOOL Window::DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_INITDIALOG, RUN().OnInit);
		HANDLE_MSG(hWnd, WM_COMMAND, RUN().OnCommand);
		HANDLE_MSG(hWnd, WM_CLOSE, RUN().OnClose);
	default: return FALSE;
	}
}

BOOL Window::OnInit(HWND hWnd, HWND focus, LPARAM lParam)
{
	_handls.hDialog = hWnd;

	_handls.hHost = GetDlgItem(hWnd, IDC_BUTT_HOST);
	_handls.hConnect = GetDlgItem(hWnd, IDC_BUTT_CONNECT);

	_handls.hComboIP = GetDlgItem(hWnd, IDC_COMBO_IP);
	_handls.hButtIP = GetDlgItem(hWnd, IDC_BUTT_IP);

	_handls.hChat = GetDlgItem(hWnd, IDC_LIST_MSG);
	_handls.hEnter = GetDlgItem(hWnd, IDC_EDIT_MSG);
	_handls.hSend = GetDlgItem(hWnd, IDC_BUTT_ENTER);

	wstring wstr;

	struct hostent* phe = gethostbyname("");

	struct in_addr addr;
	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));

		wstr = to_wstring(addr.S_un.S_un_b.s_b1);
		wstr += L'.' + to_wstring(addr.S_un.S_un_b.s_b2);
		wstr += L'.' + to_wstring(addr.S_un.S_un_b.s_b3);
		wstr += L'.' + to_wstring(addr.S_un.S_un_b.s_b4);

		SendMessage(_handls.hComboIP, CB_ADDSTRING, 0, (LPARAM)wstr.c_str());
		SendMessage(_handls.hComboIP, CB_SETCURSEL, 0, 0);
	}

	return TRUE;
}

VOID Window::OnCommand(HWND hWnd, int id, HWND Ctl, UINT Notify)
{	
	switch (id)
	{
	case IDC_BUTT_HOST: ChatHostMod(); break;
	case IDC_BUTT_CONNECT: ChatConnectMod(); break;
	case IDC_BUTT_IP: PressButtIp(); break;
	case IDC_BUTT_ENTER: ChatSendMesg(); break;
	}
}

VOID Window::OnClose(HWND hWnd)
{
	if (_socket != nullptr) ConnectClose();
	EndDialog(hWnd, FALSE);
}

DWORD WINAPI OnConnect(LPVOID _sock)
{
	if (Window::RUN().GetSock()->SetConnection())
	{
		Window::RUN().ChatMsg(L"Pen friend connect!");

		SendMessage(Window::RUN().GetHWNDs().hEnter, EM_SETREADONLY, FALSE, 0);
		EnableWindow(Window::RUN().GetHWNDs().hSend, TRUE);

		const wstring rName = L"Pen friend: ";
		wstring msgBuff;
		while (true)
		{
			msgBuff = rName + Window::RUN().GetSock()->_Recv();
			if (msgBuff == rName) break;
			Window::RUN().ChatMsg(msgBuff);
		}

		Window::RUN().ChatMsg(L"Pen friend disconnected");
		Window::RUN().ConnectClose();
	}

	return 0;
}

void Window::ChatHostMod()
{
	if (_socket != nullptr)
	{
		ConnectClose();
		ChatMsg(L"Disconnecting from the server");
	}

	_socket = new Host(_handls.hDialog);

	EnableWindow(_handls.hHost, FALSE);
	EnableWindow(_handls.hConnect, TRUE);

	EnableWindow(_handls.hButtIP, TRUE);
	SetWindowText(_handls.hButtIP, L"Copy");

	ChatMsg(L"Server is start!");
	ChatMsg(L"Waiting a pen friend...");

	hOnConnection = CreateThread(NULL, 0, OnConnect, &RUN(), 0, NULL);
}

void Window::ChatConnectMod()
{
	if (_socket != nullptr)
	{
		ConnectClose();
		ChatMsg(L"Server closed.");
	}

	EnableWindow(_handls.hHost, TRUE);
	EnableWindow(_handls.hConnect, FALSE);

	EnableWindow(_handls.hButtIP, TRUE);
	SetWindowText(_handls.hButtIP, L"Connect");

	ChatMsg(L"Specify the connection address...");
}

void Window::PressButtIp()
{
	if (IsWindowEnabled(_handls.hHost)) ChatConnect();
	else CopyAddres();
}

void Window::ChatMsg(const wstring& wstr) const
{
	SendMessage(_handls.hChat, LB_ADDSTRING, 0, (LPARAM)wstr.c_str());
	SendMessage(_handls.hChat, LB_SETCURSEL, SendMessage(_handls.hChat, LB_GETCOUNT, 0, 0) - 1, 0);
}

void Window::CopyAddres()
{
	if (!OpenClipboard(NULL)) return;
	EmptyClipboard();

	string addr = GetComboStr();

	HGLOBAL hglb;
	if ((hglb = GlobalAlloc(GMEM_MOVEABLE, addr.size())) == NULL)
	{
		CloseClipboard();
		return;
	}

	char* pBuf = (char*)GlobalLock(hglb);
	if (pBuf != nullptr) strcpy_s(pBuf, addr.size(), addr.c_str());
	GlobalUnlock(hglb);

	SetClipboardData(CF_OEMTEXT, hglb);
	CloseClipboard();
}

void Window::ChatConnect()
{
	string addr = GetComboStr();
	_socket = new Client(_handls.hDialog, addr);

	ChatMsg(L"Server search...");
	hOnConnection = CreateThread(NULL, 0, OnConnect, &RUN(), 0, NULL);

	SendMessage(_handls.hEnter, EM_SETREADONLY, FALSE, 0);
	EnableWindow(_handls.hButtIP, FALSE);
	EnableWindow(_handls.hSend, TRUE);
}

void Window::ChatSendMesg()
{
	wstring wstr;
	wstr.resize(GetWindowTextLength(_handls.hEnter) + 1);
	GetWindowText(_handls.hEnter, (LPWSTR)wstr.data(), wstr.size());

	if (wstr != L"")
	{
		const wstring sName = L"You: ";
		if (_socket->_Send(wstr))
		{
			ChatMsg(sName + wstr);
			SetWindowText(_handls.hEnter, L"");
		}
	}
}

wstring Window::GetComboWstr()
{
	wstring wstr;
	wstr.resize(SendMessage(_handls.hComboIP, CB_GETLBTEXTLEN, SendMessage(_handls.hComboIP, CB_GETCURSEL, 0, 0), 0) + 1);
	GetWindowText(_handls.hComboIP, (LPWSTR)wstr.data(), wstr.size());

	return wstr;
}

string Window::GetComboStr()
{
	wstring wstr = GetComboWstr();

	string str;
	str.resize(wstr.size());
	WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wstr.size(), (LPSTR)str.data(), wstr.length(), NULL, NULL);

	return str;
}

Window& Window::RUN()
{
	static Window win;
	return win;
}

BOOL Window::WinStartup(HWND hPar)
{
	return DialogBox(
		NULL,
		MAKEINTRESOURCE(Window::DialogID),
		hPar,
		(DLGPROC)Window::DlgProc
	);
}

 Socket* const Window::GetSock() const
{
	return _socket;
}

 const Window::Handls& Window::GetHWNDs() const
 {
	 return _handls;
 }

 void Window::ConnectClose()
 {
	 CloseHandle(hOnConnection);
	 delete _socket;
	 _socket = nullptr;

	 EnableWindow(_handls.hHost, TRUE);
	 EnableWindow(_handls.hConnect, TRUE);

	 EnableWindow(_handls.hButtIP, FALSE);
	 SetWindowText(_handls.hButtIP, L"...");
	 SendMessage(Window::RUN().GetHWNDs().hEnter, EM_SETREADONLY, TRUE, 0);
	 EnableWindow(Window::RUN().GetHWNDs().hSend, FALSE);
 }
