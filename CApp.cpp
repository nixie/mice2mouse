#include <iostream>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "CApp.h"
#include "debug.h"
using namespace std;


CApp::CApp() {
    Surf_Display = NULL;
    Running = true;
    Fullscreen = false;
    x=y=z=0;
}
 

bool CApp::OnInit() {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        cerr << SDL_GetError();
        return false;
    }
    const SDL_VideoInfo *desktop  = SDL_GetVideoInfo();
    desktop_w = desktop->current_w;
    desktop_h = desktop->current_h;
    std::cerr << "W:" << desktop_w << "H:" << desktop_h << std::endl;

    if((Surf_Display = SDL_SetVideoMode(w, h, 0,
                        SDL_ANYFORMAT | SDL_HWSURFACE |
                        SDL_DOUBLEBUF | SDL_OPENGL)) == NULL) {
        cerr << SDL_GetError();
        return false;
    }

    int asda=1;
    //glutInit(&asda, NULL);

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

    gluPerspective(45, (double)w/(double)h, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);
    glLoadIdentity();

    
    DEBUG(cerr << SDL_NumJoysticks() << " joysticks were found:\n");

    int m2m_js=-1;
    for (int i=0; i < SDL_NumJoysticks(); i++) {
        joystick = SDL_JoystickOpen(i);
        DEBUG(cerr << SDL_JoystickName(i) <<
                "\taxes: " << SDL_JoystickNumAxes(joystick) <<
                "\tbtns: " << SDL_JoystickNumButtons(joystick) << endl);
        if (string(SDL_JoystickName(i)).find("m2m") != string::npos){
            // this is joystick we need
            m2m_js = i;
        }
        SDL_JoystickClose(joystick);
    }

    if (m2m_js == -1){
        cerr << "No mice2mouse joystick found ...\n" ;
        SDL_Quit();
        return false;
    }else{
        joystick = SDL_JoystickOpen(m2m_js);
    }


    return true;
}

void CApp::OnEvent(SDL_Event* Event) {
    CEvent::OnEvent(Event);
}

void CApp::OnExit() {
    Running = false;
}

void CApp::OnCleanup() {
    SDL_Quit();
}

int CApp::OnExecute() {
    if(OnInit() == false) {
        return -1;
    }
 
    SDL_Event Event;
 
    while(Running) {
        while(SDL_PollEvent(&Event)) {
            OnEvent(&Event);
        }
 
        OnLoop();
        OnRender();
    }
 
    OnCleanup();
 
    return 0;
}

void CApp::OnLoop() {
}
void CApp::OnRender() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt ( SIZE/2, SIZE/2, 900.0,
                SIZE/2, SIZE/2, 0.0,
                0.0, 1.0, 0.0);

    glScalef(1,1,1.5);
    renderCursor();

    glTranslatef(SIZE/2, SIZE/2, SIZE/2);
    glutWireCube(SIZE);
 
    SDL_GL_SwapBuffers();
}

void CApp::renderCursor(){
    glBegin(GL_LINES);
    {
        glColor3f(0.0f, 0.0f, 1.0f);    // x-axis
        glVertex3f(0,  y,z);
        glVertex3f(SIZE,y,z);
        glColor3f(0.0f, 1.0f, 0.0f);    // x-axis
        glVertex3f(x,0,  z);
        glVertex3f(x,SIZE,z);
        glColor3f(1.0f, 0.0f, 0.0f);    // z-axis
        glVertex3f(x,y,0);
        glVertex3f(x,y,SIZE);
    }
    glEnd();
}


/*
void CApp::renderGrid(){
    int n_cubes = size/bs;
    glColor4f(0.2,0.2,0.2,0.3);
    glLineWidth(1);
    for (int xx=0; xx < n_cubes; xx++){
        for (int yy=0; yy < n_cubes; yy++){
            for (int zz=0; zz < n_cubes; zz++){
                glPushMatrix();
                glTranslatef(xx*bs, yy*bs, zz*bs);
                glutWireCube(bs);
                glPopMatrix();
            }
        }
    }
}*/


void CApp::OnJoyButtonUp(Uint8 which, Uint8 button){
    DEBUG(cerr << "UP which:" << (int)which << " button :" << (int)button << endl);
}

void CApp::OnJoyAxis(Uint8 which, Uint8 axis, Sint16 value){
    DEBUG(cerr << "AX axis:" << (int)axis << " value :" << (int)value << endl);

    switch(axis){
        case 0: x += value;
                break;
        case 1: y += value;
                break;
        case 2: z += value;
                break;
    }

    if (x > SIZE) x=SIZE;
    if (x < 0) x=0; 
    if (y > SIZE) y=SIZE;
    if (y < 0) y=0; 
    if (z > SIZE) z=SIZE;
    if (z < 0) z=0;
    //glutPostRedisplay();

}


void CApp::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode){
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
        case SDLK_ESCAPE:
            Running = false;
            break;
        default:
            break;
    }
}


int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    CApp theApp;
    return theApp.OnExecute();
}
