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
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include "BaseApp.h"
#include "debug.h"

#include "gl2ps-1.3.6/gl2ps.h"

using namespace std;

char help[] = 
"Useful keys:\n"
"Esc - quit application\n"
"  f - toggle fullscreen\n"
"  h - toggle this help\n"
"  l - switch mouse layout\n"
"  p - toggle automove mode (Play)\n"
"  c - center view\n"
"   Right  - increase autmove speed\n"
"   Left   - decrease autmove speed\n"
"   Up     - increase autmove amplitude\n"
"   Down   - decrease autmove amplitude\n";

char apphelp[] = "";
char *BaseApp::appHelp(){ return apphelp; }


BaseApp::BaseApp() {
    Surf_Display = NULL;
    Running = true;
    Fullscreen = false;
    x=y=z=0;

    for (int i=0; i<N_BTNS; i++){
        buttons[i] = false;
    }

    mytime = 0.0;
    rot_x = rot_y = rot_z = 0;
    spinrot_x = spinrot_y = spinrot_z = 0;
    displacement = 5;
    layout_xzxy = false;
    time_increment = 0.05;
    paused = false;
    showhelp = true;
    take_screenshot=false;

}
 

void printStringUsingGlutBitmapFont(char *string, void *font,
        int x, int y, int z, float r, float g, float b) {
    glColor3f(r, g, b);                 // nastaveni barvy vykreslovanych bitmap
    glRasterPos3i(x, y, z);                // nastaveni pozice pocatku bitmapy
    int line=0;
    while (*string){                     // projit celym retezcem
        if (*string == '\n'){
            line++;
            string++;
            if (font == GLUT_BITMAP_TIMES_ROMAN_24){
                glRasterPos3i(x, y-line*20, z);
            }else{
                glRasterPos3i(x, y-line*10, z);
            }
            continue;
        }
        glutBitmapCharacter(font, *string++); // vykresleni jednoho znaku
    }
}

bool BaseApp::OnInit() {
    // SDL initialization
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << SDL_GetError();
        return false;
    }
    const SDL_VideoInfo *desktop  = SDL_GetVideoInfo();
    desktop_w = desktop->current_w;
    desktop_h = desktop->current_h;
    std::cerr << "Resolution: " << desktop_w << "x" << desktop_h << std::endl;

    // Initialize mice2mouse input
    DEBUG(cerr << SDL_NumJoysticks() << " joysticks were found\n");
    int m2m_js=-1;
    for (int i=0; i < SDL_NumJoysticks(); i++) {
        joystick = SDL_JoystickOpen(i);
        if (string(SDL_JoystickName(i)).find("m2m") != string::npos){
            // this is joystick we need
            m2m_js = i;
        }
        SDL_JoystickClose(joystick);
    }

    if (m2m_js == -1){
        cerr << "No mice2mouse joystick found, exiting\n" ;
        SDL_Quit();
        return false;
    }else{
        cerr << "Using joystick \"" << SDL_JoystickName(m2m_js) << "\"\n";
        joystick = SDL_JoystickOpen(m2m_js);
    }


    // create and open openGL display surface
    if((Surf_Display = SDL_SetVideoMode(w, h, 0,
                        SDL_ANYFORMAT | SDL_HWSURFACE |
                        SDL_DOUBLEBUF | SDL_OPENGL)) == NULL) {
        cerr << SDL_GetError();
        return false;
    }

    // initialize openGL
    glClearColor(0.0,0.0,0.0,0.0);
    glPointSize(1.0);
    glLineWidth(1);
 
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

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45, (double)w/(double)h, 0, 1100.0);

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);
    glLoadIdentity();

    timer_id = SDL_AddTimer(TIMER_INTERVAL, timer_callback, this);

    appInit();

    return true;
}

void BaseApp::renderApp(){
}

unsigned int BaseApp::timer_callback(unsigned int interval, void *param){

    BaseApp *this_ptr = (BaseApp*) param;
    if (!this_ptr->paused){
        this_ptr->mytime += this_ptr->time_increment;
        this_ptr->spinrot_x = (sin(this_ptr->mytime) * this_ptr->displacement);
        this_ptr->spinrot_y = (cos(this_ptr->mytime) * this_ptr->displacement);
        this_ptr->spinrot_z = (cos(this_ptr->mytime) * this_ptr->displacement);
    }

    this_ptr->appTimer();

    return interval;
}

