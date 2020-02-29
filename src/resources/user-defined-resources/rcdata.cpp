#include <windows.h>  //include all the basics
#include <tchar.h>    //string and other mapping macros
#include <string>
#include "resources.h"

//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;

struct ResData
{
ustring  name;
COLORREF colour;
};
//=============================================================================
//message processing function declarations
INT_PTR CALLBACK DlgProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
void OnCommand(const HWND,int,int,const HWND);
INT_PTR OnInitDlg(const HWND,LPARAM);

//non-message function declarations
inline int ErrMsg(const ustring&);

ResData GetData(const int,const TCHAR *pType=RT_RCDATA);
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE,LPSTR,int)
{
INT_PTR success=DialogBox(hInst,MAKEINTRESOURCE(IDD_DIALOG1),0,DlgProc);

if (success==-1)
  {
  ErrMsg(_T("DialogBox failed."));
  }
return 0;
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
  case WM_INITDIALOG:
    {
    return OnInitDlg(hwnd,lParam);
    }
  default:
    return FALSE;  //let system deal with msg
  }
}
//=============================================================================
void OnCommand(const HWND hwnd,int id,int notifycode,const HWND hCntrl)
{
//handles WM_COMMAND message of the modal dialogbox
switch (id)
  {
  case IDOK:        //RETURN key pressed or 'ok' button selected
  case IDCANCEL:    //ESC key pressed or 'cancel' button selected
    EndDialog(hwnd,id);
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

//load rcdata
ResData dt;                                        
dt=GetData(IDR_RCDATA);                                               
                                               
SetWindowText(GetDlgItem(hwnd,IDC_EDIT_NAME),dt.name.c_str());
SetDlgItemInt(hwnd,IDC_EDIT_COLOUR,dt.colour,FALSE);

dt=GetData(IDR_USERDATA,_T("MyUserData"));  
//dt=GetData(IDR_USERDATA,MAKEINTRESOURCE(100)); 
SetWindowText(GetDlgItem(hwnd,IDC_EDIT_USERNAME),dt.name.c_str());
SetDlgItemInt(hwnd,IDC_EDIT_USERCOLOUR,dt.colour,FALSE);

//ensure focus rectangle is properly draw around control with focus
PostMessage(hwnd,WM_KEYDOWN,VK_TAB,0);
return TRUE;
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
return MessageBox(0,s.c_str(),_T("ERROR"),MB_OK|MB_ICONEXCLAMATION);
}
//=============================================================================
ResData GetData(const int id,const TCHAR *pType)
{
//load user-defined or rcdata from resources
//Default: pType=RT_RCDATA
ResData dt;
HRSRC hRes=FindResource(0,MAKEINTRESOURCE(id),pType);
if (!hRes)
  {
  ErrMsg(_T("FindResource failed"));
  dt.name=_T("Error");
  return dt;
  }
DWORD dwSize=SizeofResource(0,hRes);
HGLOBAL hResMem=LoadResource(0,hRes);
if (!hRes)
  {
  ErrMsg(_T("LoadResource failed"));
  dt.name=_T("Error");
  return dt;
  }

char *pMem=reinterpret_cast<char*>(hResMem);
DWORD i=0;

while (pMem[i]!=_T('\0') && i<dwSize)
  {
  dt.name+=pMem[i];
  ++i;
  }

CopyMemory(&dt.colour,&pMem[i+1],sizeof(COLORREF));

return dt;
}