#include <iostream>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include "CApp.h"
#include "debug.h"

using namespace std;


CApp::CApp() {
    Surf_Display = NULL;
    Running = true;
    Fullscreen = false;
    x=y=z=0;
    last_drawn.x = last_drawn.y = last_drawn.z = 0;
    rh_left_btn = 0;

    mytime = 0.0;
    displac_x = displac_y = 0;
    displacement = 5;
    time_increment = 0.05;
    paused = true;
}
 

void printStringUsingGlutBitmapFont(char *string, void *font,
        int x, int y, int z, float r, float g, float b) {
    glColor3f(r, g, b);                 // nastaveni barvy vykreslovanych bitmap
    glRasterPos3i(x, y, z);                // nastaveni pozice pocatku bitmapy
    while (*string)                     // projit celym retezcem
        glutBitmapCharacter(font, *string++); // vykresleni jednoho znaku
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


    timer_id = SDL_AddTimer(TIMER_INTERVAL, timer_callback, this);

    return true;
}

unsigned int CApp::timer_callback(unsigned int interval, void *param){

    CApp *this_ptr = (CApp*) param;
    if (!this_ptr->paused){
        this_ptr->mytime += this_ptr->time_increment;
        this_ptr->displac_x = (sin(this_ptr->mytime) * this_ptr->displacement);
        this_ptr->displac_y = (cos(this_ptr->mytime) * this_ptr->displacement);
        DEBUG(cerr << "dx/dy: " << this_ptr->displac_x << " " <<
                this_ptr->displac_y << "\n");
    }

    return interval;
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


    snprintf(coords, TEXT_MAX, "[%3d,%3d,%3d]", x, y, z);
    snprintf(params, TEXT_MAX, "time_inc:%g, ampl:%d",
                                time_increment, displacement);
    printStringUsingGlutBitmapFont(coords,
            GLUT_BITMAP_8_BY_13,          SIZE+60, -70, SIZE, 0.3,0.3,0.3);
    printStringUsingGlutBitmapFont(params,
            GLUT_BITMAP_8_BY_13,          SIZE+60, -80, SIZE, 0.3,0.3,0.3);

    glTranslatef(SIZE/2, SIZE/2, 0);
    glRotatef(displac_x, 1, 0, 0);
    glRotatef(displac_y, 0, 1, 0);
    glTranslatef(-SIZE/2, -SIZE/2, 0);

    glScalef(1,1,1.5);
    renderCursor();
    renderDrawing();

    renderGrid();



    glColor3f(1, 1, 1);
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

void CApp::renderDrawing(){
    glLineWidth(4);
    glBegin(GL_LINE_STRIP);
    {
        for(std::vector<point3D_t *>::iterator it = drawing.begin();
                it != drawing.end(); it++){
            point3D_t *ptr = *it;
            glVertex3f(ptr->x, ptr->y, ptr->z);
        }
    }
    glEnd();
    glLineWidth(1);
}


void CApp::renderGrid(){
    int bs = 30;
    int n_blks = SIZE/bs;
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


int dist3D(point3D_t &a, int x2, int y2, int z2){
    int dx, dy, dz;
    dx = a.x - x2;
    dy = a.y - y2;
    dz = a.z - z2;
    return (int)sqrt((dx*dx)+(dy*dy)+(dz*dz));
}


void CApp::OnJoyButtonUp(Uint8 which, Uint8 button){
    if (button == 3){
        rh_left_btn = 0;
    }
}

void CApp::OnJoyButtonDown(Uint8 which, Uint8 button){
    if (button == 3){
        rh_left_btn = 1;
    }
/*
    point3D_t *ptr = new point3D_t;
    ptr->x = x; ptr->y = y; ptr->z = z;
    drawing.push_back(ptr);*/
}

void CApp::OnJoyAxis(Uint8 which, Uint8 axis, Sint16 value){

    switch(axis){
        case 0: x += value;
                break;
        case 1: y += -value;
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

    if (rh_left_btn){
        // we are drawing
        if (dist3D(last_drawn, x, y, z) > 5){
            point3D_t *ptr = new point3D_t;
            last_drawn.x = x; last_drawn.y = y; last_drawn.z = z;
            ptr->x       = x; ptr->y       = y; ptr->z       = z;
            drawing.push_back(ptr);
        }
    }
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
        case SDLK_p:
            paused = !paused;
            break;
        case SDLK_c:
            for(std::vector<point3D_t *>::iterator it = drawing.begin();
                    it != drawing.end(); it++){
                delete *it;
            }
            drawing.clear();
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


int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    CApp theApp;
    return theApp.OnExecute();
}