void BaseApp::OnEvent(SDL_Event* Event) {
    CEvent::OnEvent(Event);
}

void BaseApp::OnExit() {
    Running = false;
}

void BaseApp::OnCleanup() {
    SDL_Quit();
}

int BaseApp::OnExecute() {
    if(OnInit() == false) {
        return -1;
    }
 
    SDL_Event Event;
 
    while(Running) {
        while(SDL_PollEvent(&Event)) {
            OnEvent(&Event);
        }

        OnLoop();

        // GL2PS stuff with rendering inside
        if (take_screenshot){
            take_screenshot=false;
            FILE *fp = fopen("screenshot.pdf", "wb");
            GLint buffsize = 0, state = GL2PS_OVERFLOW;
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);

            while( state == GL2PS_OVERFLOW ){
                buffsize += 1024*1024;
                gl2psBeginPage ( "MyTitle", "MySoftware", viewport,
                        GL2PS_PDF, GL2PS_BSP_SORT, GL2PS_SILENT |
                        GL2PS_SIMPLE_LINE_OFFSET | GL2PS_DRAW_BACKGROUND |
                        GL2PS_OCCLUSION_CULL | GL2PS_BEST_ROOT | GL2PS_COMPRESS,
                        GL_RGBA, 0, NULL, 0, 0, 0, buffsize,
                        fp, "screenshot.pdf" );
                OnRender();
                state = gl2psEndPage();
            }
            fclose(fp);
        }else{
            OnRender();
        }
    }

    appExit();
    OnCleanup();
 
    return 0;
}

void BaseApp::OnLoop() {
}
void BaseApp::OnRender() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt ( SIZE/2, SIZE/2, 900.0,
                SIZE/2, SIZE/2, 0.0,
                0.0, 1.0, 0.0);

    
    if (showhelp){
        // display general help and also apṕlication specific help
        
        printStringUsingGlutBitmapFont(help, GLUT_BITMAP_9_BY_15,
                SIZE/2-100, SIZE-100, SIZE, 0, 1, 0);

        printStringUsingGlutBitmapFont(appHelp(), GLUT_BITMAP_9_BY_15,
                SIZE/2-100, SIZE/2-20, SIZE, 1, 1, 0);
    }


    renderApp_first();

    snprintf(coords, TEXT_MAX, "[%3d,%3d,%3d]", x, y, z);
    snprintf(params, TEXT_MAX, "speed:%g, ampl:%d",
                                time_increment, displacement);
    printStringUsingGlutBitmapFont(coords,
            GLUT_BITMAP_8_BY_13,          SIZE+5, -0, SIZE, 0.3,0.3,0.3);
    printStringUsingGlutBitmapFont(params,
            GLUT_BITMAP_8_BY_13,          SIZE+5, -10, SIZE, 0.3,0.3,0.3);

    glTranslatef(SIZE/2, SIZE/2, SIZE/2);
    glRotatef(spinrot_x, 1, 0, 0);
    glRotatef(spinrot_y, 0, 1, 0);
    glRotatef(spinrot_z, 0, 0, 1);
    glRotatef(rot_x, 1, 0, 0);
    glRotatef(rot_y, 0, 1, 0);
    glRotatef(rot_z, 0, 0, 1);
    glTranslatef(-SIZE/2, -SIZE/2, -SIZE/2);

    glScalef(1,1,1);
    renderCursor();

    renderGrid();

    renderApp();


    glColor3f(1, 1, 1);
    glTranslatef(SIZE/2, SIZE/2, SIZE/2);
    glutWireCube(SIZE);
    
 
    SDL_GL_SwapBuffers();
}

