#include <windows.h>  //include all the basics
#include <tchar.h>    //string and other mapping macros

#if defined __MINGW_H
#define _WIN32_IE 0x0400
#endif
#include <commctrl.h>

#include <string>
#include <vector>

//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//message processing function declarations
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
int OnCreate(const HWND,CREATESTRUCT*);
void OnDestroy(const HWND);
void OnSize(const HWND,int,int,UINT);

//non-message function declarations
HWND CreateTab(const HWND,const HINSTANCE,DWORD,const RECT&,const int);
inline int ErrMsg(const ustring&);
int InsertItem(HWND,const ustring&,int,int image_index=-1, UINT mask=TCIF_TEXT|TCIF_IMAGE);
bool SetTabCntrlImagelist(const HWND);                     
void StartCommonControls(DWORD);

//setup some control id's
enum {
  IDC_TAB=200,
};
#define ID_TABCTRL 1
#define ID_EDIT 2
#define BTN_ADD 3
#define BTN_DEL 4
#define BTN_CLR 5
#define MAX_TAB_LEN 15
HWND hTabCntrl, hEdit;
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR pStr,int nCmd)
{
	ustring classname=_T("SIMPLEWND");
	WNDCLASSEX wcx={0};  //used for storing information about the wnd 'class'

	wcx.cbSize         = sizeof(WNDCLASSEX);           
	wcx.lpfnWndProc    = WndProc;             //wnd Procedure pointer
	wcx.hInstance      = hInst;               //app instance
	//use 'LoadImage' to load wnd class icon and cursor as it supersedes the 
	//obsolete functions 'LoadIcon' and 'LoadCursor', although these functions will 
	//still work. Because the icon and cursor are loaded from system resources ie 
	//they are shared, it is not necessary to free the image resources with either 
	//'DestroyIcon' or 'DestroyCursor'.
	wcx.hIcon         = reinterpret_cast<HICON>(LoadImage(0,IDI_APPLICATION,
												IMAGE_ICON,0,0,LR_SHARED));
	wcx.hCursor       = reinterpret_cast<HCURSOR>(LoadImage(0,IDC_ARROW,
												  IMAGE_CURSOR,0,0,LR_SHARED));
	wcx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE+1);   
	wcx.lpszClassName = classname.c_str(); 
	//the window 'class' (not c++ class) has to be registered with the system
	//before windows of that 'class' can be created
	if (!RegisterClassEx(&wcx))
	{
		ErrMsg(_T("Failed to register wnd class"));
		return -1;
	}

	int desktopwidth=GetSystemMetrics(SM_CXSCREEN);
	int desktopheight=GetSystemMetrics(SM_CYSCREEN);

	HWND hwnd=CreateWindowEx(0, classname.c_str(), _T("Common Controls - Tab"), WS_OVERLAPPEDWINDOW, 
		desktopwidth/4, desktopheight/4, desktopwidth/2, desktopheight/2, 0, 0,  hInst, 0); 
	if (!hwnd)
	{
		ErrMsg(_T("Failed to create wnd"));
		return -1;
	}

	ShowWindow(hwnd,nCmd); 
	UpdateWindow(hwnd);
	//start message loop - windows applications are 'event driven' waiting on user,
	//application or system signals to determine what action, if any, to take. Note 
	//that an error may cause GetMessage to return a negative value so, ideally,  
	//this result should be tested for and appropriate action taken to deal with 
	//it(the approach taken here is to simply quit the application).
	MSG msg;
	while (GetMessage(&msg,0,0,0)>0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}
