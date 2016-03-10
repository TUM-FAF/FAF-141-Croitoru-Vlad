#include <windows.h>
#include "resource.h"

int iMinWindowHeight = 600;
int iMinWindowWidth  = 480;

BOOL CALLBACK DialogProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

char szClassName[ ] = "Lab2Class";
HINSTANCE hProgramInstance;

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon(hThisInstance, MAKEINTRESOURCE(IDI_ICON));
    wincl.hIconSm = LoadIcon(hThisInstance, MAKEINTRESOURCE(IDI_ICON));
    wincl.hCursor = LoadCursor(hThisInstance, MAKEINTRESOURCE(IDC_CURSOR));
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = CreateSolidBrush( bkColor );
    //wincl.hbrBackground = CreatePatternBrush(LoadBitmap(hThisInstance, MAKEINTRESOURCE(IDB_BACKGROUND)));


    if(!RegisterClassEx(&wincl)) return 0;

    hwnd = CreateWindowEx (
        0,
        szClassName,
        "Make America great again #2",
        WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT,
        iMinWindowWidth, iMinWindowHeight,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL);

    ShowWindow (hwnd, nCmdShow);

    while (GetMessage (&messages, NULL, 0, 0)) {
      TranslateMessage(&messages);
      DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // Child windows' handles
    static HWND hwndListBox, hwndNewItem, hwndAddButton, hwndRemoveButton, hwndClearButton, hwndHeightScroll, hwndWidthScroll;
    static HINSTANCE hInstance;

    // Size and position variables
    int iSysWidth;
    int iSysHeight;
    int iWinWidth;
    int iWinHeight;

    int iWidth  = 60;   // Button width
    int iHeight = 30;   // Button height
    int x;
    int y;

    int i, cxChar, cyChar;

    // Menu & menu items
    HMENU hMenu;
    HMENU hSubMenu;

    // String
    char* szText;
    int iTextLength;

    // Paint and size structs
    TEXTMETRIC tm;
    SCROLLINFO si;
    HBRUSH brush;
    RECT rect, rct;
    int color;
    HDC hdc;


    hdc = GetDC(hwnd);
    GetTextMetrics(hdc, &tm);
    cxChar = tm.tmAveCharWidth;
    cyChar = tm.tmHeight;
    ReleaseDC(hwnd, hdc);

    switch(message) {

        case WM_CREATE:
            hwndListBox = CreateWindowEx(
                (DWORD)NULL,
                TEXT("ListBox"),
                NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | LBS_WANTKEYBOARDINPUT,
                0, 0, 0, 0, hwnd,
                (HMENU)IDC_LIST_BOX,
                hProgramInstance,
                NULL);

            hwndNewItem = CreateWindowEx(
                (DWORD)NULL,
                TEXT("Edit"),
                TEXT("Input candidates"),
                WS_CHILD | WS_VISIBLE | WS_BORDER |ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                0, 0, 0, 0, hwnd,
                (HMENU)IDC_NEW_ITEM,
                hProgramInstance,
                NULL);

            hwndWidthScroll = CreateWindow(
                "Scrollbar",
                NULL,
                WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN,
                0, 0, 0, 0, hwnd,
                (HMENU)IDC_WIDTH_SCROLL,
                hProgramInstance,
                NULL);
            SetScrollRange(hwndWidthScroll, SB_CTL, 0, 100, FALSE);
            SetScrollPos(hwndWidthScroll, SB_CTL, 0, FALSE);

            hwndHeightScroll = CreateWindow(
                "Scrollbar",
                NULL,
                WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_BOTTOMALIGN,
                0, 0, 0, 0, hwnd,
                (HMENU)IDC_HEIGHT_SCROLL,
                hProgramInstance,
                NULL);
            SetScrollRange(hwndHeightScroll, SB_CTL, 0, 100, TRUE);
            SetScrollPos(hwndHeightScroll, SB_CTL, 0, FALSE);

            hwndAddButton = CreateWindowEx(
                (DWORD)NULL,
                TEXT("Button"),
                TEXT("Add"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                0, 0, 0, 0, hwnd,
                (HMENU)IDC_ADD_BUTTON,
                hProgramInstance,
                NULL);

            hwndRemoveButton = CreateWindowEx(
                (DWORD)NULL,
                TEXT("Button"),
                TEXT("Remove"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                0, 0, 0, 0, hwnd,
                (HMENU)IDC_REMOVE_BUTTON,
                hProgramInstance,
                NULL);

            hwndClearButton = CreateWindowEx(
                (DWORD)NULL,
                TEXT("Button"),
                TEXT("Clear"),
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                0, 0, 0, 0, hwnd,
                (HMENU)IDC_CLEAR_BUTTON,
                hProgramInstance,
                NULL);


            // Create menu
            hMenu = CreateMenu();

            // Add "File" menu, with "Exit" submenu
            hSubMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&File");
            AppendMenu(hSubMenu, MF_STRING, IDC_FILE_EXIT, "&Exit");

            hSubMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Edit");
            AppendMenu(hSubMenu, MF_STRING, IDC_MOVE, "&Jump around");

            hSubMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "&Help");
            AppendMenu(hSubMenu, MF_STRING, IDC_HELP_ABOUT, "&About");

            // Set the menu
            SetMenu(hwnd, hMenu);
            break;

        case WM_SIZE:
            iWidth  = LOWORD(lParam);
            iHeight = HIWORD(lParam);

            // Set vertical scroll bar range and page size
            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = ((iMinWindowHeight - 70) / cyChar);
            si.nPage = iHeight / cyChar;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

            // Set horizontal scroll bar range and page size
            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = ((iMinWindowWidth - 20) / cxChar);
            si.nPage = iWidth / cxChar;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

            // CHILD WINDOWS REPOSITION BEGIN

            MoveWindow(hwndListBox, iWidth/2 - 210, iHeight/2 - 150, 300, 350, TRUE);
            MoveWindow(hwndNewItem, iWidth/2 - 210, iHeight/2 - 220, 300, 50, TRUE);
            MoveWindow(hwndWidthScroll, iWidth/2 - 210, iHeight/2 + 200, 410, 20, TRUE);
            MoveWindow(hwndHeightScroll, iWidth/2 + 180, iHeight/2 - 220, 20, 410, TRUE);
            MoveWindow(hwndAddButton, iWidth/2 + 100, iHeight/2 - 210, 60, 30, TRUE);
            MoveWindow(hwndRemoveButton, iWidth/2 + 100, iHeight/2 - 140, 60, 30, TRUE);
            MoveWindow(hwndClearButton, iWidth/2 + 100 , iHeight/2- 100, 60, 30, TRUE);

            // CHILD WINDOWS REPOSITION END
            GetWindowRect(hwnd, &rect);
            iWinWidth = rect.right - rect.left;
            iWinHeight = rect.bottom - rect.top;
            iSysWidth = GetSystemMetrics(SM_CXSCREEN);
            iSysHeight = GetSystemMetrics(SM_CYSCREEN);

            // Set width scrollbar position
            SetScrollPos(hwndWidthScroll, SB_CTL, (iWinWidth * 100 / iSysWidth), TRUE);

            // Set height scrollbar position
            SetScrollPos(hwndHeightScroll, SB_CTL, (iWinHeight * 100 / iSysHeight), TRUE);
            break;

        case WM_VSCROLL:
            GetWindowRect(hwnd, &rect);
            iWinWidth = rect.right - rect.left;
            iWinHeight = rect.bottom - rect.top;
            iSysWidth = GetSystemMetrics(SM_CXSCREEN);
            iSysHeight = GetSystemMetrics(SM_CYSCREEN);

            if(GetWindowLong((HWND)lParam, GWL_ID) == IDC_HEIGHT_SCROLL) {
                si.cbSize = sizeof(si);
                si.fMask = SIF_ALL;
                GetScrollInfo(hwndHeightScroll, SB_CTL, &si);
                y = si.nPos;
                switch(LOWORD(wParam)) {
                    case SB_LINELEFT:
                        si.nPos -= 1;
                        break;
                    case SB_LINERIGHT:
                        si.nPos += 1;
                        break;
                    case SB_THUMBPOSITION:
                        si.nPos = si.nTrackPos;
                        break;
                    default:
                        break;
                }
                si.fMask = SIF_POS;
                SetScrollInfo(hwndHeightScroll, SB_CTL, &si, TRUE);
                GetScrollInfo(hwndHeightScroll, SB_CTL, &si);
                if(si.nPos != y) {
                    SetScrollPos(hwndHeightScroll, SB_CTL, si.nPos, TRUE);
                }
                // Set window height
                MoveWindow(hwnd, rect.left, rect.top, iWinWidth, (si.nPos * iSysHeight / 100), TRUE);
                break;
            }


            // Get all the vertical scroll bar information
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &si);

            // Save the position for later comparison
            x = si.nPos;


            switch(LOWORD(wParam)) {
                case SB_TOP:
                    si.nPos = si.nMin;
                    break;

                case SB_BOTTOM:
                    si.nPos = si.nMax;
                    break;

                case SB_LINEUP:
                    si.nPos -= 1;
                    break;

                case SB_LINEDOWN:
                    si.nPos += 1;
                    break;

                case SB_PAGEUP:
                    si.nPos -= si.nPage;
                    break;

                case SB_PAGEDOWN:
                    si.nPos += si.nPage;
                    break;

                case SB_THUMBTRACK:
                    si.nPos = si.nTrackPos;
                    break;

                default:
                    break;
            }

            // Set the position and then retrieve it
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            GetScrollInfo(hwnd, SB_VERT, &si);

            // If the position has changed, scroll the window and update it
            if(si.nPos != x) {
                ScrollWindow(hwnd, 0, cyChar * (y - si.nPos), NULL, NULL);
                UpdateWindow(hwnd);
            }
            break;

        case WM_HSCROLL:
            GetWindowRect(hwnd, &rect);
            iWinWidth = rect.right - rect.left;
            iWinHeight = rect.bottom - rect.top;
            iSysWidth = GetSystemMetrics(SM_CXSCREEN);
            iSysHeight = GetSystemMetrics(SM_CYSCREEN);

            if(GetWindowLong((HWND)lParam, GWL_ID) == IDC_WIDTH_SCROLL) {
                si.cbSize = sizeof(si);
                si.fMask = SIF_ALL;
                GetScrollInfo(hwndWidthScroll, SB_CTL, &si);
                x = si.nPos;
                switch(LOWORD(wParam)) {
                    case SB_LINELEFT:
                        si.nPos -= 1;
                        break;
                    case SB_LINERIGHT:
                        si.nPos += 1;
                        break;
                    case SB_THUMBPOSITION:
                        si.nPos = si.nTrackPos;
                        break;
                    default:
                        break;
                }
                si.fMask = SIF_POS;
                SetScrollInfo(hwndWidthScroll, SB_CTL, &si, TRUE);
                GetScrollInfo(hwndWidthScroll, SB_CTL, &si);
                if(si.nPos != x) {
                    SetScrollPos(hwndWidthScroll, SB_CTL, si.nPos, TRUE);
                }
                // Set window width
                MoveWindow(hwnd, rect.left, rect.top, (si.nPos * iSysWidth / 100), iWinHeight, TRUE);
                break;
            }

           // Get all the vertical scroll bar information
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_HORZ, &si);
            // Save the position for later comparison
            x = si.nPos;
            switch(LOWORD(wParam)) {
                case SB_LINELEFT:
                    si.nPos -= 1;
                    break;

                case SB_LINERIGHT:
                    si.nPos += 1;
                    break;

                case SB_PAGELEFT:
                    si.nPos -= si.nPage;
                    break;

                case SB_PAGERIGHT:
                    si.nPos += si.nPage;
                    break;

                case SB_THUMBPOSITION:
                    si.nPos = si.nTrackPos;
                    break;

                default:
                    break;
            }
            // Set the position and then retrieve it
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            GetScrollInfo(hwnd, SB_HORZ, &si);

            // If the position has changed, scroll the window and update it
            if(si.nPos != x) {
                ScrollWindow(hwnd, cxChar * (x - si.nPos), 0, NULL, 0);
                UpdateWindow(hwnd);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_LIST_BOX:
                    switch (HIWORD(wParam)) {
                        case LBN_DBLCLK:
                            MessageBox(NULL, TEXT("Candidate Selected!"), TEXT("Congrats!"), MB_OK);
                            break;
                    }
                    break;

                case IDC_ADD_BUTTON:
                    iTextLength = SendMessage(hwndNewItem, WM_GETTEXTLENGTH, 0, 0);
                    szText = (char*)malloc(iTextLength+1);
                    SendMessage(hwndNewItem, WM_GETTEXT, iTextLength+1, (LPARAM)szText);
                    SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)szText);
                    SendMessage(hwndNewItem, WM_SETTEXT, 0, (LPARAM)"");
                    free(szText);
                    break;

                case IDC_REMOVE_BUTTON:
                    i = SendMessage(hwndListBox, LB_GETCURSEL, 0, 0);
                    if(i != LB_ERR) {
                        SendMessage(hwndListBox, LB_DELETESTRING, i, 0);
                    }
                    break;

                case IDC_CLEAR_BUTTON:
                    SendMessage(hwndListBox, LB_RESETCONTENT, 0, 0);
                    break;

                case IDC_FILE_EXIT:
                    SendMessage(hwnd, WM_DESTROY, 0, 0);
                    break;

                case IDC_MOVE:
                            int xPos, yPos;
                            xPos = rand() % 1000;
                            yPos = rand() % 800;
                            SetWindowPos(hwnd, HWND_TOP, xPos, yPos, 0, 0, SWP_NOSIZE);
                            break;

                case IDC_HELP_ABOUT:
                    return DialogBox(hProgramInstance, MAKEINTRESOURCE(IDD_ABOUT), NULL, (DLGPROC)DialogProcedure);
                    break;

                default:
                    DefWindowProc(hwnd, WM_COMMAND, wParam, lParam);
                    break;
            }
            break;

        case WM_KEYDOWN:
            GetWindowRect(hwnd, &rect);
            iWinWidth = rect.right - rect.left;
            iWinHeight = rect.bottom - rect.top;
            iSysWidth = GetSystemMetrics(SM_CXSCREEN);
            iSysHeight = GetSystemMetrics(SM_CYSCREEN);

            // Move window right on Shift + Arrow Right
            if((wParam == VK_RIGHT)&&(HIBYTE(GetKeyState(VK_SHIFT)) > 1)) {
                if(rect.left >= iSysWidth - iWinWidth) {
                    MoveWindow(hwnd, iSysWidth - iWinWidth, rect.top, iWinWidth, iWinHeight, TRUE);
                }
                else {
                    MoveWindow(hwnd, rect.left + 5, rect.top, iWinWidth, iWinHeight, TRUE);
                }
                break;
            }
            // Move window left on Shift + Arrow Left
            if((wParam == VK_LEFT)&&(HIBYTE(GetKeyState(VK_SHIFT)) > 1)) {
                if(rect.left <= 0) {
                    MoveWindow(hwnd, 0, rect.top, iWinWidth, iWinHeight, TRUE);
                }
                else {
                    MoveWindow(hwnd, rect.left - 5, rect.top, iWinWidth, iWinHeight, TRUE);
                }
                break;
            }
            // Move window down on Shift + Arrow Down
            if((wParam == VK_DOWN)&&(HIBYTE(GetKeyState(VK_SHIFT)) > 1)) {
                if(rect.top >= iSysHeight - iWinHeight) {
                    MoveWindow(hwnd, rect.left, iSysHeight - iWinHeight, iWinWidth, iWinHeight, TRUE);
                }
                else {
                    MoveWindow(hwnd, rect.left, rect.top + 5, iWinWidth, iWinHeight, TRUE);
                }
                break;
            }
            // Move window up on Shift + Arrow Up
            if((wParam == VK_UP)&&(HIBYTE(GetKeyState(VK_SHIFT)) > 1)) {
                if(rect.top <= 0) {
                    MoveWindow(hwnd, rect.left, 0, iWinWidth, iWinHeight, TRUE);
                }
                else {
                    MoveWindow(hwnd, rect.left, rect.top - 5, iWinWidth, iWinHeight, TRUE);
                }
                break;
            }
            return DefWindowProc (hwnd, WM_PAINT, wParam, lParam);


        case WM_PAINT:
            return DefWindowProc (hwnd, WM_PAINT, wParam, lParam);
            break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;

        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

BOOL CALLBACK DialogProcedure(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg) {
        case WM_INITDIALOG:
            return TRUE;


        case WM_CLOSE:
            EndDialog(hwndDlg, 0);
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hwndDlg, 0);
                    return TRUE;
            }
            break;
    }
    return FALSE;
}