void BaseApp::renderCursor(){
    glBegin(GL_LINES);
    {
        glColor3f(0.0f, 0.0f, 1.0f);    // x-axis
        glVertex3f(0,  y,z);
        glVertex3f(SIZE,y,z);
        glColor3f(0.0f, 1.0f, 0.0f);    // y-axis
        glVertex3f(x,0,  z);
        glVertex3f(x,SIZE,z);
    }
    glEnd();

    // z-axis (divided into front and back part)
    glLineWidth(3);
    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_LINES); {
        glVertex3f(x,y,0);
        glVertex3f(x,y,z);
    }
    glEnd();

    glLineWidth(1);
    glColor3f(1.0f, 1.0f, 0.0f);

    glBegin(GL_LINES); {
        glVertex3f(x,y,z);
        glVertex3f(x,y,SIZE);
    }
    glEnd();

}


void BaseApp::renderGrid(){
    int n_blks = 6;
    int bs = SIZE/n_blks;
    glColor4f(0.3,0.3,0.3,0.5);
    glLineWidth(1);
    glBegin(GL_LINES); {
        for (int ii=0; ii < n_blks; ii++){
            // back
            glVertex3f(ii*bs, 0, 0);
            glVertex3f(ii*bs, SIZE, 0);
            glVertex3f(0, ii*bs, 0);
            glVertex3f(SIZE, ii*bs, 0);
            // left & right side
            glVertex3f(0, ii*bs, 0);
            glVertex3f(0, ii*bs, SIZE);
            glVertex3f(0, 0, ii*bs);
            glVertex3f(0, SIZE, ii*bs);
            glVertex3f(SIZE, ii*bs, 0);
            glVertex3f(SIZE, ii*bs, SIZE);
            glVertex3f(SIZE, 0, ii*bs);
            glVertex3f(SIZE, SIZE, ii*bs);
            // top & bottom size
            glVertex3f(ii*bs,   0, 0);
            glVertex3f(ii*bs,   0, SIZE);
            glVertex3f(0,       0, ii*bs);
            glVertex3f(SIZE,    0,  ii*bs);
            glVertex3f(ii*bs,   SIZE, 0);
            glVertex3f(ii*bs,   SIZE, SIZE);
            glVertex3f(0,       SIZE, ii*bs);
            glVertex3f(SIZE,    SIZE,  ii*bs);
        }
    }
    glEnd();
}


int displac_round(float d){
    if (d >= -45 && d < 45 ){
        return 0;
    } else if (d >= 45){
        return 90;
    }else{
        return -90;
    }
}


void BaseApp::OnJoyButtonUp(Uint8 which, Uint8 button){
    buttons[button] = false;
    showhelp = false;
    appBtnUp((int)button);
}

void BaseApp::OnJoyButtonDown(Uint8 which, Uint8 button){
    buttons[button] = true;
    appBtnDown((int)button);
}

const float pi = 3.14159265359;
const float deg2rad = 360.0/(2*pi);

// rotate vector $vector by multipling it with preinitialized rotation matrix
void rotate(float vector[3], float angle, int axis){
    float angle_rad = angle / deg2rad;
    float _sin = sinf(angle_rad);
    float _cos = cosf(angle_rad);
    float x  = vector[0], y=vector[1], z=vector[2];
     // DEBUG(cerr << "[" << x << " " << y << " " << z << "] rotated by "
     //       << angle << "(" << angle_rad << "rads):");
    switch(axis){
        case 0:
            vector[0] = x;
            vector[1] = y*_cos + z*_sin;
            vector[2] = z*_cos - y*_sin;
            break;
        case 1:
            vector[0] = x*_cos - z*_sin;
            vector[1] = y;
            vector[2] = x*_sin + z*_cos;
            break;
        case 2:
            vector[0] = x*_cos + y*_sin;
            vector[1] = y*_cos - x*_sin;
            vector[2] = z;
            break;
    }

     // DEBUG(cerr << "[" << vector[0] << " " << vector[1] << " " << vector[2]
     //       << "]\n");
}

int BaseApp::axis_rot_transform(int axis, int *value){
    // depending, from what place we are looking on the model, switch axes
    // from right
    
    float axis_vect[3] = {0, 0, 0};
    axis_vect[axis] = 1;
    rotate(axis_vect, rot_x, 0);
    rotate(axis_vect, rot_y, 1);
    rotate(axis_vect, rot_z, 2);

    // find out, to which axis the original x/y/z axis unit vector has
    // transformed
    int ax=0;
    float max=0;
    for (int i=0; i < 3; i++){
        if (fabsf(axis_vect[i]) > max){
            max = fabsf(axis_vect[i]);
            ax = i;
        }
    }

    if (axis_vect[ax] < 0){
        *value = -1 * (*value);
    }

    if (layout_xzxy && ax == 1){
        return 2;
    }else if (layout_xzxy && ax == 2){
        return 1;
    }else{
        return ax;
    }
}