//=============================================================================
LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	TCITEMW tie;
    wchar_t text[4];
    LRESULT count, id;
    
	switch (uMsg)
	{
		case WM_CREATE:
		{
			return OnCreate(hwnd,reinterpret_cast<CREATESTRUCT*>(lParam));
		}
		case WM_COMMAND:

            switch(LOWORD(wParam)) {

                case BTN_ADD:

                    GetWindowTextW(hEdit, text, 250);

                    if (lstrlenW(text) != 0 ) {

                        tie.mask = TCIF_TEXT;
                        tie.pszText = text;
                        count = SendMessageW(hTabCntrl, TCM_GETITEMCOUNT, 0, 0);
                        SendMessageW(hTabCntrl, TCM_INSERTITEMW, count, 
                            (LPARAM) (LPTCITEM) &tie);
                    }
                    break;

                case BTN_DEL:
                
                    id = SendMessageW(hTabCntrl, TCM_GETCURSEL, 0, 0);
                    
                    if (id != -1) {
                    
                       SendMessageW(hTabCntrl, TCM_DELETEITEM, 0, id);
                    }
                    break;

                case BTN_CLR:
               
                    SendMessageW(hTabCntrl, TCM_DELETEALLITEMS, 0, 0);
                    break;
            } 
            break;

		case WM_DESTROY:
		{
			OnDestroy(hwnd);
			return 0;
		}
		case WM_SIZE:
		{
			OnSize(hwnd,LOWORD(lParam),HIWORD(lParam),static_cast<UINT>(wParam));
			return 0;
		}
		default:
		//let system deal with msg
		return DefWindowProc(hwnd,uMsg,wParam,lParam);  
	}
}
//=============================================================================
int OnCreate(const HWND hwnd,CREATESTRUCT *cs)
{
	//handles the WM_CREATE message of the main, parent window; return -1 to fail
	//window creation
	RECT rc={0,0,0,0};  //set dimensions in parent window's WM_SIZE handler 
	StartCommonControls(ICC_TAB_CLASSES);
	
	hTabCntrl=CreateTab(hwnd,cs->hInstance,TCS_FIXEDWIDTH,rc,IDC_TAB);

	//now store the tab control handle as the user data associated with the
	//parent window so that it can be retrieved for later use. This will emit a 
	//C4244 warning if /wp64 is enabled with ms compilers under win32 due to how 
	//SetWindowLongPtr is typedef'd for 32bit and 64bit compatibility. The warning
	//in this context can be safely ignored. Despite this being identified as a 
	//glitch under msvc.net 2003, it still exists in the later msvc express 2005. 
	//A workaround would be to wrap the offending call in #pragma warning 
	//directives, or to typedef the fn properly for 32/64 bit compatibility. 
	//See http://msdn.microsoft.com/msdnmag/issues/01/08/bugslayer/
	//for details.
	SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(hTabCntrl));

	//setup and attach some tab pages
	SetTabCntrlImagelist(hTabCntrl);

	InsertItem(hTabCntrl,_T("First Page"),0,0);
	InsertItem(hTabCntrl,_T("Second Page"),1,1);
	InsertItem(hTabCntrl,_T("Third Page"),2,2);
	InsertItem(hTabCntrl,_T("Fourth Page"),3,3);
	InsertItem(hTabCntrl,_T("Fifth Page"),4,4);

	//set the font of the tabs to a more typical system GUI font
	SendMessage(hTabCntrl,WM_SETFONT,
				reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)),0);
	
	//hTabCntrl = CreateWindowW(WC_TABCONTROLW, NULL, WS_CHILD | WS_VISIBLE,
      //          0, 0, 200, 150, hwnd,(HMENU) ID_TABCTRL, NULL, NULL);

    hEdit = CreateWindowW(WC_EDITW, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        250, 20, 100, 25, hwnd, (HMENU) ID_EDIT, NULL, NULL);

    SendMessage(hEdit, EM_SETLIMITTEXT, MAX_TAB_LEN, 0);

    CreateWindowW(WC_BUTTONW, L"Add", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        250, 50, 100, 25, hwnd, (HMENU) BTN_ADD, NULL, NULL);

    CreateWindowW(WC_BUTTONW, L"Delete", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        250, 80, 100, 25, hwnd, (HMENU) BTN_DEL, NULL, NULL);

    CreateWindowW(WC_BUTTONW, L"Clear", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        250, 110, 100, 25, hwnd, (HMENU) BTN_CLR, NULL, NULL);				   
	return 0;
}
//=============================================================================
void OnDestroy(const HWND hwnd)
{
	//get the tab control handle which has been previously stored in the user
	//data associated with the parent window.
	HWND hTabCntrl=reinterpret_cast<HWND>(static_cast<LONG_PTR>
									   (GetWindowLongPtr(hwnd,GWLP_USERDATA)));

	HIMAGELIST hImages=reinterpret_cast<HIMAGELIST>(SendMessage(hTabCntrl,
													TCM_GETIMAGELIST,0,0)); 

	ImageList_Destroy(hImages);

	PostQuitMessage(0); //signal end of application                           
}
//=============================================================================
void OnSize(const HWND hwnd,int cx,int cy,UINT flags)
{
	//get the header control handle which has been previously stored in the user
	//data associated with the parent window.
	HWND hTabCntrl=reinterpret_cast<HWND>(static_cast<LONG_PTR>
									   (GetWindowLongPtr(hwnd,GWLP_USERDATA)));
	//resize tab common control so that it always fills parent's client area
	MoveWindow(hTabCntrl,2,2,cx-4,cy-4,TRUE);
}
//=============================================================================
HWND CreateTab(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle,
               const RECT& rc,const int id)
{
	dwStyle|=WS_CHILD|WS_VISIBLE;
	return CreateWindowEx(0, WC_TABCONTROL, 0, dwStyle, rc.left, rc.top, rc.right, rc.bottom,  hParent, 
		reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), hInst, 0); 
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
	return MessageBox(0,s.c_str(),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
}
//=============================================================================
void StartCommonControls(DWORD flags)
{
	//load the common controls dll, specifying the type of control(s) required 
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize=sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC=flags;
	InitCommonControlsEx(&iccx);
}
//=============================================================================
int InsertItem(HWND hTc,const ustring& txt,int item_index,int image_index,
               UINT mask)
{
	//insert a tab page. 'mask' defaults to TCIF_TEXT|TCIF_IMAGE. image_index has
	//a default value of -1 (no image);

	//copy the text into a temporary array (vector) so it's in a suitable form
	//for the pszText member of the TCITEM struct to use. This avoids using
	//const_cast on 'txt.c_str()' or variations applied directly to the string that
	//break its constant nature.
	std::vector<TCHAR> tmp(txt.begin(),txt.end());
	tmp.push_back(_T('\0'));

	TCITEM tabPage={0};

	tabPage.mask=mask;
	tabPage.pszText=&tmp[0];
	tabPage.cchTextMax=static_cast<int>(txt.length());
	tabPage.iImage=image_index;
	return static_cast<int>(SendMessage(hTc,TCM_INSERTITEM,item_index,
							reinterpret_cast<LPARAM>(&tabPage)));
}
//=============================================================================
bool SetTabCntrlImagelist(const HWND hTc)
{
	//set up and attach image lists to tab common control

	//create and fill the image list
	HIMAGELIST hImages=ImageList_Create(GetSystemMetrics(SM_CXSMICON),
										GetSystemMetrics(SM_CYSMICON),
										ILC_COLOR32|ILC_MASK,1,1);
	if (hImages==0)
	{
		ErrMsg(_T("Failed to create image list."));
		return false;
	}
	  
	//get an instance handle for a source of icon images - for convenience use a
	//known system dll
	HINSTANCE hLib=LoadLibrary(_T("shell32.dll"));

	int icon_index[]={23,41,43,44,47}; 
	int i;
	HICON hIcon;

	for (i=0;i<5;++i)
	{
		//Because the icons are loaded from system resources  ie they are shared, 
		//it is not necessary to free the image resources with 'DestroyIcon'.
		hIcon=reinterpret_cast<HICON>(LoadImage(hLib,
								   MAKEINTRESOURCE(icon_index[i]),
								   IMAGE_ICON,
								   0,0,
								   LR_SHARED));
		ImageList_AddIcon(hImages,hIcon); 
	}

	FreeLibrary(hLib);
					 
	//finally attach image list to tab control
	TabCtrl_SetImageList(hTc,hImages);   

	return true;               
}