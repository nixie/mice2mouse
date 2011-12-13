#include "DemoHunt.h"
#include "debug.h"
#include <vector>
#include <cmath>
#include <cstring>
#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>


int dist3D(point3D_t &a, int x2, int y2, int z2){
    int dx, dy, dz;
    dx = a.x - x2;
    dy = a.y - y2;
    dz = a.z - z2;
    return (int)sqrt((dx*dx)+(dy*dy)+(dz*dz));
}

void HuntApp::renderApp_first(){

    snprintf(text_buf, TEXT_MAX, "time: %d, cnt: (%d,%d)",
                            secs, hlog.corrects(), hlog.incorrects());
    printStringUsingGlutBitmapFont(text_buf,
            GLUT_BITMAP_TIMES_ROMAN_24,          -60,  SIZE-10, SIZE, 1,0,0);

    if (chosen_btn == -1){
        printStringUsingGlutBitmapFont(
                "Please, press the button of your choice\n"
                "You wish to use to catch the candies ...\n",
                GLUT_BITMAP_TIMES_ROMAN_24, 0, SIZE+30, 0, 1,0,0);
    }


}

void HuntApp::appExit(){
    hlog.save();
}

char* HuntApp::appHelp(){
    return "Try to collect all the candies in a shortest time!";
}

void HuntApp::renderApp(){

    

    int nx,ny,nz;
    nx = x/(bs+1);
    ny = y/(bs+1);
    nz = z/(bs+1);
    
    nx *= bs;
    ny *= bs;
    nz *= bs;


    glPushMatrix();
    glTranslatef(nx+bs_half,ny+bs_half,nz+bs_half);

    glLineWidth(1);
    glColor3f(1,0,0);
    glutWireCube(bs-4);
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

                if (target[0] == xx && target[1] == yy && target[2] == zz){
                    glColor3f(0.1,1.0,0.5);
                    glLineWidth(1);
                    glutWireCube(bs-4);
                    glColor4f(0.2,0.2,0.2,0.3);
                } else{
                    glutWireCube(bs);
                }
                glPopMatrix();
            }
        }
    }
    glPopMatrix();


}

void HuntApp::appBtnUp(int button){
}
void HuntApp::appBtnDown(int button){
    if (chosen_btn == -1 && !(button == 2 || button == 5)){
        chosen_btn = button;
    }

    if (button == chosen_btn){
        int nx,ny,nz;
        nx = x/(bs+1);
        ny = y/(bs+1);
        nz = z/(bs+1);


        if (nx == target[0] && ny == target[1] && nz == target[2]){
            hlog.log(correct);
            newTarget();
        } else {
            hlog.log(incorrect);
        }
    }
}
void HuntApp::appMotion(int axis, int delta){ }
void HuntApp::appKeyDown(SDLKey sym){
}

void HuntApp::newTarget(){
    target[0] = rand() % n_blks;
    target[1] = rand() % n_blks;
    target[2] = rand() % n_blks;
}

void HuntApp::appTimer(){
    if (hlog.isRunning()){
        gettimeofday(&now, NULL);
        secs = now.tv_sec - tstart.tv_sec;
    } else {
        gettimeofday(&tstart, NULL);
    }
}

void HuntApp::appInit(){
    gettimeofday(&tstart, NULL);
    gettimeofday(&now, NULL);
    secs = 0;
    newTarget();
    chosen_btn=-1;
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    HuntApp theApp;
    return theApp.OnExecute();
}
