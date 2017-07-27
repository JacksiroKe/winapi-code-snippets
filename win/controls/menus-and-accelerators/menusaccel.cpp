#include <windows.h>  //include all the basics
#include <tchar.h>    //string and other mapping macros
#include <string>
#include <vector>
#include "resources.h"

//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//message processing function declarations
INT_PTR CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
void OnCommand(const HWND,int,int,const HWND);
INT_PTR OnInitDlg(const HWND,LPARAM);

//non-message function declarations
inline int ErrMsg(const ustring&);
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int)
{
HWND hDlg=CreateDialog(hInst,MAKEINTRESOURCE(IDD_DIALOG1),0,DlgProc);

if (!hDlg)
  {
  ErrMsg(_T("CreateDialog failed."));
  return 0;
  }
  
//Load keyboard accelerator so that it's available for use
HACCEL hAccelTable=LoadAccelerators(hInst,MAKEINTRESOURCE(IDR_ACCELERATOR1));
  
//start message loop - windows applications are 'event driven' waiting on user,
//application or system signals to determine what action, if any, to take. Note 
//that an error may cause GetMessage to return a negative value so, ideally,  
//this result should be tested for and appropriate action taken to deal with 
//it(the approach taken here is to simply quit the application).
MSG msg;
while (GetMessage(&msg,0,0,0)>0)
  {
  //first check if the message is from a resource defined keyboard accelerator key
  if (!TranslateAccelerator(hDlg,hAccelTable,&msg))
    {
    //get default keyboard message handling for dialog box eg control tabbing
    if (!IsWindow(hDlg) || !IsDialogMessage(hDlg,&msg))
      {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      }
    }
  }
return static_cast<int>(msg.wParam);
}
//=============================================================================
INT_PTR CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
switch (uMsg)
  {
  case WM_COMMAND:
    {
    OnCommand(hwnd,LOWORD(wParam),HIWORD(wParam),
              reinterpret_cast<HWND>(lParam));
    return 0;
    }
  case WM_DESTROY:
    PostQuitMessage(0);    //signal end of application
    return 0;
  case WM_INITDIALOG:
    {
    return OnInitDlg(hwnd,lParam);
    }
  default:
    return 0;  //let system deal with msg
  }
}
//=============================================================================
void OnCommand(const HWND hwnd,int id,int notifycode,const HWND hCntrl)
{
//handles WM_COMMAND message
if (!hCntrl)
  {
  ustring txt;
  switch (id)
    {
    case IDCANCEL:
      txt=_T("exit");
      break;
    case IDM_COPY:
      txt=_T("copy");
      break;
    case IDM_PASTE:
      txt=_T("paste");
      break;
    case IDM_CUT:
      txt=_T("cut");
      break;
    case IDM_ABOUT:
      txt=_T("about");
      break;
    case IDM_HELP:
      txt=_T("help");
      break;
    case IDM_OPEN:
      txt=_T("open");
      break;
    case IDM_SAVE:
      txt=_T("save");
      break;
    }
  if (notifycode==0)
    {
    ustring title=_T("Menu Selected");
    MessageBox(0,txt.c_str(),title.c_str(),MB_OK);
    }
  else if (notifycode==1)
    {
    ustring title=_T("Accelerator Selected");
    MessageBox(0,txt.c_str(),title.c_str(),MB_OK);
    }
  }
switch (id)
  {
  case IDOK:        //RETURN key pressed or 'ok' button selected
  case IDCANCEL:    //ESC key pressed or 'cancel' button selected
    DestroyWindow(hwnd);
  }
}
//=============================================================================
INT_PTR OnInitDlg(const HWND hwnd,LPARAM lParam)
{
//set the small icon for the dialog. IDI_APPLICATION icon is set by default 
//for winxp
SendMessage(hwnd,WM_SETICON,ICON_SMALL,
            reinterpret_cast<LPARAM>(LoadImage(0,IDI_APPLICATION,IMAGE_ICON,
                                               0,0,LR_SHARED)));
//ensure focus rectangle is properly draw around control with focus
PostMessage(hwnd,WM_KEYDOWN,VK_TAB,0);
return TRUE;
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
return MessageBox(0,s.c_str(),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
}
