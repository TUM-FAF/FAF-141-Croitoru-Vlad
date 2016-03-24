#include <windows.h>
#include <windowsx.h>

#define IDW_DRAWING_AREA    10
#define IDB_PEN             20
#define IDB_LINE            30
#define IDB_POLYGON         40
#define IDB_ELLIPSE         50
#define IDB_BEZIER          60
#define IDB_ERASER          70
#define BkColor             RGB(235, 2, 254)
#define BkColorA            RGB(0, 128, 129)
#define BkColorB            RGB(235, 2, 254)

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void updateColorPreview(HDC, COLORREF, int, int);
int getWeight(HWND);
POINT drawingLimits(int, int, RECT, int);
HRGN getLastRectRegion(RECT, int, BOOL*);
HRGN getLastEllipticRegion(RECT, int, BOOL*);

char szClassName[ ] = "Lab#3";
HINSTANCE hInstance;

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = 0;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = CreateSolidBrush( BkColorA );

    if(!RegisterClassEx(&wincl)) return 0;

    hwnd = CreateWindowEx (
        0,
        szClassName,
        "Laboratory Work #3",
        (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX),
        CW_USEDEFAULT, CW_USEDEFAULT,
        900, 680,
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

    static HWND hwndEraserWeight;
    static HWND hwndStrokeWeight;
    static HWND hwndPenTool;
    static HWND hwndLineTool;
    static HWND hwndPolygonTool;
    static HWND hwndEllipseTool;
    static HWND hwndBezierTool;
    static HWND hwndEraserTool;
    static HWND hwndFillCheck;

    static POINT pen;
    static BOOL firstLine;
    static POINT newLine;
    static BOOL firstPolygon;
    static RECT newPolygon;
    static BOOL firstEllipse;
    static RECT newEllipse;
    static BOOL firstBezier;
    static BOOL secondBezier;
    static BOOL thirdBezier;
    static POINT pointsforBezier[4];
    static HRGN lastRegion;
    static BOOL regionDeleted;

    int xMouse, yMouse;

    static RECT drawingArea = {15, 180, 580, 640};

    RECT rectRED     = {255, 100, 375, 115};
    RECT rectGREEN   = {255, 120, 375, 135};
    RECT rectBLUE    = {255, 140, 375, 155};
    RECT rectTemp;

    HDC hdc = GetDC(hwnd);
    COLORREF strokeRGB;
    int strokeWeight;
    COLORREF fillRGB;
    HBRUSH fillBrush;
    PAINTSTRUCT ps;
    HPEN strokePen;
    POINT point;
    RECT rect;

    HBRUSH hBrush;
    int xFillPreview =240 ;
    int yFillPreview = 50;
    int xStrokePreview = 320;
    int yStrokePreview = 50;
    UINT fillRED      = 255;
    UINT fillGREEN    = 255;
    UINT fillBLUE     = 255;
    UINT strokeRED    = 0;
    UINT strokeGREEN  = 0;
    UINT strokeBLUE   = 0;

    HDC hdcMem;
    BITMAP bitmap;
    HBITMAP hbmpimage = NULL;
    hbmpimage = (HBITMAP)LoadImage(hInstance, "aesthetic1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    GetObject(hbmpimage, sizeof(bitmap), &bitmap);

    switch(message) {

        case WM_CREATE:

            CreateWindowEx(
                0,
                "Button",
                "Tools",
                WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                5, 5,
                100, 160,
                hwnd,
                0,
                hInstance,
                NULL);

            CreateWindowEx(
                0,
                "Button",
                "Style",
                WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                520, 5,
                135, 120,
                hwnd,
                0,
                hInstance,
                NULL);

            CreateWindowEx(
                0,
                "Button",
                "Color",
                WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                220, 5,
                180, 160,
                hwnd,
                0,
                hInstance,
                NULL);

            CreateWindowEx(
                0,
                "Static",
                "Fill",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                240, 25,
                45, 20,
                hwnd,
                (HMENU)0,
                hInstance,
                NULL);

            CreateWindowEx(
                0,
                "Static",
                "Stroke",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                320, 25,
                45, 20,
                hwnd,
                (HMENU)0,
                hInstance,
                NULL);

            CreateWindowEx(
                0,
                "Static",
                "R",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                240, 100,
                10, 15,
                hwnd,
                (HMENU)0,
                hInstance,
                NULL);

            CreateWindowEx(
                0,
                "Static",
                "G",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                240, 120,
                10, 15,
                hwnd,
                (HMENU)0,
                hInstance,
                NULL);

            CreateWindowEx(
                0,
                "Static",
                "B",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                240, 140,
                10, 15,
                hwnd,
                (HMENU)0,
                hInstance,
                NULL);

            hwndPenTool = CreateWindowEx(
                0,
                "Button",
                "Pen",
                WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON,
                15, 30,
                80, 18,
                hwnd,
                (HMENU)IDB_PEN,
                hInstance,
                NULL);

            Button_SetCheck(hwndPenTool, BST_CHECKED);

            hwndLineTool = CreateWindowEx(
                0,
                "Button",
                "Line",
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                15, 55,
                80, 18,
                hwnd,
                (HMENU)IDB_LINE,
                hInstance,
                NULL);

            hwndPolygonTool = CreateWindowEx(
                0,
                "Button",
                "Polygon",
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                15, 80,
                80, 18,
                hwnd,
                (HMENU)IDB_POLYGON,
                hInstance,
                NULL);

            hwndEllipseTool = CreateWindowEx(
                0,
                "Button",
                "Ellipse",
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                15, 105,
                80, 18,
                hwnd,
                (HMENU)IDB_ELLIPSE,
                hInstance,
                NULL);

            hwndBezierTool = CreateWindowEx(
                0,
                "Button",
                "Bezier",
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                15, 130,
                80, 18,
                hwnd,
                (HMENU)IDB_BEZIER,
                hInstance,
                NULL);

            hwndFillCheck = CreateWindowEx(
                0,
                "Button",
                "Fill",
                WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                530, 30,
                80, 18,
                hwnd,
                (HMENU)0,
                hInstance,
                NULL);

            CreateWindowEx(
                0,
                "Static",
                "Stroke",
                WS_VISIBLE | WS_CHILD,
                530, 60,
                80, 20,
                hwnd,
                (HMENU)0,
                hInstance,
                NULL);

            hwndStrokeWeight = CreateWindowEx(
                0,
                "Edit",
                "1",
                WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
                620, 60,
                20, 20,
                hwnd,
                (HMENU)0,
                hInstance,
                NULL);

            hwndEraserTool = CreateWindowEx(
                0,
                "Button",
                "Eraser",
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                530, 90,
                80, 20,
                hwnd,
                (HMENU)IDB_ERASER,
                hInstance,
                NULL);

            hwndEraserWeight = CreateWindowEx(
                0,
                "Edit",
                "1",
                WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,
                620, 90,
                20, 20,
                hwnd, (HMENU)0,
                hInstance,
                NULL);
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                default:
                    DefWindowProc(hwnd, WM_COMMAND, wParam, lParam);
                    break;
            }
            return 0;

        case WM_LBUTTONDOWN:
            xMouse = GET_X_LPARAM(lParam);
            yMouse = GET_Y_LPARAM(lParam);

            if((xMouse > rectRED.left)&&(xMouse <= rectRED.right)) {
                strokeRGB = GetPixel(hdc, xStrokePreview + 20, yStrokePreview + 20);

                if((yMouse > rectRED.top)&&(yMouse <= rectRED.bottom)) {
                    strokeRED = (xMouse - rectRED.left) * 255 / (rectRED.right - rectRED.left);
                    strokeGREEN = GetGValue(strokeRGB);
                    strokeBLUE = GetBValue(strokeRGB);
                    updateColorPreview(hdc, RGB(strokeRED, strokeGREEN, strokeBLUE), xStrokePreview, yStrokePreview);
                }

                else if((yMouse > rectGREEN.top)&&(yMouse <= rectGREEN.bottom)) {
                    strokeRED = GetRValue(strokeRGB);
                    strokeGREEN = (xMouse - rectGREEN.left) * 255 / (rectGREEN.right - rectGREEN.left);
                    strokeBLUE = GetBValue(strokeRGB);
                    updateColorPreview(hdc, RGB(strokeRED, strokeGREEN, strokeBLUE), xStrokePreview, yStrokePreview);
                }

                else if((yMouse > rectBLUE.top)&&(yMouse <= rectBLUE.bottom)) {
                    strokeRED = GetRValue(strokeRGB);
                    strokeGREEN = GetGValue(strokeRGB);
                    strokeBLUE = (xMouse - rectBLUE.left) * 255 / (rectBLUE.right - rectBLUE.left);
                    updateColorPreview(hdc, RGB(strokeRED, strokeGREEN, strokeBLUE), xStrokePreview, yStrokePreview);
                }
                return 0;
            }

                if((xMouse > drawingArea.left)&&(xMouse < drawingArea.right)&&(yMouse > drawingArea.top)&&(yMouse < drawingArea.bottom)) {
                strokeWeight = getWeight(hwndStrokeWeight);
                point = drawingLimits(xMouse, yMouse, drawingArea, strokeWeight);
                xMouse = point.x;
                yMouse = point.y;

                if((wParam == MK_LBUTTON)&&(Button_GetCheck(hwndPenTool) == BST_CHECKED)) {
                    pen.x = xMouse;
                    pen.y = yMouse;
                }

                if((wParam == MK_LBUTTON)&&(Button_GetCheck(hwndLineTool) == BST_CHECKED)) {
                    newLine.x = xMouse;
                    newLine.y = yMouse;
                    firstLine = true;
                }

                if((wParam == MK_LBUTTON)&&(Button_GetCheck(hwndPolygonTool) == BST_CHECKED)) {
                    newPolygon.left = xMouse;
                    newPolygon.top = yMouse;
                    firstPolygon = true;
                }

                if((wParam == MK_LBUTTON)&&(Button_GetCheck(hwndEllipseTool) == BST_CHECKED)) {
                    newEllipse.left = xMouse;
                    newEllipse.top = yMouse;
                    firstEllipse = true;
                }

                if((wParam == MK_LBUTTON)&&(Button_GetCheck(hwndBezierTool) == BST_CHECKED)) {
                    pointsforBezier[0] = point;
                    firstBezier  = true;
                    secondBezier = false;
                    thirdBezier  = false;
                }
            }
            return 0;

        case WM_LBUTTONUP:

            xMouse = GET_X_LPARAM(lParam);
            yMouse = GET_Y_LPARAM(lParam);

            strokeRGB = GetPixel(hdc, xStrokePreview + 20, yStrokePreview + 20);
            fillRGB = GetPixel(hdc, xFillPreview + 20, yFillPreview + 20);
            strokeWeight = getWeight(hwndStrokeWeight);

            if(firstLine) {
                point = drawingLimits(xMouse, yMouse, drawingArea, strokeWeight);
                xMouse = point.x;
                yMouse = point.y;

                strokePen = CreatePen(PS_SOLID, strokeWeight, strokeRGB);
                SelectObject(hdc, strokePen);
                MoveToEx(hdc, xMouse, yMouse, NULL);
                LineTo(hdc, newLine.x, newLine.y);
                DeleteObject(strokePen);

                firstLine = false;
            }

            if(firstPolygon) {
                point = drawingLimits(xMouse, yMouse, drawingArea, strokeWeight);
                newPolygon.right = point.x;
                newPolygon.bottom = point.y;

                strokePen = CreatePen(PS_SOLID, strokeWeight, strokeRGB);
                fillBrush = (Button_GetCheck(hwndFillCheck) == BST_CHECKED)? CreateSolidBrush(fillRGB) : (HBRUSH)GetStockObject(NULL_BRUSH);
                SelectObject(hdc, strokePen);
                SelectObject(hdc, fillBrush);
                Rectangle(hdc, newPolygon.left, newPolygon.top, newPolygon.right, newPolygon.bottom);
                DeleteObject(strokePen);
                DeleteObject(fillBrush);

                lastRegion = getLastRectRegion(newPolygon, strokeWeight, &regionDeleted);

                firstPolygon = false;
            }

            if(firstEllipse) {
                point = drawingLimits(xMouse, yMouse, drawingArea, strokeWeight);
                newEllipse.right = point.x;
                newEllipse.bottom = point.y;

                strokePen = CreatePen(PS_SOLID, strokeWeight, strokeRGB);
                fillBrush = (Button_GetCheck(hwndFillCheck) == BST_CHECKED)? CreateSolidBrush(fillRGB) : (HBRUSH)GetStockObject(NULL_BRUSH);
                SelectObject(hdc, strokePen);
                SelectObject(hdc, fillBrush);
                Ellipse(hdc, newEllipse.left, newEllipse.top, newEllipse.right, newEllipse.bottom);
                DeleteObject(strokePen);
                DeleteObject(fillBrush);

                lastRegion = getLastEllipticRegion(newEllipse, strokeWeight, &regionDeleted);

                firstEllipse = false;
            }

            if(firstBezier) {
                point = drawingLimits(xMouse, yMouse, drawingArea, strokeWeight);
                pointsforBezier[1] = point;
                firstBezier  = false;
                secondBezier = true;
                thirdBezier  = false;
            }
            return 0;

        case WM_RBUTTONDOWN:
            xMouse = GET_X_LPARAM(lParam);
            yMouse = GET_Y_LPARAM(lParam);

            if((xMouse > rectRED.left)&&(xMouse <= rectRED.right)) {
                fillRGB = GetPixel(hdc, xFillPreview + 20, yFillPreview + 20);

                if((yMouse > rectRED.top)&&(yMouse <= rectRED.bottom)) {
                    fillRED = (xMouse - rectRED.left) * 255 / (rectRED.right - rectRED.left);
                    fillGREEN = GetGValue(fillRGB);
                    fillBLUE = GetBValue(fillRGB);
                    updateColorPreview(hdc, RGB(fillRED, fillGREEN, fillBLUE), xFillPreview, yFillPreview);
                }

                else if((yMouse > rectGREEN.top)&&(yMouse <= rectGREEN.bottom)) {
                    fillRED = GetRValue(fillRGB);
                    fillGREEN = (xMouse - rectGREEN.left) * 255 / (rectGREEN.right - rectGREEN.left);
                    fillBLUE = GetBValue(fillRGB);
                    updateColorPreview(hdc, RGB(fillRED, fillGREEN, fillBLUE), xFillPreview, yFillPreview);
                }

                else if((yMouse > rectBLUE.top)&&(yMouse <= rectBLUE.bottom)) {
                    fillRED = GetRValue(fillRGB);
                    fillGREEN = GetGValue(fillRGB);
                    fillBLUE = (xMouse - rectBLUE.left) * 255 / (rectBLUE.right - rectBLUE.left);
                    updateColorPreview(hdc, RGB(fillRED, fillGREEN, fillBLUE), xFillPreview, yFillPreview);
                }
                return 0;
            }

            if((xMouse > drawingArea.left)&&(xMouse < drawingArea.right)&&(yMouse > drawingArea.top)&&(yMouse < drawingArea.bottom)) {
                strokeWeight = getWeight(hwndStrokeWeight);
                point = drawingLimits(xMouse, yMouse, drawingArea, strokeWeight);
                xMouse = point.x;
                yMouse = point.y;

                if((wParam == MK_RBUTTON)&&(Button_GetCheck(hwndBezierTool) == BST_CHECKED)&&(secondBezier)) {
                    pointsforBezier[2] = point;
                    firstBezier  = false;
                    secondBezier = false;
                    thirdBezier  = true;
                }
            }
            if(regionDeleted) {
                InvalidateRgn(hwnd, lastRegion, TRUE);
                DeleteObject(lastRegion);
                regionDeleted = false;
            }
            return 0;

        case WM_RBUTTONUP:
            xMouse = GET_X_LPARAM(lParam);
            yMouse = GET_Y_LPARAM(lParam);
            strokeRGB = GetPixel(hdc, xStrokePreview + 20, yStrokePreview + 20);
            fillRGB = GetPixel(hdc, xFillPreview + 20, yFillPreview + 20);
            strokeWeight = getWeight(hwndStrokeWeight);
            point = drawingLimits(xMouse, yMouse, drawingArea, strokeWeight);
            xMouse = point.x;
            yMouse = point.y;

            if(thirdBezier) {
                pointsforBezier[3] = point;
                strokePen = CreatePen(PS_SOLID, strokeWeight, strokeRGB);
                SelectObject(hdc, strokePen);
                PolyBezier(hdc, pointsforBezier, 4);
                DeleteObject(strokePen);
                firstBezier  = false;
                secondBezier = false;
                thirdBezier  = false;
            }
            return 0;

        case WM_MOUSEMOVE:
            xMouse = GET_X_LPARAM(lParam);
            yMouse = GET_Y_LPARAM(lParam);

            if((xMouse > drawingArea.left)&&(xMouse < drawingArea.right)
                &&(yMouse > drawingArea.top)&&(yMouse < drawingArea.bottom)) {
                strokeRGB = GetPixel(hdc, xStrokePreview + 20, yStrokePreview + 20);
                fillRGB   = GetPixel(hdc, xStrokePreview + 20, yStrokePreview + 20);

                if((wParam == MK_LBUTTON)&&(Button_GetCheck(hwndPenTool) == BST_CHECKED)) {
                    strokePen = CreatePen(PS_SOLID, 1, strokeRGB);
                    SelectObject(hdc, strokePen);
                    MoveToEx(hdc, xMouse, yMouse, NULL);
                    LineTo(hdc, pen.x, pen.y);
                    DeleteObject(strokePen);
                    pen.x = xMouse;
                    pen.y = yMouse;
                }

                if((wParam == MK_LBUTTON)&&(Button_GetCheck(hwndEraserTool) == BST_CHECKED)) {
                    strokeWeight = getWeight(hwndEraserWeight);
                    point = drawingLimits(xMouse, yMouse, drawingArea, strokeWeight);
                    xMouse = point.x;
                    yMouse = point.y;
                    rect.left = point.x - (strokeWeight/2);
                    rect.right = point.x + (strokeWeight/2);
                    rect.top = point.y - (strokeWeight/2);
                    rect.bottom = point.y + (strokeWeight/2);
                    InvalidateRect(hwnd, &rect, FALSE);
                    SendMessage(hwnd, WM_PAINT, 0, 0);
                    ValidateRect(hwnd, &rect);
                }
            }
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            updateColorPreview(hdc, RGB(255, 255, 255), xFillPreview, yFillPreview);

            updateColorPreview(hdc, RGB(0, 0, 0), xStrokePreview, yStrokePreview);

            hdcMem = CreateCompatibleDC(hdc);
            SelectObject(hdcMem, hbmpimage);
            BitBlt(hdc, 600, 40, 280, 600, hdcMem, 0, 0, SRCCOPY);
            DeleteDC(hdcMem);

            rectTemp.top = rectRED.top;
            rectTemp.bottom = rectRED.bottom;
            for(int i = 0; i < (rectRED.right - rectRED.left); i++) {
                int r;
                r = i * 255 / (rectRED.right - rectRED.left);
                rectTemp.left  = rectRED.left  + i;
                rectTemp.right = rectRED.left + i + 1;
                hBrush = CreateSolidBrush(RGB(r, 0, 0));
                FillRect(hdc, &rectTemp, hBrush);
                DeleteObject(hBrush);
            }

            rectTemp.top = rectGREEN.top;
            rectTemp.bottom = rectGREEN.bottom;
            for(int i = 0; i < (rectGREEN.right - rectGREEN.left); i++) {
                int g;
                g = i * 255 / (rectGREEN.right - rectGREEN.left);
                rectTemp.left  = rectGREEN.left  + i;
                rectTemp.right = rectGREEN.left + i + 1;
                hBrush = CreateSolidBrush(RGB(0, g, 0));
                FillRect(hdc, &rectTemp, hBrush);
                DeleteObject(hBrush);
            }

            rectTemp.top = rectBLUE.top;
            rectTemp.bottom = rectBLUE.bottom;
            for(int i = 0; i < (rectBLUE.right - rectBLUE.left); i++) {
                int b;
                b = i * 255 / (rectBLUE.right - rectBLUE.left);
                rectTemp.left  = rectBLUE.left  + i;
                rectTemp.right = rectBLUE.left + i + 1;
                hBrush = CreateSolidBrush(RGB(0, 0, b));
                FillRect(hdc, &rectTemp, hBrush);
                DeleteObject(hBrush);
            }

            SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(0,0,0)));
            SelectObject(hdc, (HBRUSH)GetStockObject(WHITE_BRUSH));
            Rectangle(hdc, drawingArea.left, drawingArea.top, drawingArea.right, drawingArea.bottom);

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage (0);
            return 0;

        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

