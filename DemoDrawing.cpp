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
#include "DemoDrawing.h"
#include <vector>
#include <cmath>
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>


static char drawing_help[] = 
"For the first time, click with some mouse button\n"
"to define which button you will be using for painting.\n"
"Than - just paint ... ;)\n"
"\nUse \"c\" key to erase any drawing";

int dist3D(point3D_t &a, int x2, int y2, int z2){
    int dx, dy, dz;
    dx = a.x - x2;
    dy = a.y - y2;
    dz = a.z - z2;
    return (int)sqrt((dx*dx)+(dy*dy)+(dz*dz));
}

char* DrawingApp::appHelp(){
    return drawing_help;
}

void DrawingApp::renderApp_first(){
    if (painting_btn == -1){
        static char text[] = "Please, press the button of your choice\n"
                             "You wish to use for painting ...\n";
        printStringUsingGlutBitmapFont(
                text, GLUT_BITMAP_TIMES_ROMAN_24, 0, SIZE+30, 0, 1,0,0);
    }
}

void DrawingApp::renderApp(){
    glColor3f(0.4, 0.7, 0.7);
    glLineWidth(4);
    glBegin(GL_LINE_STRIP);
    {
        for(std::vector<point3D_t *>::iterator it = drawing.begin();
                it != drawing.end(); it++){
            point3D_t *ptr = *it;
            if (ptr->x >= 0){
                glVertex3f(ptr->x, ptr->y, ptr->z);
            }else{
                // this item tells us to begin new line segment
                glEnd();
                glBegin(GL_LINE_STRIP);
            }
        }
    }
    glEnd();
    glLineWidth(1);
}

void DrawingApp::appInit(){
    painting_btn = -1;
}

void DrawingApp::appBtnUp(int button){
    if (button == painting_btn){
        // end this drawing segment
        drawing.push_back(new point3D_t(-1, -1, -1));
    }
}

void DrawingApp::appBtnDown(int button){
    if (painting_btn == -1 && !(button == 2 || button ==5)){
        painting_btn = button;
    }
}
void DrawingApp::appMotion(int axis, int delta){
    if (painting_btn != -1 && buttons[painting_btn]){
        // we are drawing
        if (dist3D(last_drawn, x, y, z) > 5){
            point3D_t *ptr = new point3D_t(x, y, z);
            last_drawn.x = x; last_drawn.y = y; last_drawn.z = z;
            drawing.push_back(ptr);
        }
    }
}
void DrawingApp::appKeyDown(SDLKey sym){
    if (sym == SDLK_c){
        for(std::vector<point3D_t *>::iterator it = drawing.begin();
                it != drawing.end(); it++){
            delete *it;
        }
        drawing.clear();
        painting_btn = -1;
    }
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    DrawingApp theApp;
    return theApp.OnExecute();
}
