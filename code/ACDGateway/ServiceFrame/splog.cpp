#include <splog.hpp>




void SysLogSP::OutputWin(std::string & o)
{
	if ( m_hWnd == NULL )
		return;
	
	int len = o.length();
	int max = 128000;
	while ( GetWindowTextLength(m_hWnd) + len >= max )
	{
		SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);


		DWORD								start, finish;
		
		
		SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&finish);
		SendMessage(m_hWnd,
					EM_SETSEL,
					0,
					SendMessage(m_hWnd, EM_LINEINDEX, 1, 0));
		SendMessage(m_hWnd, EM_REPLACESEL, FALSE, (DWORD)"");
		SendMessage(m_hWnd, EM_SETSEL, start, finish);
		SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);
	}

	SendMessage(m_hWnd, EM_SETSEL, max, max);


	strcpy(m_buf, o.c_str());
	/*char *								lf;
	const char *						prev = m_buf;

	
	while ( ( lf = strchr(prev, '\n') ) != NULL )
	{
		if ( *(lf - 1) == '\r' )
			prev = lf+1;
		else
		{
			*lf++ = '\0';*/
			SendMessage(m_hWnd, EM_REPLACESEL, FALSE, (DWORD)m_buf);
			SendMessage(m_hWnd, EM_REPLACESEL, FALSE, (DWORD)"\r\n");
			/*break;
		}
	}*/
}




IFINE_SYSLOG * CreateLogSP(HWND p_hwnd)
{
	return new SysLogSP(p_hwnd);
}