
// bound to a hidden Window Edit control to handle text input
// relies on the windows control to deal with the tedious handling of adding, deleting, backspacing, cutting, pasting text and computing the cursor position
class CInputBox 
{

public:


	CInputBox()
	:_bAccountMode(FALSE), _hEdit(0)
	{

	}

	HWND    GetEditWindow()						{ return _hEdit;  }
	void	SetEditWindow(HWND hEdit)			{ _hEdit = hEdit; }
	void	HandleWindowMsg(DWORD dwMsgType, DWORD dwParam1, DWORD dwParam2);
	
	
	void	SetAccountMode(BOOL bAccount);		// sets account entry mode, blocking spaces, commas and so on
	void	SetPasswordMode(BOOL bPassword);	// sets password mode, producing a string of '*'
	void	SetDigitalMode(BOOL bDigitalMode);	// sets numeric mode, accepting only digits
	void	SetMaxNum(int nMaxNum);				// sets the maximum number of characters
	void	SetMultiMode(BOOL bMultiLine);		// sets multi-line mode
	void	SetSel(int nStartChar, int nEndChar);


	void	SetCursorTail();					// moves the cursor to the end of the text
	char*	RefreshText();
	int		RefreshCursor();
	void	ClearText();
	char*	SetText(const char *pszText);
	void	ReplaceSel(const char *pszRplText, BOOL bCanUndo = TRUE);	//replaces the current selection - added by Arcol

protected:

	char	_szText[1024];	// the entered text
	int		_nCursorPos;	// the position of the cursor in the string
	BOOL	_bAccountMode;	// whether it is in account mode
	HWND	_hEdit;
};


inline void CInputBox::SetMaxNum(int nMaxNum)
{
	::SendMessage(_hEdit, EM_LIMITTEXT, nMaxNum, 0);
}

inline char* CInputBox::RefreshText()
{
	GetWindowText(_hEdit, _szText, 1000); 
	return _szText;
}

inline char* CInputBox::SetText(const char *pszText)
{
	SetWindowText(_hEdit, pszText);
	strcpy(_szText, pszText);
	return _szText;
}

inline void CInputBox::ClearText()
{
	strcpy(_szText, "");
	SetWindowText(_hEdit, "");
}

inline int CInputBox::RefreshCursor()
{
	::SendMessage(_hEdit, EM_GETSEL, (WPARAM)&_nCursorPos, 0);
	return _nCursorPos;
}

inline void CInputBox::SetCursorTail()
{
	int nLen = (int)(strlen(_szText));
	::SendMessage(_hEdit, EM_SETSEL, nLen, nLen);
}

inline void CInputBox::SetSel(int nStartChar, int nEndChar)
{
	::SendMessage(_hEdit, EM_SETSEL, nStartChar, nEndChar);
}

inline void CInputBox::HandleWindowMsg(DWORD dwMsgType, DWORD dwParam1, DWORD dwParam2)
{
	// passes the windows keyboard input of the game window on to the edit control
	// if(!(dwMsgType==WM_KEYDOWN || dwMsgType==WM_CHAR)) return;

	// if(dwMsgType==WM_IME_NOTIFY || dwMsgType==WM_IME_STARTCOMPOSITION) return;

	if(dwMsgType==WM_KEYDOWN || dwMsgType==WM_KEYUP)
	{
		if(dwParam1==VK_UP) return; // blocks the UP key so the corresponding operation of the Edit control is not triggered
		// ::SendMessage(_hEdit, WM_KEYDOWN, dwParam1, dwParam2);
	}
	else if(dwMsgType==WM_CHAR)
	{
		if(_bAccountMode)
		{
			if(dwParam1==' ' || dwParam1==',') return;
		}
	}
	::SendMessage(_hEdit, dwMsgType, dwParam1, dwParam2);
}

inline void CInputBox::ReplaceSel(const char *pszRplText, BOOL bCanUndo)
{
	::SendMessage(_hEdit, EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)(LPCTSTR)pszRplText);
}

extern CInputBox g_InputBox;
