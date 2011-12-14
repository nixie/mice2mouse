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
#ifndef _DEMO_DRAWING_H_
#define _DEMO_DRAWING_H_

#include "BaseApp.h"
#include "CEvent.h"

class DrawingApp : public BaseApp {
    private:
        std::vector<point3D_t*> drawing;
        point3D_t last_drawn;
        int painting_btn;
    public:
        virtual void renderApp();
        virtual void renderApp_first();
        virtual void appInit();

        virtual void appBtnUp(int button);
        virtual char* appHelp(void);
        virtual void appBtnDown(int button);
        virtual void appMotion(int axis, int delta);
        virtual void appKeyDown(SDLKey sym);
        
};

#endif
