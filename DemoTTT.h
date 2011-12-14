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
#ifndef _DEMO_TTT_H_
#define _DEMO_TTT_H_

#include "BaseApp.h"
#include "CEvent.h"

typedef enum occup {ANY=0, PLAYER1, PLAYER2} occup_t;


struct tick3D {
    int x, y, z;    // units are blocks
    int r, g, b;    // color of this tick
};
        

class TTTApp : public BaseApp {
    public:
        static const int n_blks=6;
    private:
        occup_t in_turn;
        occup_t ttt_state[n_blks][n_blks][n_blks];
        int chosen_btn;

    public:
        static const int bs=SIZE/n_blks;
        static const int bs_half=bs/2;
        virtual void renderApp();
        virtual void renderApp_first();

        virtual void appInit();
        virtual char* appHelp();
        virtual void appBtnUp(int button);
        virtual void appBtnDown(int button);
        virtual void appMotion(int axis, int delta);
        virtual void appKeyDown(SDLKey sym);
        
};

#endif
