

#include <windows.h>
// #include <stdlib.h>
// #include <string.h>
#include <string>
#include <tchar.h>

#include <Gdiplus.h>

#include <iostream>
#include <cmath>
#include <vector>


// #pragma comment(lib,"User32.lib")
// #pragma comment(lib,"gdi32.lib")
// #pragma comment(linker, "/SUBSYSTEM:WINDOWS")

// #pragma comment (lib,"Gdiplus.lib")

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int makePlot(HDC hdc, int cornerRight, int cornerBottom, int width, int height, std::vector<int> bkdColor, int pltNB, std::vector<double> data);

VOID OnPaint(HDC hdc, int startingPoint_x, int startingPoint_y, int endingPoint_x, int endingPoint_y, int lineWidth, int alpha, std::vector<int> color);

void paintBkd(HDC hdc, int rightCorner, int bottomCorner);

HINSTANCE hInst;
HBRUSH hbrGray;

int nbPlots_Ver = 1;
int nbPlots_Hor = 1;

std::vector< std::vector<int> > plotClrs;

// int WINAPI main(_In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPSTR     lpCmdLine,
//    _In_ int       nCmdShow){
int main(){

    std::vector<int> clr1 = {255 , 10, 53, 66};
    std::vector<int> clr2 = {255 , 5, 46, 59};

    plotClrs.push_back(clr1);
    plotClrs.push_back(clr2);

    FreeConsole();

    const wchar_t CLASS_NAME[]  = L"Sample Window Class";

    const wchar_t szTitle[] = L"emgAcquire";

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    // Initialize GDI+.
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // int nCmdShow;

    // LPSTR lpCmdLine;

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = NULL;
    wcex.hIcon = LoadIcon(NULL, IDI_HAND); // IDI_APPLICATION
    // wcex.hIcon = (HICON)LoadImageA(NULL, "C:\\Users\\yias4\\Documents\\drawings\\acquire_gui\\ac_icon2.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED); // IDI_APPLICATION
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //(COLOR_WINDOW+1); //CreateSolidBrush(0x000000ff);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = CLASS_NAME;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if(!RegisterClassEx(&wcex)){
        MessageBox(NULL, L"Call to RegisterClassEx failed!", L"Windows desktop Guided Tour", NULL);
        return -1;
    }

    HWND hWnd = CreateWindowEx(WS_EX_LEFT, //WS_EX_LEFT,  //, WS_OVERLAPPEDWINDOW
                                CLASS_NAME, 
                                szTitle, 
                                WS_TILEDWINDOW,  // WS_TILEDWINDOW
                                300, 300, 800, 600, 
                                NULL, NULL,
                                NULL, // hInst
                                NULL);
    
    if (!hWnd){
        MessageBox(NULL,
            L"Call to CreateWindow failed!",
            L"Windows Desktop Guided Tour",
            NULL);

        return -2;
    }

    ShowWindow(hWnd, SW_RESTORE);

    UpdateWindow(hWnd);

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);
    return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    XFORM xForm;
    RECT rect;
    TCHAR title[] = L"Channel ";
    TCHAR units[] = L"mV";

    int maxPlotLength = 0;
    int minPlotLength = 200;

    int minPlotHeight = 200;

    int rightPlane_l = 90;
    int bottomPlane_h = 20;

    int steps_on_x = 0;

    int steps_on_y = 0;

    int plt_counter = 0;
    // HICON hIcon = LoadIcon(NULL, IDI_ASTERISK);
    HICON hIcon = (HICON)LoadImageA(NULL, "C:\\Users\\yias4\\Documents\\drawings\\acquire_gui\\acquire_icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE ); // IDI_APPLICATION
    // SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    std::vector<double> tmp={1,2,3,4};

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        
        // Here your application is laid out.

        // apply a scaling transformation
        xForm.eM11 = (FLOAT) 0.5; 
        xForm.eM12 = (FLOAT) 0.0; 
        xForm.eM21 = (FLOAT) 0.0; 
        xForm.eM22 = (FLOAT) 0.5; 
        xForm.eDx  = (FLOAT) 0.0; 
        xForm.eDy  = (FLOAT) 0.0;
        SetWorldTransform(hdc, &xForm);

        GetClientRect(hWnd, (LPRECT) &rect);
        DPtoLP(hdc, (LPPOINT) &rect, 2); 

        paintBkd(hdc, rect.right, rect.bottom);

        rightPlane_l = rect.right / 7;
        
        steps_on_x = (rect.right - rightPlane_l) / nbPlots_Hor;
        steps_on_y = (rect.bottom - bottomPlane_h) / nbPlots_Ver;


        for (int j=0; j<nbPlots_Ver; j++){
            for (int i=0; i<nbPlots_Hor; i++){
                makePlot(hdc, rect.right - ((nbPlots_Hor-i)*steps_on_x + rightPlane_l), rect.bottom - ((nbPlots_Ver-j)*steps_on_y + bottomPlane_h), steps_on_x, steps_on_y, plotClrs[j % 2], plt_counter +1,  tmp);
                plt_counter++;
            }
        }
        
        // End application-specific layout section.

        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}

