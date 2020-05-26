

#include "acquireGui.h"

acqGui::acqGui(){

    isGuiRunning = false;
    frequency = DEFAULT_FREQ;
    maxTime = DEFAULT_MAX_TIME;

    nbPlots_Ver = DEFAULT_NB_VER_PLOTS;
    nbPlots_Hor = DEFAULT_NB_HOR_PLOTS;
    terminateWindow = false;
    isFirstMsg = true;
    isRecording = false;
    icounter = 0;

}



LRESULT acqGui::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    XFORM xForm;
    RECT rect;
    TCHAR title[] = L"Channel ";
    TCHAR units[] = L"mV";
    DRAWITEMSTRUCT* pdis;

    int maxPlotLength = 0;
    int minPlotLength = 200;

    int minPlotHeight = 200;

    int rightPlane_l = 90;
    int bottomPlane_h = 20;

    int steps_on_x = 0;

    int steps_on_y = 0;

    int plt_counter = 0;

    POINT rPanelSPoint;
    

    if(terminateWindow){
        message = WM_DESTROY;
    }

    switch (message)
    {

    case WM_CREATE:
        onCreate(hWnd);
        break;
    
    case WM_DRAWITEM:
        pdis = (DRAWITEMSTRUCT*) lParam;
        switch(pdis->CtlID){
            case BNID_RECORD:
                createMyIcons(pdis);
                break;
            case BNID_MERGE:
                // std::cout << "merge clicked\n";
                break;
            case BNID_BACK:
                // std::cout << "back click\n";
                break;
            case BNID_PUBLISH:
                // std::cout << "publishing\n";
                break;
            default:
                break;
        }

    case WM_PAINT:
        if(isFirstMsg){
            SendMessageA(hWnd, WM_CREATE, wParam,lParam);
            isFirstMsg = false;
            break;
        }
        paintHandler(hWnd);
        break;

    case WM_COMMAND:
        switch(LOWORD(wParam)){
            case BNID_RECORD:
                if(!isRecording){
                    // isRecording = false;
                    eraseRecordRectangle(hWnd);
                    std::cout << "stop recording\n";
                }else{
                    // isRecording = true;
                    drawRecordRectangle(hWnd);
                    std::cout << "start recording\n";
                }
                break;
            case BNID_MERGE:
                std::cout << "merge clicked\n";
                break;
            case BNID_BACK:
                std::cout << "back click\n";
                break;
            case BNID_PUBLISH:
                std::cout << "publishing\n";
                break;
            default:
                break;
        }  
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int acqGui::makePlot(HDC hdc, int cornerLeft, int cornerBottom, int width, int height, std::vector<int> bkdColor, int pltNB, std::vector<double> data){

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

void acqGui::OnPaint(HDC hdc, int startingPoint_x, int startingPoint_y, int endingPoint_x, int endingPoint_y, int lineWidth, int alpha, std::vector<int> color)
{
    Gdiplus::Graphics graphics(hdc);
    Gdiplus::Pen      pen(Gdiplus::Color(alpha, color[0], color[1], color[2]), lineWidth);
    graphics.DrawLine(&pen, startingPoint_x, startingPoint_y, endingPoint_x, endingPoint_y);
}

void acqGui::paintBkd(HDC hdc, int rightCorner, int bottomCorner){

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


void acqGui::drawRecordRectangle(HWND hWnd){

    HDC hdc = GetDC(hWnd);
    RECT rect;
    int bottomPlane_h = 20;

    GetClientRect(hWnd, (LPRECT) &rect);

    int rightPlane_l = rect.right / 7;


    Gdiplus::Graphics graphics(hdc);
    Gdiplus::Pen redPen(Gdiplus::Color(150, 255, 0, 0), 8);

    graphics.DrawRectangle(&redPen, 0, 1, rect.right - rightPlane_l, rect.bottom - bottomPlane_h);
}


void acqGui::drawRecordRectangle(HDC hdc, int leftTop, int leftBottom, int width, int height){


    Gdiplus::Graphics graphics(hdc);
    Gdiplus::Pen redPen(Gdiplus::Color(150, 255, 0, 0), 8);

    graphics.DrawRectangle(&redPen, leftTop, leftBottom, width, height);
}


void acqGui::eraseRecordRectangle(HWND hWnd){
    HDC hdc = GetDC(hWnd);
    RECT rect;
    int bottomPlane_h = 20;

    GetClientRect(hWnd, (LPRECT) &rect);

    int rightPlane_l = rect.right / 7;

    Gdiplus::Graphics graphics(hdc);

    Gdiplus::LinearGradientBrush linGrBrush(
                                Gdiplus::Point(0, 0),
                                Gdiplus::Point(rect.right +  (rect.bottom * std::tan(26.5)), rect.bottom), 
                                Gdiplus::Color(255, 10, 53, 66),   
                                Gdiplus::Color(255, 21, 0, 42));  

    Gdiplus::REAL relativeIntensities[] = {0.0f, 0.0f, 1.0f, 1.0f};
    Gdiplus::REAL relativePositions[]   = {0.0f, 0.7f, 0.8f, 0.8f};

    linGrBrush.SetBlend(relativeIntensities, relativePositions, 3);

    Gdiplus::Pen mPen(&linGrBrush, 10);

    graphics.DrawRectangle(&mPen, 0, 1, rect.right - rightPlane_l, rect.bottom - bottomPlane_h -1);

}


int acqGui::onCreate(HWND hWnd){

    PAINTSTRUCT ps;
    HDC hdc;
    RECT rect;
    POINT rPanelSPoint;

    hdc = BeginPaint(hWnd, &ps);
    GetClientRect(hWnd, (LPRECT) &rect);
    DPtoLP(hdc, (LPPOINT) &rect, 2); 

    paintBkd(hdc, rect.right, rect.bottom);

    int rightPlane_l = rect.right / 7;

    
    rPanelSPoint.x = rect.right - rightPlane_l;
    rPanelSPoint.y = 40;
    createRightPanel(hWnd, rPanelSPoint);

    EndPaint(hWnd, &ps);
    return 0;
}


int acqGui::createMyIcons(DRAWITEMSTRUCT* pdis){

    RECT rect;

    // std::cout << "counter " << icounter << std::endl;
    
    HICON hCurrIcon;

    // Use copy of rectangle:
	rect = pdis->rcItem;

    if(icounter<1){

        icounter++;
        iconRecord_off_up = (HICON) LoadImageA(hInst, "icons\\record_icon_off_up.ico", IMAGE_ICON, 200, 100, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        // ihonRecord = (HICON) LoadIconA(hInst, "icons\\record_icon.ico");//, IMAGE_ICON, 205, 49, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        if (!iconRecord_off_up) {
			std::cout << "! iconRecord_off_up NULL (createMyIcons)" << std::endl;
			
		}

        iconRecord_off_up_hi = (HICON) LoadImageA(hInst, "icons\\record_icon_off_hl.ico", IMAGE_ICON, 200, 100, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        // ihonRecord = (HICON) LoadIconA(hInst, "icons\\record_icon.ico");//, IMAGE_ICON, 205, 49, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        if (!iconRecord_off_up_hi) {
			std::cout << "! iconRecord_off_up_hi NULL (createMyIcons)" << std::endl;
			
		}

        iconRecord_off_do = (HICON) LoadImageA(hInst, "icons\\record_icon_off_do.ico", IMAGE_ICON, 200, 100, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        // ihonRecord = (HICON) LoadIconA(hInst, "icons\\record_icon.ico");//, IMAGE_ICON, 205, 49, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        if (!iconRecord_off_do) {
			std::cout << "! iconRecord_off_do NULL (createMyIcons)" << std::endl;
			
		}

        iconRecord_on_up = (HICON) LoadImageA(hInst, "icons\\record_icon_on_up.ico", IMAGE_ICON, 200, 100, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        // ihonRecord = (HICON) LoadIconA(hInst, "icons\\record_icon.ico");//, IMAGE_ICON, 205, 49, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        if (!iconRecord_on_up) {
			std::cout << "! iconRecord_on_up NULL (createMyIcons)" << std::endl;
			
		}
        
        iconRecord_on_up_hi = (HICON) LoadImageA(hInst, "icons\\record_icon_on_up_hl.ico", IMAGE_ICON, 200, 100, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        // ihonRecord = (HICON) LoadIconA(hInst, "icons\\record_icon.ico");//, IMAGE_ICON, 205, 49, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        if (!iconRecord_on_up_hi) {
			std::cout << "! iconRecord_on_up_hi NULL (createMyIcons)" << std::endl;
			
		}
        
        iconRecord_on_do = (HICON) LoadImageA(hInst, "icons\\record_icon_on_do.ico", IMAGE_ICON, 200, 100, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        // ihonRecord = (HICON) LoadIconA(hInst, "icons\\record_icon.ico");//, IMAGE_ICON, 205, 49, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED);
        if (!iconRecord_on_do) {
			std::cout << "! iconRecord_on_do NULL (createMyIcons)" << std::endl;
			
		}
    }
    // std::cout << rect.right << ", " << rect.left << ", " << rect.bottom << ", " << rect.top << std::endl;

    // If the control's id is that of the "Up" button:
	if (pdis->CtlID == BNID_RECORD) {
		// If the button is selected, display the 
		// "active" icon, else the "inactive" icon:
        
		if (pdis->itemState & ODS_SELECTED){
            // hCurrIcon = iconRecord_off_do;
            if (isRecording){
                // std::cout << " ODS_SELECTED, isRecording " << isRecording << std::endl;
                hCurrIcon = iconRecord_on_do;
                isRecording = false;
            }else{
                // std::cout << " ODS_SELECTED, isRecording " << isRecording << std::endl;
                hCurrIcon = iconRecord_off_do;
                isRecording = true;
            }
        }
		else {
            // hCurrIcon = iconRecord_off_up;
            if (isRecording){
                // std::cout << " not ODS_SELECTED, isRecording " << isRecording << std::endl;
                hCurrIcon = iconRecord_on_up;
            }else{
                // std::cout << " not ODS_SELECTED, isRecording " << isRecording << std::endl;
                hCurrIcon = iconRecord_off_up;  
            }
        }
	}

// (int) 0.5 * (rect.right - rect.left - 205),
// 		(int) 0.5 * (rect.bottom - rect.top - 49),
    // Center the icon inside the control's rectangle:
	if (!DrawIconEx(
		pdis->hDC,
		rect.left,
        rect.top,
		(HICON) hCurrIcon,
		200,
		100,
		0, NULL, DI_NORMAL)) 
    {
		std::cout <<"! %ld from DrawIconEx (createMyIcons) " << GetLastError() << std::endl;;
	}


    return 0;
}


int acqGui::createRightPanel(HWND hWnd, POINT startPoint){

    // int rightPlaneOffset_x = 30;
    // int vertal_offset = 30;
    // int buttonLength = 100;
    // int buttonHeight = 50;
  
    recordButton = CreateWindowEx(WS_EX_LEFT,
                                        L"BUTTON",
                                        L"Record",
                                        WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, // BS_DEFPUSHBUTTON
                                        startPoint.x + rightPlaneOffset_x, startPoint.y , buttonLength, buttonHeight,
                                        hWnd,
                                        (HMENU) BNID_RECORD,
                                        hInst,
                                        NULL);

    CreateWindowEx(0,
                    L"BUTTON",
                    L"Merge",
                    WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    startPoint.x + rightPlaneOffset_x, startPoint.y + buttonHeight + vertal_offset, buttonLength, buttonHeight,
                    hWnd,
                    (HMENU) BNID_MERGE,
                    hInst,
                    NULL);
    
    CreateWindow(L"BUTTON",
                    L"Publish",
                    WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    startPoint.x + rightPlaneOffset_x, startPoint.y + 2*(buttonHeight + vertal_offset), buttonLength, buttonHeight,
                    hWnd,
                    (HMENU) BNID_PUBLISH,
                    hInst,
                    NULL);

    
    CreateWindow(L"BUTTON",
                    L"Back",
                    WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                    startPoint.x + rightPlaneOffset_x, startPoint.y + 3*(buttonHeight + vertal_offset), buttonLength, buttonHeight,
                    hWnd,
                    (HMENU) BNID_BACK,
                    hInst,
                    NULL);

    return 0;
}

int acqGui::paintHandler(HWND hWnd){

        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(hWnd, &ps);
        XFORM xForm;
        RECT rect;
        TCHAR title[] = L"Channel ";
        TCHAR units[] = L"mV";
        int bottomPlane_h = 20;
        int rightPlaneOffset_x = 30;

        // Here your application is laid out.
        // std::cout << "test2\n";
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

        int rightPlane_l = rect.right / 7;

        // move right pannel
        moveRightPanel(hWnd, rect.right - rightPlane_l + rightPlaneOffset_x, 40);

        // paint the backgound
        paintBkd(hdc, rect.right, rect.bottom);

        
        int steps_on_x = (rect.right - rightPlane_l) / nbPlots_Hor;
        int steps_on_y = (rect.bottom - bottomPlane_h) / nbPlots_Ver;

        int plt_counter = 0;
        for (int j=0; j<nbPlots_Ver; j++){
            for (int i=0; i<nbPlots_Hor; i++){
                if (dataBuffer.size()>0){
                    makePlot(hdc, rect.right - ((nbPlots_Hor-i)*steps_on_x + rightPlane_l), rect.bottom - ((nbPlots_Ver-j)*steps_on_y + bottomPlane_h), steps_on_x, steps_on_y, plotClrs[j % 2], plt_counter +1,  dataBuffer[plt_counter]);
                }else{
                    makePlot(hdc, rect.right - ((nbPlots_Hor-i)*steps_on_x + rightPlane_l), rect.bottom - ((nbPlots_Ver-j)*steps_on_y + bottomPlane_h), steps_on_x, steps_on_y, plotClrs[j % 2], plt_counter +1,  std::vector<double>(3,0));
                }
                plt_counter++;
            }
        }
        
        if(isRecording){
            drawRecordRectangle(hdc, 0, 0, rect.right - rightPlane_l, rect.bottom - bottomPlane_h);
        }

        // End application-specific layout section.

        EndPaint(hWnd, &ps);

    return 0;
}


int acqGui::moveRightPanel(HWND hWnd, int leftPos, int topPos){

    // int vertal_offset = 30;
    // int buttonLength = 100;
    // int buttonHeight = 50;

    // move record button
    HWND bhndr = GetDlgItem(hWnd, BNID_RECORD);
    SetWindowPos(bhndr, NULL, leftPos,  topPos, 3, 3, SWP_NOSIZE);
    
    // move merge button
    bhndr = GetDlgItem(hWnd, BNID_MERGE);
    SetWindowPos(bhndr, NULL, leftPos, topPos + buttonHeight + vertal_offset, 3, 3, SWP_NOSIZE);

    // move publish button
    bhndr = GetDlgItem(hWnd, BNID_PUBLISH);
    SetWindowPos(bhndr, NULL, leftPos, topPos + 2*(buttonHeight + vertal_offset), 3, 3, SWP_NOSIZE);

    // move back button
    bhndr = GetDlgItem(hWnd, BNID_BACK);
    SetWindowPos(bhndr, NULL, leftPos, topPos + 3*(buttonHeight + vertal_offset), 3, 3, SWP_NOSIZE);

    return 0;
}


int acqGui::init(){

    std::vector<int> clr1 = {255 , 10, 53, 66};
    std::vector<int> clr2 = {255 , 5, 46, 59};

    plotClrs.push_back(clr1);
    plotClrs.push_back(clr2);

    // FreeConsole();

    maxNbSamples = frequency * maxTime;

    rightPlaneOffset_x = 30;
    vertal_offset = 30;
    buttonLength = 200;
    buttonHeight = 100;

    // const wchar_t CLASS_NAME[]  = L"Sample Window Class";

    // const wchar_t szTitle[] = L"emgAcquire";



    return 0;
}

int acqGui::setPlotsNb(int new_nb_vert, int new_nb_hor){

    if(!isGuiRunning){
        nbPlots_Ver = new_nb_vert;
        nbPlots_Hor = new_nb_hor;
        return 0;
    }else{

        return -1;
    }
  
}


int acqGui::setFrequency(float nFreq){

    if(!isGuiRunning){
        frequency = nFreq;
        maxNbSamples = frequency * maxTime;
        return 0;
    }else{

        return -1;
    }
}


int acqGui::setMaxTime(float nTime){

    if(!isGuiRunning){
        maxTime = nTime;
        maxNbSamples = frequency * maxTime;
        return 0;
    }else{

        return -1;
    }
}


bool acqGui::guiStatus(){
    return isGuiRunning;
}


bool acqGui::recordStatus(){
    return isRecording;
}


int acqGui::guiHandler(){

    const wchar_t CLASS_NAME[]  = L"Sample Window Class";

    const wchar_t szTitle[] = L"Acquire";

    // Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    // ULONG_PTR gdiplusToken;

    // Initialize GDI+.
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // int nCmdShow;

    // LPSTR lpCmdLine;

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = &acqGui::staticWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    // wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); // IDI_APPLICATION
    wcex.hIcon = (HICON)LoadImageA(NULL, "icons\\acq_icon_large.ico", IMAGE_ICON, 256, 256, LR_LOADFROMFILE | LR_VGACOLOR | LR_SHARED); // IDI_APPLICATION
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //(COLOR_WINDOW+1); //CreateSolidBrush(0x000000ff);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = CLASS_NAME;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.cbWndExtra = sizeof(&wcex);

    if(!RegisterClassEx(&wcex)){
        MessageBox(NULL, L"Call to RegisterClassEx failed!", L"Windows desktop Guided Tour", NULL);
        return -1;
    }

    int desktopwidth=GetSystemMetrics(SM_CXSCREEN);
    int desktopheight=GetSystemMetrics(SM_CYSCREEN);

    hWnd = CreateWindowEx(0, //WS_EX_LEFT,  //, WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW
                        CLASS_NAME, 
                        szTitle, 
                        WS_OVERLAPPEDWINDOW | WS_EX_RIGHTSCROLLBAR,  // WS_TILEDWINDOW
                        desktopwidth / 4, desktopheight / 4, 800, nbPlots_Ver*300, 
                        NULL, NULL,
                        hInst, // hInst
                        NULL);
    
    if (!hWnd){
        MessageBox(NULL,
            L"Call to CreateWindow failed!",
            L"Windows Desktop Guided Tour",
            NULL);

        return -2;
    }

    SetWindowLongPtrW(hWnd, 0, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(hWnd, SW_RESTORE);

    

    UpdateWindow(hWnd);

    

    isGuiRunning = true;

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);

    isGuiRunning = false;

    std::cout << "window closed" << std::endl;
    return (int) msg.wParam;
}

LRESULT CALLBACK acqGui::staticWndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam )//HWND, UINT, WPARAM, LPARAM)
{

    // Retrieve instance pointer
    acqGui* pWnd = reinterpret_cast<acqGui*>(GetWindowLongPtrW(hwnd, 0));
    if ( pWnd != NULL ){  // See Note 1 below
        // Call member function if instance is available
        return pWnd->WndProc(hwnd, uMsg, wParam, lParam);
    }
    else{
        // Otherwise perform default message handling
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

}


int acqGui::launchGui(){

    guiThread = std::thread(&acqGui::guiHandler, this);
    std::cout << "Gui started" << std::endl;
    isGuiRunning = true;

    return 0;
}



int acqGui::updateData(std::vector< std::vector<double> > ddata){

    if(isGuiRunning){

        return 0;
    }else{
        return -1;
    }

}



int acqGui::shutdownGui(){

    if(isGuiRunning){
        terminateWindow = true;
        DestroyWindow(hWnd);
        if(guiThread.joinable()){
            std::cout << "Joining gui thread" << std::endl;
            guiThread.join();
        }

    }

    isRecording = false;

    std::cout << "Window shut down" << std::endl;

    return 0;
}


acqGui::~acqGui(){

    if(isGuiRunning){
        terminateWindow = true;
        DestroyWindow(hWnd);
        if(guiThread.joinable()){
            std::cout << "Joining gui thread" << std::endl;
            guiThread.join();
        }

    }

    for (int i=0; i< (int)dataBuffer.size(); i++){
        dataBuffer[i].clear();
    }
}
