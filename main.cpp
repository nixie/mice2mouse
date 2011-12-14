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
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <sys/time.h>
#include <GL/glut.h>

#include "mice2mouse.h"
#include "hunt_log.h"

using namespace std;

// GLUT callbacks
void onDisplay(void);
void onKeyboard(unsigned char key, int x, int y);
void onResize(int w, int h);
//void onMouse(int btn, int state, int x, int y);
//void onMotion(int x, int y);

// not compiled display list for mouse cursor
void renderCursor();

enum game_mode {MODE_HUNT, MODE_TTT} mode = MODE_HUNT;
typedef enum occup {ANY=0, PLAYER1, PLAYER2} occup_t;

bool fullscreen;
int x,y,z;
int old_x, old_y;
occup_t in_turn;

HuntLog hlog;

// constant viewpoint moving
int displac_x, displac_y;
float mytime;
bool paused;
const int TIMER_INTERVAL=20;
const int DISPLACEMENT=80;

int size=300;
const int n_blks=6;
int bs=size/n_blks;              // block/brick size
int bs_half=bs/2;
int lp = size-bs_half;  // last possible block coordinate

const int TEXT_MAX=100;
char text_buf[TEXT_MAX];

// window identifier
static int win;

int target[3];

struct timeval tstart, now;
int secs;

void newTarget(){
    target[0] = rand() % n_blks;
    target[1] = rand() % n_blks;
    target[2] = rand() % n_blks;
}

struct tick3D {
    int x, y, z;    // units are blocks
    int r, g, b;    // color of this tick
};

occup_t ttt_state[n_blks][n_blks][n_blks];