int makePlot(HDC hdc, int cornerLeft, int cornerBottom, int width, int height, std::vector<int> bkdColor, int pltNB, std::vector<double> data){

    TCHAR plotTitle[] = L"Channel ";
    wcscat(plotTitle, std::to_wstring(pltNB).c_str());
    TCHAR plotUnits[] = L"mV";
    TCHAR plotZero[] = L"0";

    //    Initializing original object
    HGDIOBJ original_brush = NULL;
    HGDIOBJ original_pen = NULL;

    HFONT hFont,hTmp;

    //    Saving the original object
    original_brush = SelectObject(hdc,GetStockObject(DC_BRUSH));
    original_pen = SelectObject(hdc,GetStockObject(DC_PEN));

    // Rectangle(hdc, cornerLeft, cornerBottom, cornerLeft + 20, cornerBottom + 20);

    SelectObject(hdc, GetStockObject(DC_BRUSH));

    // set the brush color
    SetDCBrushColor(hdc, RGB(bkdColor[1], bkdColor[2], bkdColor[3]));

    SelectObject(hdc, GetStockObject(DC_PEN));

    int maxPlotLength = 0;
    int minPlotLength = 200;

    int minPlotHeight = 150;

    int boxLeftCornter_x = cornerLeft + 30;
    int boxLeftCornter_y = cornerBottom + 30;

    int boxWidth = ((width > minPlotLength) ? width : minPlotLength) - 40;
    int boxHeight = ((height > minPlotHeight) ? height : minPlotHeight) - 50;
    

    // SetDCPenColor(hdc, RGB(0,0,102)); //0,51,102 , 50,80,120
    // display title
    hFont = CreateFont(20,0,0,0, FW_SEMIBOLD, 0,0,0,0,0,0,2,0, L"SYSTEM_FIXED_FONT");
    hTmp = (HFONT)SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(205, 207, 208));
    TextOut(hdc, boxLeftCornter_x + (boxWidth / 2) - (_tcslen(plotTitle) * 10 * 0.5) , cornerBottom + 10 , plotTitle, _tcslen(plotTitle));
    DeleteObject(SelectObject(hdc,hTmp));


    // display data rectangle
    // SetDCBrushColor(hdc, RGB(51,92,100));
    Rectangle(hdc,  boxLeftCornter_x, 
                    boxLeftCornter_y,
                    boxLeftCornter_x + boxWidth, 
                    boxLeftCornter_y + boxHeight);
    
    // display units
    hFont = CreateFont(15,0,0,0, FW_SEMIBOLD, 0,0,0,0,0,0,2,0, L"ARIAL");
    hTmp = (HFONT)SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(80, 114, 130));
    TextOut(hdc, boxLeftCornter_x - (_tcslen(plotUnits)*12) , boxLeftCornter_y - 5, plotUnits, _tcslen(plotUnits));

    // display zero line unit
    TextOut(hdc, boxLeftCornter_x - 13, boxLeftCornter_y + (boxHeight / 2) - 8, plotZero, _tcslen(plotZero));

    // display zero line
    std::vector<int> h_line_clr = {80, 114, 130};
    OnPaint(hdc, boxLeftCornter_x, boxLeftCornter_y + (boxHeight / 2), boxLeftCornter_x + boxWidth - 1 , boxLeftCornter_y + (boxHeight / 2), 1, 100, h_line_clr);

    DeleteObject(SelectObject(hdc,hTmp));
    SelectObject(hdc,original_brush);
    SelectObject(hdc,original_pen);

    return 0;
}

VOID OnPaint(HDC hdc, int startingPoint_x, int startingPoint_y, int endingPoint_x, int endingPoint_y, int lineWidth, int alpha, std::vector<int> color)
{
    Gdiplus::Graphics graphics(hdc);
    Gdiplus::Pen      pen(Gdiplus::Color(alpha, color[0], color[1], color[2]), lineWidth);
    graphics.DrawLine(&pen, startingPoint_x, startingPoint_y, endingPoint_x, endingPoint_y);
}

void paintBkd(HDC hdc, int rightCorner, int bottomCorner){

    Gdiplus::Graphics graphics(hdc);

    Gdiplus::LinearGradientBrush linGrBrush(
    Gdiplus::Point(0, 0),
    Gdiplus::Point(rightCorner +  (bottomCorner * std::tan(26.5)), bottomCorner), // rightCorner+(bottomCorner/2), bottomCorner // * (1 + std::tan(26.5))
    Gdiplus::Color(255, 10, 53, 66),   // opaque blue
    Gdiplus::Color(255, 21, 0, 42));  // opaque green

    Gdiplus::REAL relativeIntensities[] = {0.0f, 0.0f, 1.0f, 1.0f};
    Gdiplus::REAL relativePositions[]   = {0.0f, 0.7f, 0.8f, 0.8f};

    linGrBrush.SetBlend(relativeIntensities, relativePositions, 3);


    // Gdiplus::Pen pen(&linGrBrush);

    graphics.FillRectangle(&linGrBrush, 0, 0, rightCorner, bottomCorner);



}