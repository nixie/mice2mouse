#include <iostream>
#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include "BaseApp.h"
#include "debug.h"

using namespace std;


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

    return true;
}

void BaseApp::renderApp(){
}

unsigned int BaseApp::timer_callback(unsigned int interval, void *param){

    BaseApp *this_ptr = (BaseApp*) param;
    if (!this_ptr->paused){
        this_ptr->mytime += this_ptr->time_increment;
        this_ptr->rot_x = (sin(this_ptr->mytime) * this_ptr->displacement);
        this_ptr->rot_y = (cos(this_ptr->mytime) * this_ptr->displacement);
        this_ptr->rot_z = 0;
    }

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
        OnRender();
    }
 
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


    snprintf(coords, TEXT_MAX, "[%3d,%3d,%3d]", x, y, z);
    snprintf(params, TEXT_MAX, "time_inc:%g, ampl:%d",
                                time_increment, displacement);
    printStringUsingGlutBitmapFont(coords,
            GLUT_BITMAP_8_BY_13,          SIZE+60, -70, SIZE, 0.3,0.3,0.3);
    printStringUsingGlutBitmapFont(params,
            GLUT_BITMAP_8_BY_13,          SIZE+60, -80, SIZE, 0.3,0.3,0.3);

    glTranslatef(SIZE/2, SIZE/2, SIZE/2);
    glRotatef(rot_x, 1, 0, 0);
    glRotatef(rot_y, 0, 1, 0);
    glRotatef(rot_z, 0, 0, 1);
    glTranslatef(-SIZE/2, -SIZE/2, -SIZE/2);

    glScalef(1,1,1);
    renderCursor();
    renderApp();

    renderGrid();



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
    appBtnUp((int)button);

    if (button == 4){
        rot_x = displac_round(rot_x);
        rot_y = displac_round(rot_y);
        rot_z = displac_round(rot_z);
    }
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

    return ax;
}


void BaseApp::OnJoyAxis(Uint8 which, Uint8 axis, Sint16 value){
    // DEBUG(cerr << "axis:"<< (int)axis << " value:" << (int)value << endl);
    appMotion((int)axis, (int)value);

    int ax = axis;
    int delta = (int)value;

    if (paused){
        // transform axes only in paused mode (no need for translating
        // if we are looking just in front
        ax = axis_rot_transform((int)axis, &delta);
        // DEBUG(cerr << "motion in axis " << (int)axis << "->" << ax << endl);
    }

    if (axis == 1){
        delta *= -1;
    }



    if (buttons[4]){
        paused = true;
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

    switch(ax){
        case 0: x += delta;
                break;
        case 1: y += delta;
                break;
        case 2: z += delta;
                break;
    }

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

