#include <windows.h>

/* window procedure */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdParam, int cmdShow)
{
    MSG messages;        /* for messages queue manipulation */
    WNDCLASSEX WndClass; /* data struct for window class */
    HWND hWnd;           /* handle for window */

    /* define window class */
    WndClass.cbSize = sizeof(WNDCLASSEX);
    WndClass.style = CS_DBLCLKS;
    WndClass.hInstance = hInst;
    WndClass.lpszClassName = "WindowClassName";
    WndClass.lpfnWndProc = WndProc;

    /* icons, cursor and menu */
    WndClass.hIcon = LoadIcon(hInst, "MAINICON"); /* default icon */
    WndClass.hIconSm = LoadIcon(hInst, "MAINICON"); /* default icon */
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW); /* cursor */
    WndClass.lpszMenuName = NULL; /* no menu */
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;

    /* window background color */
    WndClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE); 
    RegisterClassEx(&WndClass);
    hWnd = CreateWindowEx(0,                     /* extended window style */
                          "WindowClassName",     /* registered class */
                          "Windows Application", /* window title */
                          WS_OVERLAPPEDWINDOW,   /* default window style */
                          CW_USEDEFAULT,         /* x position */
                          CW_USEDEFAULT,         /* y position */
                          640,                   /* width of window */
                          480,                   /* heigth of window */
                          HWND_DESKTOP,          /* The window is a child-window to desktop */
                          NULL,                  /* no menu */ 
                          hInst,                 /* Program Instance handler */
                          NULL);                 /* No Window Creation data */
                          
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);  

    /* loop messages. run until GetMessage return 0*/  
    while (GetMessage(&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages); /* translate virtual keys into character messages*/
        DispatchMessage(&messages);  /* Send message to WndProc */
    }
    /* return value to system */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage(0); /* send a WM_QUIT to the message queue */
            break;
        default:                /* for messages that we don't deal with */
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}