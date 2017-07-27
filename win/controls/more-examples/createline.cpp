#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR lpCmdLine, int nCmdShow) {
    
    MSG  msg;
    WNDCLASSW wc = {0};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = L"Lines";
    wc.hInstance     = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc   = WndProc;
    wc.hCursor       = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"Lines",
          WS_OVERLAPPEDWINDOW | WS_VISIBLE,
          100, 100, 300, 200, NULL, NULL, hInstance, NULL);

    while (GetMessage(&msg, NULL, 0, 0)) {
        
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam) {
    
    HDC hdc;
    PAINTSTRUCT ps;

    switch(msg) {
    
        case WM_PAINT:

            hdc = BeginPaint(hwnd, &ps);

            MoveToEx(hdc, 50, 50, NULL);
            LineTo(hdc, 250, 50);

            HPEN hWhitePen = GetStockObject(WHITE_PEN);
            HPEN hOldPen = SelectObject(hdc, hWhitePen);

            MoveToEx(hdc, 50, 100, NULL);
            LineTo(hdc, 250, 100);

            SelectObject(hdc, hOldPen);

            EndPaint(hwnd, &ps);
            break;

        case WM_DESTROY:
 
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}