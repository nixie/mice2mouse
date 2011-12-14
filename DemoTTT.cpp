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
#include "DemoTTT.h"
#include "debug.h"
#include <vector>
#include <cmath>
#include <cstring>
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

char *TTTApp::appHelp(){
    static char text[] =
        "This is a Tic-Tac-Toe demo, explore 3D gaming now!\n"
        "By pressing key \"c\" you can clean the playground.\n";
    return text;
}

int dist3D(point3D_t &a, int x2, int y2, int z2){
    int dx, dy, dz;
    dx = a.x - x2;
    dy = a.y - y2;
    dz = a.z - z2;
    return (int)sqrt((dx*dx)+(dy*dy)+(dz*dz));
}


void set_plr_color(occup_t plr){
    switch(plr){
        case PLAYER1: glColor3f(1,1,0);
                      break;
        case PLAYER2: glColor3f(0,1,1);
                      break;
        case ANY:     glColor4f(0.2,0.2,0.2,0.3);
                      break;
    }
}

void TTTApp::renderApp(){

    int nx,ny,nz;
    nx = x/(bs+1);
    ny = y/(bs+1);
    nz = z/(bs+1);
    
    nx *= bs;
    ny *= bs;
    nz *= bs;


    glPushMatrix();
    glTranslatef(nx+bs_half,ny+bs_half,nz+bs_half);

    glLineWidth(5);
    set_plr_color(in_turn);
    glutWireCube(bs-1);

    glPopMatrix();




    glPushMatrix();
    glTranslatef(bs_half, bs_half, bs_half);

    glColor4f(0.2,0.2,0.2,0.3);
    glLineWidth(1);
    for (int xx=0; xx < n_blks; xx++){
        for (int yy=0; yy < n_blks; yy++){
            for (int zz=0; zz < n_blks; zz++){
                glPushMatrix();
                glTranslatef(xx*bs, yy*bs, zz*bs);
                occup_t plr = ttt_state[xx][yy][zz];
                glLineWidth(1);
                switch(plr){
                    case ANY:   glutWireCube(bs);
                                break;
                    case PLAYER1:
                                set_plr_color(plr);
                                glutWireCube(bs-3);
                                glColor4f(0.2,0.2,0.2,0.3);
                                break;
                    case PLAYER2:
                                set_plr_color(plr);
                                glutWireCube(bs-3);
                                glColor4f(0.2,0.2,0.2,0.3);
                                break;
                }
                glPopMatrix();
            }
        }
    }
    glPopMatrix();
}

void TTTApp::appBtnUp(int button){
}
void TTTApp::appBtnDown(int button){

    if (chosen_btn == -1 && !(button == 2 || button == 5)){
        chosen_btn = button;
        return;
    }

    if (button == chosen_btn){
        int nx,ny,nz;
        nx = x/(bs+1);
        ny = y/(bs+1);
        nz = z/(bs+1);


        occup_t *field = &ttt_state[nx][ny][nz];
        if (*field == ANY){
            *field = (occup_t) in_turn;
            in_turn = (in_turn == PLAYER1) ? PLAYER2 : PLAYER1;
        }
    }
}
void TTTApp::appMotion(int axis, int delta){ }
void TTTApp::appKeyDown(SDLKey sym){
    if (sym == SDLK_c){
        memset(&ttt_state, ANY, sizeof(ttt_state));
        chosen_btn = -1;
    }
}

void TTTApp::appInit(){
    memset(&ttt_state, ANY, sizeof(ttt_state));
    in_turn = PLAYER1;
    chosen_btn = -1;
}


void TTTApp::renderApp_first(){

    if (chosen_btn == -1){
        static char text[] = 
                "Please, press the button of your choice\n"
                "You wish to use for painting ...\n";
        printStringUsingGlutBitmapFont(
                text, GLUT_BITMAP_TIMES_ROMAN_24, 0, SIZE+30, 0, 1,0,0);
    }
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    TTTApp theApp;
    return theApp.OnExecute();
}
