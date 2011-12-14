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
#include "BaseApp.h"
#include "GL/glut.h"

class CursorDemo : public BaseApp {
    public:
        virtual char *appHelp(){
            static char text[] = 
                "Welcome to m2m introduction demo :)\n"
                "You can press down the mouse middle\n"
                "button and rotate with space beeing\n"
                "displayed ... Try it!\n\n"
                "After You are done with exploration,\n"
                "press Escape, next demo is waiting\n"
                "for You ...";
            return text;
        }

};

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    CursorDemo theApp;
    return theApp.OnExecute();
}