POINT drawingLimits(int xMouse, int yMouse, RECT limit, int stroke) {
    POINT result;
    stroke = stroke / 2 + 1;

    if(xMouse - stroke < limit.left) {
        result.x = limit.left + stroke;
    } else if(xMouse + stroke > limit.right) {
        result.x = limit.right - stroke;
    } else result.x = xMouse;

    if(yMouse - stroke < limit.top) {
        result.y = limit.top + stroke;
    } else if(yMouse + stroke > limit.bottom) {
        result.y = limit.bottom - stroke;
    } else result.y = yMouse;

    return result;
}

void updateColorPreview(HDC hdc, COLORREF rgb, int xLeft, int yTop) {
    HBRUSH hBrush = CreateSolidBrush(rgb);
    SelectObject(hdc, hBrush);
    Rectangle(hdc, xLeft, yTop, xLeft + 45, yTop + 45);
    DeleteObject(hBrush);
}

HRGN getLastEllipticRegion(RECT rect, int stroke, BOOL* flag) {
    HRGN result = CreateEllipticRgn(
        rect.left - (stroke / 2) - 1,
        rect.top - (stroke / 2) - 1,
        rect.right + (stroke / 2) + 1,
        rect.bottom + (stroke / 2) + 1);
    *flag = TRUE;
    return result;
}

HRGN getLastRectRegion(RECT rect, int stroke, BOOL* flag) {
    HRGN result = CreateRectRgn(
        rect.left - (stroke / 2) - 1,
        rect.top - (stroke / 2) - 1,
        rect.right + (stroke / 2) + 1,
        rect.bottom + (stroke / 2) + 1);
    *flag = TRUE;
    return result;
}

int getWeight(HWND input) {
    int result;
    int iLength = SendMessage(input, WM_GETTEXTLENGTH, 0, 0);
    char* szText = (char*)malloc(iLength+1);
    SendMessage(input, WM_GETTEXT, iLength+1, (LPARAM)szText);
    result = atoi(szText);
    _itoa(result, szText, 10);
    SendMessage(input, WM_SETTEXT, 0, (LPARAM)szText);
    free(szText);
    return result;
}


