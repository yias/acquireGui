
#ifndef ACQUIREGUI_H
#define ACQUIREGUI_H

#include <windows.h>
#include <tchar.h>
#include <Gdiplus.h>

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <thread>
#include <mutex>

const float DEFAULT_FREQ = 1000.0;  // Hz
const float DEFAULT_MAX_TIME = 30;  // seconds
const int DEFAULT_NB_VER_PLOTS = 1; // default number of plots vertically
const int DEFAULT_NB_HOR_PLOTS = 1; // default number of plots horizontally


enum{
    BNID_RECORD=200,
    BNID_BACK,
    BNID_MERGE,
    BNID_PUBLISH,
    WNID_RIGHTPANEL
};

class acqGui{

    HINSTANCE hInst;                                        // instance of the windows
    HWND hWnd;                                              // the main window
    HWND rightPanelWnd;                                     // the right panel window
    
    // button-related variables
    HWND recordButton;                                      // the window of the record button
    // icons for the record button
    HICON iconRecord_off_up, iconRecord_off_up_hi, iconRecord_off_do, iconRecord_on_up, iconRecord_on_up_hi, iconRecord_on_do;
    HWND mergeButton;                                       // the window of the merge button

    
    int rightPlaneOffset_x;                                 // horizontal offset from the plots
    int vertal_offset;                                      // vertical offset between buttons
    int buttonLength;                                       // button length
    int buttonHeight;                                       // button heigth

    // HBRUSH hbrGray;

    std::vector< std::vector<int> > plotClrs;               // background colors of the plots

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;       // graphics handler
    ULONG_PTR gdiplusToken;
    
    std::thread guiThread;
    std::mutex threadMutex;

    bool isGuiRunning;
    bool terminateWindow;
    bool isFirstMsg;
    bool isRecording;

    std::vector< std::vector<double> > dataBuffer;          // buffer of the data to plot
    int maxNbSamples;                                       // maximum number of samples to keep in the buffer
    float frequency;                                        // frequency of the acquired signals
    float maxTime;                                          // the maximum time appeared in the plots

    int nbPlots_Ver;                                        // number of plots vertically
    int nbPlots_Hor;                                        // number of plots horizontally

    // private functions
    LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
    static LRESULT CALLBACK staticWndProc(HWND, UINT, WPARAM, LPARAM);

    int makePlot(HDC hdc, int cornerRight, int cornerBottom, int width, int height, std::vector<int> bkdColor, int pltNB, std::vector<double> data);

    // plot line
    void OnPaint(HDC hdc, int startingPoint_x, int startingPoint_y, int endingPoint_x, int endingPoint_y, int lineWidth, int alpha, std::vector<int> color);

    // paint the background of a rectange
    void paintBkd(HDC hdc, int rightCorner, int bottomCorner);

    // draw and erase record rectangle
    void drawRecordRectangle(HDC hdc, int leftTop, int leftBottom, int width, int height);
    void drawRecordRectangle(HWND hWnd);
    void eraseRecordRectangle(HWND hWnd);


    // GUI handler
    int guiHandler();

    // create initial lay-out
    int onCreate(HWND hWnd);

    // create custom buttons
    int createMyIcons(DRAWITEMSTRUCT* pdis);
    int icounter;

    // handle changes on size
    int paintHandler(HWND hWnd);

    // create right panel
    int createRightPanel(HWND hWnd, POINT startPoint);

    // moving right panel
    int moveRightPanel(HWND hWnd, int leftPos, int topPos);

    

public:
    acqGui();
    ~acqGui();

    bool guiStatus();
    bool recordStatus();

    int init();

    int setPlotsNb(int nb_vert, int nb_hor);

    int setFrequency(float nFreq);

    int setMaxTime(float nTime);

    int launchGui();

    int updateData(std::vector< std::vector<double> > ddata);

    int shutdownGui();

    

};



#endif