void BaseApp::OnJoyAxis(Uint8 which, Uint8 axis, Sint16 value){
    // DEBUG(cerr << "axis:"<< (int)axis << " value:" << (int)value << endl);
    appMotion((int)axis, (int)value);

    int ax = axis;
    int delta = (int)value;

    //if (!(buttons[2] || buttons[5])){
        // transform axes only in paused mode (no need for translating
        // if we are looking just in front
        ax = axis_rot_transform((int)axis, &delta);
        // DEBUG(cerr << "motion in axis " << (int)axis << "->" << ax << endl);
    //}

    if (axis == 1){
        delta *= -1;
    }



    if (buttons[2] || buttons[5]){
        // paused = true;
        switch(axis){
            case 2: rot_x += value/3.0;
                     break;
            case 0: rot_y += value/3.0;
                     break;
            case 1: rot_z += value/3.0;
                    break;
        }

        const int max_displac = 90;
        rot_x = (rot_x > max_displac) ? max_displac : rot_x;
        rot_x = (rot_x < -max_displac) ? -max_displac : rot_x;
        rot_y = (rot_y > max_displac) ? max_displac : rot_y;
        rot_y = (rot_y < -max_displac) ? -max_displac : rot_y;
        rot_z = (rot_z > max_displac) ? max_displac : rot_z;
        rot_z = (rot_z < -max_displac) ? -max_displac : rot_z;
        return;
    }

    cerr << "axis " << (int)axis << " mapped to: " << ax << endl;

    switch(ax){
        case 0: x += delta;
                break;
        case 1: y += delta;
                break;
        case 2: z += delta;
                break;
    }
    /*
    }else{
        switch(ax){
            case 0: x += delta;
                    break;
            case 1: y += delta;
                    break;
            case 2: z += delta;
                    break;
        }
    }*/

    if (x > SIZE) x=SIZE;
    if (x < 0) x=0; 
    if (y > SIZE) y=SIZE;
    if (y < 0) y=0; 
    if (z > SIZE) z=SIZE;
    if (z < 0) z=0;

}


void BaseApp::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode){
    appKeyDown(sym);

    switch(sym){
        case SDLK_f:
            Fullscreen = !Fullscreen;
            SDL_FreeSurface(Surf_Display);
            if (Fullscreen){
                Surf_Display = SDL_SetVideoMode(
                        desktop_w,desktop_h,0,
                        SDL_DOUBLEBUF|SDL_FULLSCREEN|SDL_HWSURFACE|SDL_OPENGL);
                SDL_ShowCursor(SDL_DISABLE);
                glViewport(0, 0, desktop_w, desktop_h);
            } else {
                Surf_Display = SDL_SetVideoMode(w,h,0,
                        SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_OPENGL);
                SDL_ShowCursor(SDL_ENABLE);
                glViewport(0, 0, w, h);
            }

            if (Surf_Display == NULL){
                cerr << SDL_GetError();
                exit(1);
            }
            break;
        case SDLK_p:
            paused = !paused;
            break;
            
        case SDLK_c:
            // center view
            rot_x = displac_round(rot_x);
            rot_y = displac_round(rot_y);
            rot_z = displac_round(rot_z);
            spinrot_x = spinrot_y = spinrot_z = 0;
            break;
        case SDLK_h:
            showhelp = !showhelp;
            break;
        case SDLK_l:
            layout_xzxy = !layout_xzxy;
            break;
        case SDLK_s:
            take_screenshot = true;
            break;
        case SDLK_ESCAPE:
            Running = false;
            break;
        case SDLK_UP:
            displacement++;
            break;
        case SDLK_DOWN:
            displacement--;
            break;
        case SDLK_RIGHT:
            time_increment += 0.01;
            break;
        case SDLK_LEFT:
            time_increment -= 0.01;
            break;
        default:
            break;
    }
}

