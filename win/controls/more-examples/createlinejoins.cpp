#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DoDrawing(HWND);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR lpCmdLine, int nCmdShow) {
    
    MSG  msg;
    WNDCLASSW wc = {0};

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName = L"Pens";
    wc.hInstance     = hInstance;
    wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.lpfnWndProc   = WndProc;
    wc.hCursor       = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    CreateWindowW(wc.lpszClassName, L"Line joins",
          WS_OVERLAPPEDWINDOW | WS_VISIBLE,
          100, 100, 450, 200, NULL, NULL, hInstance, NULL);

    while (GetMessage(&msg, NULL, 0, 0)) {
    
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam) {
    
    switch(msg) {
  
        case WM_PAINT:

            DoDrawing(hwnd);
            break;

        case WM_DESTROY:

            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void DoDrawing(HWND hwnd) {

    LOGBRUSH brush;
    COLORREF col = RGB(0, 0, 0);
    DWORD pen_style = PS_SOLID | PS_JOIN_MITER | PS_GEOMETRIC;

    brush.lbStyle = BS_SOLID;
    brush.lbColor = col;
    brush.lbHatch = 0;       

    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hwnd, &ps);

    HPEN hPen1 = ExtCreatePen(pen_style, 8, &brush, 0, NULL);
    HPEN holdPen = SelectObject(hdc, hPen1);
    
    POINT points[5] = { { 30, 30 }, { 130, 30 }, { 130, 100 }, 
        { 30, 100 }, { 30, 30}};
    Polygon(hdc, points, 5);
    
    pen_style = PS_SOLID | PS_GEOMETRIC | PS_JOIN_BEVEL;
    HPEN hPen2 = ExtCreatePen(pen_style, 8, &brush, 0, NULL);  

    SelectObject(hdc, hPen2);
    DeleteObject(hPen1);

    POINT points2[5] = { { 160, 30 }, { 260, 30 }, { 260, 100 }, 
        { 160, 100 }, {160, 30 }};
    MoveToEx(hdc, 130, 30, NULL);
    Polygon(hdc, points2, 5);

    pen_style = PS_SOLID | PS_GEOMETRIC | PS_JOIN_ROUND;
    HPEN hPen3 = ExtCreatePen(pen_style, 8, &brush, 0, NULL);  

    SelectObject(hdc, hPen3);
    DeleteObject(hPen2);

    POINT points3[5] = { { 290, 30 }, { 390, 30 }, { 390, 100 }, 
        { 290, 100 }, {290, 30 }};
    MoveToEx(hdc, 260, 30, NULL);
    Polygon(hdc, points3, 5);

    SelectObject(hdc, holdPen);
    DeleteObject(hPen3);
      
    EndPaint(hwnd, &ps);  
}