//---------------------------------------------------------------------
// Nastaveni souradneho systemu v zavislosti na velikosti okna
//---------------------------------------------------------------------
void onResize(int w, int h)
{
    glViewport(0, 0, w, h);
    paused=true;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                       // nastaveni perspektivni projekce
    gluPerspective(45, (double)w/(double)h, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void onTimer(int value){
    if (hlog.isRunning()){
        gettimeofday(&now, NULL);
        secs = now.tv_sec - tstart.tv_sec;
        glutPostRedisplay();
    } else {
        gettimeofday(&tstart, NULL);
    }

    if (!paused) {
        mytime += 0.1;
        displac_x = (int)(sin(mytime) * DISPLACEMENT);
        displac_y = (int)(cos(mytime) *DISPLACEMENT);

        glutPostRedisplay();            // a prekreslit scenu
    }
    glutTimerFunc(TIMER_INTERVAL, onTimer, value); // znovu pustit casovac
}

void renderCursor(){
    glBegin(GL_LINES);
    {
        glColor3f(0.0f, 0.0f, 1.0f);    // x-axis
        glVertex3f(0,  y,z);
        glVertex3f(size,y,z);
        glColor3f(0.0f, 1.0f, 0.0f);    // x-axis
        glVertex3f(x,0,  z);
        glVertex3f(x,size,z);
        glColor3f(1.0f, 0.0f, 0.0f);    // z-axis
        glVertex3f(x,y,0);
        glVertex3f(x,y,size);
    }
    glEnd();
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


void renderGrid(){
    int n_cubes = size/bs;
    glColor4f(0.2,0.2,0.2,0.3);
    glLineWidth(1);
    for (int xx=0; xx < n_cubes; xx++){
        for (int yy=0; yy < n_cubes; yy++){
            for (int zz=0; zz < n_cubes; zz++){
                glPushMatrix();
                glTranslatef(xx*bs, yy*bs, zz*bs);
                if (mode == MODE_HUNT &&
                        target[0] == xx && target[1] == yy && target[2] == zz){
                    glColor3f(0.1,1.0,0.5);
                    glutWireCube(bs-1);
                    glColor4f(0.2,0.2,0.2,0.3);
                } else if (mode == MODE_TTT){
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
                } else {
                    glutWireCube(bs);
                }
                glPopMatrix();
            }
        }
    }
}

void on3Dmotion(int dx, int dy, int dz){
    //fprintf(stderr, "dx:%d, dy:%d, dz: %d\n", dx, dy, dz);
    x += dx; y += dy; z += dz;
    if (x > size) x=size;
    if (x < 0) x=0; 
    if (y > size) y=size;
    if (y < 0) y=0; 
    if (z > size) z=size;
    if (z < 0) z=0;
    glutPostRedisplay();
}

void on3Dmouse(int btn, int state){
    if (btn == 0 && state == GLUT_DOWN){
        std::cout << "put\n";

        int nx,ny,nz;
        nx = x/(bs+1);
        ny = y/(bs+1);
        nz = z/(bs+1);

        if (mode == MODE_HUNT){
            if (nx == target[0] && ny == target[1] && nz == target[2]){
                hlog.log(correct);
                newTarget();
                glutPostRedisplay();
                return;
            } else {
                hlog.log(incorrect);
                glutPostRedisplay();
            }
        }else if (mode == MODE_TTT){
            occup_t *field = &ttt_state[nx][ny][nz];
            if (*field == ANY){
                *field = (occup_t) in_turn;
                in_turn = (in_turn == PLAYER1) ? PLAYER2 : PLAYER1;
                glutPostRedisplay();
            }
        }

    }

}

int main(int argc, char **argv){
    gettimeofday(&tstart, NULL);
    gettimeofday(&now, NULL);
    secs = 0;

    glutInit(&argc, argv);
    if (argc != 3){
        fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
        return 1;
    }

    /*** GLUT ***/
    // initialize glut
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE|GLUT_DEPTH );
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);

    // create the window, set the title and keep the 
    // window identifier.
    win = glutCreateWindow("ITU 11/12 3Dpointer demo");

    // callback registration
    glutDisplayFunc(onDisplay);
    glutKeyboardFunc(onKeyboard);
    glutReshapeFunc(onResize);
    //glutMouseFunc(onMouse);
    //glutPassiveMotionFunc(onMotion);
    glutTimerFunc(TIMER_INTERVAL, onTimer,
            1234);  // registrace funkce volane pri jednom tiku casovace


    if (m2m_init(argv[1], argv[2]) != 0){
        fprintf(stderr, "Cannot initialize m2m\n");
        return 1;
    }
    m2m_relMotionFunc(on3Dmotion);
    m2m_mouseFunc(on3Dmouse);
    glutIdleFunc(m2m_workHorse);

    /*** OPENGL ***/
    // define the color we use to clearscreen 
    glClearColor(0.0,0.0,0.0,0.0);
    glPointSize(1.0);
    glLineWidth(1);

    //glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glDepthFunc(GL_LESS);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_POINT_SMOOTH);  // point antialiasing 
    glEnable(GL_LINE_SMOOTH);   // line antialiasing
    glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    // enter the main loop
    
    // generate 1st random sample
    newTarget();
    in_turn = PLAYER1;

    glutMainLoop();

    return 0;
}


void printStringUsingGlutBitmapFont(char *string, void *font,
        int x, int y, int z, float r, float g, float b) {
    glColor3f(r, g, b);                 // nastaveni barvy vykreslovanych bitmap
    glRasterPos3i(x, y, z);                // nastaveni pozice pocatku bitmapy
    while (*string)                     // projit celym retezcem
        glutBitmapCharacter(font, *string++); // vykresleni jednoho znaku
}

void onDisplay(void){
    // do a clearscreen
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLineWidth(2);
    glLoadIdentity();


    glutWireCube(bs);

    gluLookAt ( (size/2+displac_x), (size/2+displac_y), 800.0,
                size/2, size/2, 0.0,
                0.0, 1.0, 0.0);


    glScalef(1,1,1.5);
    renderCursor();

    snprintf(text_buf, TEXT_MAX, "time: %d, cnt: (%d,%d)",
                            secs, hlog.corrects(), hlog.incorrects());
    printStringUsingGlutBitmapFont(text_buf,
            GLUT_BITMAP_8_BY_13,          7, size + 7, 0, 1,0,0);

    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(size/2,size/2,size/2);
    glutWireCube(size);
    glPopMatrix();
    
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

    snprintf(text_buf, TEXT_MAX, "[%d,%d,%d]", x, y, z);
    printStringUsingGlutBitmapFont(text_buf,
            GLUT_BITMAP_8_BY_13,          7, 7, 0, 0.3,0.3,0.3);

    glPopMatrix();


    glPushMatrix();
    glTranslatef(bs_half, bs_half, bs_half);
    renderGrid();
    glPopMatrix();

    glutSwapBuffers();
}


void onKeyboard(unsigned char key,
        __attribute__((__unused__)) int x,
        __attribute__((__unused__)) int y){

    glutPostRedisplay();
    if(key == 'q' || key == 27){
        glutDestroyWindow(win);
        hlog.save();
        exit(0);
    }
    if (key == 'f'){
        if (fullscreen){
            glutReshapeWindow(500, 500); // windowize
            glutPositionWindow(50, 50);
            fullscreen=false;
        }else{
            glutFullScreen();
            glutSetCursor( GLUT_CURSOR_NONE);
            fullscreen=true;
        }
    }
    if (key == 'p'){
        paused=!paused;
    }
    if (key == 'm'){
        mode = (mode == MODE_HUNT) ? MODE_TTT : MODE_HUNT;
        glutPostRedisplay();
    }
}
