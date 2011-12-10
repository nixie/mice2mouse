#include "DemoDrawing.h"
#include <vector>
#include <cmath>
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

void DrawingApp::appBtnUp(int button){
    if (button == 3){
        // end this drawing segment
        drawing.push_back(new point3D_t(-1, -1, -1));
    }


}
void DrawingApp::appBtnDown(int button){ }
void DrawingApp::appMotion(int axis, int delta){
    if (buttons[3]){
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
    }
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    DrawingApp theApp;
    return theApp.OnExecute();
}
