

#include "acquireGui.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <conio.h>


int main(){

    acqGui ag;

    ag.init();


    ag.launchGui();


    while(ag.guiStatus()){

        if(kbhit()){
            if(getch()=='q')
                break;
        }
        
    }


    ag.shutdownGui();

    return 0;
}