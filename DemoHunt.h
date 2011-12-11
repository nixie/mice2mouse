#ifndef _DEMO_HUNT_H_
#define _DEMO_HUNT_H_

#include "BaseApp.h"
#include "CEvent.h"
#include "hunt_log.h"
#include <sys/time.h>

struct tick3D {
    int x, y, z;    // units are blocks
    int r, g, b;    // color of this tick
};
     
typedef enum occup {ANY=0, BRICK} occup_t;


class HuntApp : public BaseApp {
    public:
        static const int n_blks=6;
    private:
        int target[3];
        HuntLog hlog;
        struct timeval tstart, now;
        int secs;
        char text_buf[TEXT_MAX];


    public:
        static const int bs=SIZE/n_blks;
        static const int bs_half=bs/2;
        void newTarget();
        virtual void renderApp();
        virtual void renderApp_first();

        virtual void appInit();
        virtual void appExit();
        virtual void appTimer();
        virtual void appBtnUp(int button);
        virtual void appBtnDown(int button);
        virtual void appMotion(int axis, int delta);
        virtual void appKeyDown(SDLKey sym);
        
};

#endif
