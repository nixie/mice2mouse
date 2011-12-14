/*
   Copyright (C) 2011 Radek Fér <xferra00@stud.fit.vutbr.cz>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   in a file called COPYING along with this program; if not, write to
   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
   02139, USA.
*/
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
        int chosen_btn;


    public:
        static const int bs=SIZE/n_blks;
        static const int bs_half=bs/2;
        void newTarget();
        virtual void renderApp();
        virtual void renderApp_first();

        virtual void appInit();
        virtual char* appHelp();
        virtual void appExit();
        virtual void appTimer();
        virtual void appBtnUp(int button);
        virtual void appBtnDown(int button);
        virtual void appMotion(int axis, int delta);
        virtual void appKeyDown(SDLKey sym);
        
};

#endif
