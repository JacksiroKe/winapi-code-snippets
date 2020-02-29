#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR lpCmdLine, int nCmdShow) {
    
    MSG  msg;
    WNDCLASSW wc = {0};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = L"Shapes";
    wc.hInstance     = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc   = WndProc;
    wc.hCursor       = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"Shapes",
          WS_OVERLAPPEDWINDOW | WS_VISIBLE,
          100, 100, 390, 230, NULL, NULL, hInstance, NULL);

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
    const POINT polygon[10] = { 30, 145, 85, 165, 105, 
        110, 65, 125, 30, 105 };

    switch(msg) {
  
        case WM_PAINT:

            hdc = BeginPaint(hwnd, &ps);
 
            Ellipse(hdc, 30, 30, 120, 90);
            RoundRect(hdc, 150, 30, 240, 90, 15, 20);
            Chord(hdc, 270, 30, 360, 90, 270, 45, 360, 45);
            Polygon(hdc, polygon, 5);
            Rectangle(hdc, 150, 110, 230, 160);

            EndPaint(hwnd, &ps);
            break;

        case WM_DESTROY:

            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}