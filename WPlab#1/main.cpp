#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif
#include <iostream>
#include <tchar.h>
#include <windows.h>

#define textColor     RGB(84,121,128)
#define bkColor       RGB(157,224,173)
#define customColorA  RGB(184, 58, 59)
#define customColorB  RGB(229,252,194)
#define BTN_EIN  10
#define BTN_ZWEI 20
#define ITEM_EIN  30
#define ITEM_ZWEI  40
 using namespace std;

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_HREDRAW | CS_VREDRAW;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = CreateSolidBrush( bkColor );

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("WP lab#1"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           800,                 /* The programs width */
           480,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{   HDC hdc = NULL;
    HDC hDC ;
    PAINTSTRUCT ps;
    RECT rect;
    static HWND buttonA, buttonB, textInput, textOutput;
    static short cxClient,cyClient;
    HFONT hfFont;
    HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
    static char textSz[255];
    static int index , colorChange = 0, backgroundChange = 0;
    char* fontList[] = {"Cambria", "Candara", "Bauhaus 93", "Calibri", "Helvetica", "Garamond", "Georgia", "Baskerville"};
    COLORREF colorList[] = {RGB(181,35,32), RGB(205,230,126), RGB(121,171,101), RGB(53,45,125), RGB(90,78,166), RGB(135,64,106), RGB(250,107,75), RGB(255,200,81)};


    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:

            buttonA = CreateWindow("BUTTON",
                                   "Accept",
                                   WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                   0, 0, 0, 0,
                                   hwnd,
                                   (HMENU) BTN_EIN, GetModuleHandle(NULL), NULL);

            buttonB = CreateWindow("BUTTON",
                                   "Random font and color",
                                   WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                   0, 0, 0, 0,
                                   hwnd,
                                   (HMENU) BTN_ZWEI, GetModuleHandle(NULL), NULL);

            textInput = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                                       "EDIT",
                                       "Text goes here ...",
                                       WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                                       0, 0, 0, 0,
                                       hwnd,
                                       (HMENU) ITEM_EIN, GetModuleHandle(NULL), NULL);

            textOutput = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                                        "EDIT",
                                        "",
                                        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_MULTILINE,
                                        0, 0, 0, 0,
                                        hwnd,
                                        (HMENU) ITEM_ZWEI, GetModuleHandle(NULL), NULL);

            hfFont = CreateFont(20,0,0,0,0,0,0,0,0,0,0,0,0,"Helvetica");
            SendMessage(buttonA, WM_SETFONT, (WPARAM)hfFont, MAKELPARAM(FALSE,0));
            SendMessage(buttonB, WM_SETFONT, (WPARAM)hfFont, MAKELPARAM(FALSE,0));

            break;

        case WM_SIZE:
            {
                cxClient = LOWORD(lParam);
                cyClient = HIWORD(lParam);

                MoveWindow ( buttonA, cxClient/2 - 300, cyClient/2 + 100 , 200, 40, TRUE);
                MoveWindow ( buttonB, cxClient/2 + 100 , cyClient/2 + 100, 200, 40, TRUE);
                MoveWindow ( textInput, cxClient/2 - 350, cyClient/2 - 200 , 300, 150, TRUE);
                MoveWindow ( textOutput, cxClient/2 + 50, cyClient/2 - 200 , 300, 150, TRUE);
                break;
            }

        break;

        case WM_COMMAND:
	         switch(LOWORD(wParam))
	         {
	             case BTN_EIN:
	                 {
                        SendMessage(textInput,
                                    WM_GETTEXT,
                                    sizeof(textSz) / sizeof(textSz[0]),
                                    reinterpret_cast<LPARAM>(textSz));

                        SendMessage(textOutput, WM_SETTEXT, 0, (LPARAM)textSz);


	                     break;
	                 }

                 case BTN_ZWEI:
                     {  index = rand() % 7;
                        hfFont = CreateFont(20,0,0,0,0,0,0,0,0,0,0,0,0,fontList[index]);
                        SendMessage(textOutput, WM_SETFONT, (WPARAM)hfFont, 1);
                        colorChange = 1;
                        backgroundChange = 1;
                        InvalidateRect(textOutput, NULL, TRUE);
                        break;
                     }
	         }

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            GetClientRect(hwnd, &rect);
            SetBkColor(hdc, bkColor);
            SetBkMode(hdc, OPAQUE);
            SetTextColor(hdc, textColor);
            DrawText(hdc, TEXT("Done with Pride and Prejudice by Vlad"), -1, &rect, DT_SINGLELINE| DT_CENTER| DT_VCENTER);
            EndPaint(hwnd, &ps);
            break;

         case WM_CTLCOLORSTATIC:

                if(colorChange == true && (HWND)lParam == textOutput)
                {
                    HBRUSH hbr = (HBRUSH) DefWindowProc(hwnd, message, wParam, lParam);
                    index = rand() % 7;
                    SetTextColor((HDC) wParam, colorList[index]);
                    colorChange = 0;
                    HDC hdcStatic = (HDC) wParam;
                    SetBkColor(hdcStatic, customColorB);
                    return (INT_PTR)CreateSolidBrush(customColorB);
                }
                else if(colorChange == false && (HWND)lParam == textOutput)
                {
                    HDC hdcStatic = (HDC) wParam;
                    SetBkColor(hdcStatic, customColorB);
                    return (INT_PTR)CreateSolidBrush(customColorB);
                }
                break;

        case WM_CTLCOLOREDIT:
            {
               if(ITEM_EIN == GetDlgCtrlID((HWND)lParam))
               {
                    HDC hdcStatic = (HDC) wParam;
                    SetBkColor(hdcStatic, customColorB);

                    return (INT_PTR)CreateSolidBrush(customColorB);
                }
                break;
            }


        case WM_SYSCOMMAND:
        {
            switch(wParam)
            {
                case SC_MINIMIZE:
                {
                    return MessageBox(NULL, "¿Que?", "Dondo esta la biblioteca ?", MB_OKCANCEL | MB_ICONINFORMATION);
                }

                case SC_MAXIMIZE:
                {
                    return MessageBox(NULL, "Nope", "Maximization attempt", MB_OK | MB_ICONINFORMATION);
                    break;
                }

                case SC_CLOSE:
                {
                    if(MessageBox(hwnd, "Are you sure you want to quit ?", "sinep", MB_YESNO) == IDYES)
                    {
                        exit(1);
                    }
                    break;
                }

                default:
                    return DefWindowProc(hwnd, message, wParam, lParam);
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
