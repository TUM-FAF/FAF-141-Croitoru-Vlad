#include <iostream>
#include <windows.h>
#include <cmath>

#define ID_TIMER 001

using namespace std;

class animObj{

protected:
    COLORREF clr;
public:
    POINT center;
    int xSpeed,ySpeed;

    animObj(POINT center,const int &speed);
    bool Accelerate(const int& deltaSpeedX,const int& deltaSpeedY);
    bool Color(const COLORREF &clr);
    bool Collision(const RECT &rect);
    bool morph = false;
    virtual bool Move(const HDC &hdc,const RECT& rect,HBRUSH &hBrush)=0;
};

class Circle : public animObj {
public:
    Circle(POINT center,const int &speed): animObj(center,speed)
    {
        cout<<" + 1 ball ";
    }
    bool Move(const HDC &hdc,const RECT& rect,HBRUSH &hBrush);
};
    bool Interaction(animObj &alfa,animObj &beta);


LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

static animObj *obj[50];
char szClassName[ ] = "Lab4_WP";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;         // Catching double-clicks
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = CreateSolidBrush( RGB(255, 255, 255) );


    if (!RegisterClassEx (&wincl))
        return 0;

       hwnd = CreateWindowEx (
           0,
           szClassName,
           "Laboratory Work Nr.4",
           WS_OVERLAPPEDWINDOW,
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           800,
           600,
           HWND_DESKTOP,
           NULL,
           hThisInstance,
           NULL
           );

    ShowWindow (hwnd, nCmdShow);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC hdc;
    static HDC hdcMem;
    static PAINTSTRUCT ps;
    static RECT rect;
    static HBRUSH hBrush;
    static HBITMAP hndbmMem;
    static HANDLE hndOld;
    static int timerSpeed=5;
    static int nrObj=0;


    switch (message)
    {
          case WM_CREATE:

            hdc=GetDC(hwnd);
            GetClientRect(hwnd,&rect);
            hdcMem=CreateCompatibleDC(hdc);
            hndbmMem=CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
            hndOld = SelectObject(hdcMem,hndbmMem);
            SetTimer(hwnd,ID_TIMER,timerSpeed,NULL);
            break;


        case WM_SIZE:

            SelectObject(hdcMem,hndOld);
            DeleteObject(hndbmMem);
            DeleteDC(hdcMem);
            hdc=GetDC(hwnd);
            GetClientRect(hwnd,&rect);
            hdcMem=CreateCompatibleDC(hdc);
            hndbmMem=CreateCompatibleBitmap(hdc,rect.right,rect.bottom);
            hndOld = SelectObject(hdcMem,hndbmMem);
            break;


        case WM_LBUTTONDOWN:

            POINT newCenter;
            newCenter.x=LOWORD(lParam);
            newCenter.y=HIWORD(lParam);

            obj[nrObj]=new Circle(newCenter, 5 + newCenter.x % 5);
            obj[nrObj]->Color(RGB(newCenter.x % 255, newCenter.x % 125 + newCenter.y % 125, newCenter.y % 255));
            nrObj++;
            break;


        case WM_MOUSEWHEEL:
            if((short)HIWORD(wParam)<0) {
                timerSpeed += 5;
            }else {
                timerSpeed -= 5;
                if (timerSpeed < 0) timerSpeed = 1;
            }

            KillTimer(hwnd,ID_TIMER);
            SetTimer(hwnd,ID_TIMER,timerSpeed,NULL);
            break;


        case WM_PAINT:

            hdc=BeginPaint(hwnd,&ps);
            GetClientRect(hwnd,&rect);

            for(int i=0;i<nrObj-1;i++) {
                for(int j=i+1;j<nrObj;j++) {
                    Interaction(*obj[i],*obj[j]);
                }
            }

            FillRect(hdcMem,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));


            for(int i=0;i<nrObj;i++) {
                obj[i]->Move(hdcMem,rect,hBrush);
            }

            BitBlt(hdc,0,0,rect.right,rect.bottom,hdcMem,0,0,SRCCOPY);
            EndPaint(hwnd,&ps);
            break;


        case WM_TIMER:

            InvalidateRect(hwnd,NULL,FALSE);
            break;


        case WM_DESTROY:


            SelectObject(hdcMem,hndOld);
            DeleteObject(hndbmMem);
            DeleteDC(hdcMem);
            KillTimer(hwnd,ID_TIMER);

            PostQuitMessage (0);
            break;

        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


animObj::animObj(POINT center,const int &speed) {

    this->center=center;
    this->xSpeed=speed;
    this->ySpeed=speed;
    clr=RGB(0,0,0);

}

bool animObj::Accelerate(const int& deltaSpeedX,const int& deltaSpeedY) {
    xSpeed=deltaSpeedX;
    ySpeed=deltaSpeedY;
    return TRUE;
}

bool animObj::Color(const COLORREF &clr) {
    this->clr=clr;
    return TRUE;
}

bool Circle::Move(const HDC &hdc,const RECT& rect,HBRUSH &hBrush) {

    hBrush=CreateSolidBrush(clr);
    SelectObject(hdc,hBrush);
    Collision(rect);
    center.x-=xSpeed;
    center.y-=ySpeed;

    if(morph)
    {
        Rectangle(hdc, center.x - 15, center.y - 15, center.x + 15, center.y + 15 );
        SelectObject(hdc, GetStockObject(WHITE_BRUSH));
        DeleteObject(hBrush);
        return TRUE;
    }

    Ellipse(hdc,center.x - 15, center.y - 15, center.x + 15, center.y + 15);
    SelectObject(hdc,GetStockObject(WHITE_BRUSH));
    DeleteObject(hBrush);
    return TRUE;

}

bool animObj::Collision(const RECT &rect) {

    if(center.x - 20 > rect.right - 1)
        {
            xSpeed=abs(xSpeed);
            morph = true;     }

    if(center.x + 20 < rect.left + 1)
        {
            xSpeed=-abs(xSpeed);
            morph = true;    }

    if(center.y + 20 > rect.bottom - 1)
        {
            ySpeed=abs(ySpeed + 5);
                }

    if(center.y - 20 < rect.top + 1)
        {
            ySpeed=-abs(ySpeed + 5) ;
                }

    return TRUE;
}

bool Interaction(animObj &alfa,animObj &beta) {

    float distance;
    POINT totalSpeed;

    distance = sqrt( pow(alfa.center.x-beta.center.x,2)+ pow(alfa.center.y-beta.center.y,2) );


    if ( distance < 9 ) {
    totalSpeed.x=(abs(alfa.xSpeed)+abs(beta.xSpeed))/2;
    totalSpeed.y=(abs(alfa.ySpeed)+abs(beta.ySpeed))/2;

    alfa.Accelerate(-alfa.xSpeed*totalSpeed.x/abs(alfa.xSpeed),-alfa.ySpeed*totalSpeed.y/abs(alfa.ySpeed));
    beta.Accelerate(-beta.xSpeed*totalSpeed.x/abs(beta.xSpeed),-beta.ySpeed*totalSpeed.y/abs(beta.ySpeed));

    alfa.Color(RGB(totalSpeed.x * 50 % 255, totalSpeed.x * 50 % 255, totalSpeed.x * 50 % 255));
    }

    return TRUE;
}

