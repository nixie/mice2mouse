#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <GL/glut.h>

#include "mice2mouse.h"

using namespace std;

void onDisplay(void);
void onKeyboard(unsigned char key, int x, int y);
void onResize(int w, int h);
void onMouse(int btn, int state, int x, int y);
void onMotion(int x, int y);

void renderCursor();

bool fullscreen, picked;
int x,y,z;
int cx, cy, cz;
int old_x, old_y;

// constant viewpoint moving
int displac_x, displac_y;
float mytime;
bool paused;
const int TIMER_INTERVAL=20;
const int DISPLACEMENT=120;

int size=300;
int bs=size/6;              // block/brick size
int bs_half=bs/2;
int lp = size-bs_half;  // last possible block coordinate

const int TEXT_MAX=100;
char text_buf[TEXT_MAX];

// window identifier
static int win;
int placed[100][3];
int n_placed=0;

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

void renderGrid(){
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
}

void onMouse(int btn, int state, int curr_x, int curr_y){
    if (glutGetModifiers() == GLUT_ACTIVE_CTRL){
        return;
    }
    if (btn == 4){
        y--;
    }else if(btn == 3){
        y++;
    }
    if (y > 100) y=100;
    if (y < 0) y=0;

    if (btn == 0 && state == GLUT_DOWN){
        std::cout << "put\n";
        if (picked){
            cx=x; cy=y; cz=z;
        }
        picked=!picked;

        int nx,ny,nz;
        nx = x/(bs+1);
        ny = y/(bs+1);
        nz = z/(bs+1);

        nx *= bs;
        ny *= bs;
        nz *= bs;

        placed[n_placed][0]=nx;
        placed[n_placed][1]=ny;
        placed[n_placed][2]=nz;
        n_placed++;
    }

    glutPostRedisplay();
}

void onMotion(int curr_x, int curr_y){

    int dx= curr_x-old_x;
    int dy= old_y-curr_y;

    if (glutGetModifiers() == GLUT_ACTIVE_CTRL){

        displac_x -=dx>>3;
        displac_y -=dy>>3;
    
        int limit = 360;
        if (displac_x > limit) displac_x = limit;
        if (displac_x < -limit) displac_x = -limit;
        if (displac_y > limit) displac_y = limit;
        if (displac_y < -limit) displac_y = -limit;


        glutPostRedisplay();
        return;

    }


    if (dx < 0){
        x += dx;
    }else{
        x += dx;
    }
    if (dy < 0){
        z -= dy;
    }else{
        z -= dy;
    }
    if (x > 100) x=100;
    if (x < 0) x=0; 
    if (z > 100) z=100;
    if (z < 0) z=0; 

    old_y=curr_y;
    old_x=curr_x;
    glutPostRedisplay();
}

void on3Dmotion(int dx, int dy, int dz){
    fprintf(stderr, "dx:%d, dy:%d, dz: %d\n", dx, dy, dz);
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
        if (picked){
            cx=x; cy=y; cz=z;
        }
        picked=!picked;

        int nx,ny,nz;
        nx = x/(bs+1);
        ny = y/(bs+1);
        nz = z/(bs+1);

        nx *= bs;
        ny *= bs;
        nz *= bs;

        placed[n_placed][0]=nx;
        placed[n_placed][1]=ny;
        placed[n_placed][2]=nz;
        n_placed++;

        glutPostRedisplay();
    }

}

int main(int argc, char **argv){

    if (argc != 3){
        fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
        return 1;
    }

    /*** GLUT ***/
    // initialize glut
    glutInit(&argc, argv);
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
    glutMainLoop();

    return 0;
}

void printStringUsingGlutBitmapFont(char *string, void *font,
        int x, int y, float r, float g, float b) {
    glColor3f(r, g, b);                 // nastaveni barvy vykreslovanych bitmap
    glRasterPos2i(x, y);                // nastaveni pozice pocatku bitmapy
    while (*string)                     // projit celym retezcem
        glutBitmapCharacter(font, *string++); // vykresleni jednoho znaku
}

void onDisplay(void){
    // do a clearscreen
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glLineWidth(2);
    glLoadIdentity();
    gluLookAt ( (size+displac_x)/2, (size+displac_y)/2, 800.0,
                size/2, size/2, 0.0,
                0.0, 1.0, 0.0);

    glScalef(1,1,1.5);
    renderCursor();

    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(size/2,size/2,size/2);
    glutWireCube(size);
    glPopMatrix();
    
    snprintf(text_buf, TEXT_MAX, "[%d,%d,%d]", x, y, z);
    /*
    if (picked){
        int nx,ny,nz;
        nx = (x+bs_half) > lp ? lp : x+bs_half;
        ny = (y+bs_half) > lp ? lp : y+bs_half;
        nz = (z+bs_half) > lp ? lp : z+bs_half;
        glTranslatef(nx, ny, nz);
        glColor3f(0.5,0.6,0.7);
    }else{
        glTranslatef(cx+bs_half,cy+bs_half,cz+bs_half);
        glColor3f(0.3,0.3,0.3);
    }*/

    int nx,ny,nz;
    nx = x/(bs+1);
    ny = y/(bs+1);
    nz = z/(bs+1);
    
    nx *= bs;
    ny *= bs;
    nz *= bs;


    int idx;
    for (idx=0; idx < n_placed; idx++){
        if (idx%2 == 0){
            glColor4f(1,0,0,0.4);
        }else{
            glColor4f(0,1,0,0.4);
        }

        glPushMatrix();
        glTranslatef(placed[idx][0]+bs_half,
                placed[idx][1]+bs_half,
                placed[idx][2]+bs_half);
        glutWireCube(bs-1);
        glPopMatrix();
        if (idx%2 == 0){
            glColor3f(0,1,0);
        }else{
            glColor3f(1,0,0);
        }


    }


    glPushMatrix();
    glTranslatef(nx+bs_half,ny+bs_half,nz+bs_half);

    glLineWidth(5);
    glutWireCube(bs-1);
    printStringUsingGlutBitmapFont(text_buf,
            GLUT_BITMAP_8_BY_13,          7, 7,0.3,0.3,0.3);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(bs_half, bs_half, bs_half);
    renderGrid();
    glPopMatrix();

    glutSwapBuffers();
}


void onKeyboard(unsigned char key, int x, int y){

    glutPostRedisplay();
    if(key == 'q' || key == 27){
        cout << "Got q,so quitting " << endl;
        glutDestroyWindow(win);
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
}
