#include <windows.h>
#include <commctrl.h>
#include <strsafe.h>

#define IDC_LIST 1
#define IDC_STATIC 2

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

typedef struct {
 
    wchar_t name[30]; 
    wchar_t job[20]; 
    int age; 

} Friends; 

Friends friends[] = {
 
    {L"Lucy", L"waitress", 18}, 
    {L"Thomas", L"programmer", 25}, 
    {L"George", L"police officer", 26}, 
    {L"Michael", L"producer", 38}, 
    {L"Jane", L"steward", 28}, 
}; 

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    PWSTR pCmdLine, int nCmdShow) {

    MSG  msg ;    
    WNDCLASSW wc = {0};
    wc.lpszClassName = L"MyListBox";
    wc.hInstance     = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc   = WndProc;
    wc.hCursor       = LoadCursor(0, IDC_ARROW);
  
    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"List Box",
                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                  100, 100, 340, 200, 0, 0, hInstance, 0);  

    while (GetMessage(&msg, NULL, 0, 0)) {
    
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, 
    WPARAM wParam, LPARAM lParam) {

    static HWND hwndList, hwndStatic;
    wchar_t buf[128];

    switch(msg) {

        case WM_CREATE:
       
            hwndList = CreateWindowW(WC_LISTBOXW , NULL, WS_CHILD 
                | WS_VISIBLE | LBS_NOTIFY, 10, 10, 150, 120, hwnd, 
                (HMENU) IDC_LIST, NULL, NULL);

            hwndStatic = CreateWindowW(WC_STATICW , NULL, WS_CHILD | WS_VISIBLE,
               200, 10, 120, 45, hwnd, (HMENU) IDC_STATIC, NULL, NULL);

            for (int i = 0; i < ARRAYSIZE(friends); i++)  { 
                 SendMessageW(hwndList, LB_ADDSTRING, 0, (LPARAM) friends[i].name);
            } 

            break;
 
        case WM_COMMAND:
        
            if (LOWORD(wParam) == IDC_LIST) {        
                if (HIWORD(wParam) == LBN_SELCHANGE) {                   
                    int sel = (int) SendMessageW(hwndList, LB_GETCURSEL, 0, 0);
                    StringCbPrintfW(buf, ARRAYSIZE(buf), L"Job: %ls\nAge: %d", 
                        friends[sel].job, friends[sel].age);
                    SetWindowTextW(hwndStatic, buf);
               }
            }            
            
            break;

        case WM_DESTROY:
        
            PostQuitMessage(0);
            break;
    }

    return (DefWindowProcW(hwnd, msg, wParam, lParam));
}