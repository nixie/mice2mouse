#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include "mice2mouse.h"
#include <GL/glut.h>

int m2m_old_x[2], m2m_old_y[2];
int m2m_new_x[2], m2m_new_y[2];

void (*func_mouse)(int,int);
void (*func_motion)(int,int,int);

int fds[2];

const int BUF_SIZE=64;
struct input_event ev[BUF_SIZE];

// func will be called after some of mouse buttons are pressed
void m2m_mouseFunc(void (*func)(int btns, int state)){
    func_mouse = func;
}

// 3D coordinates has changed
void m2m_relMotionFunc(void (*func)(int dx, int dy, int dz)){
    func_motion = func;
}

// this must be registered in the main module of application with glutIdleFunc()
void m2m_workHorse(){
    int i,j, n, new_flg = 0;
    m2m_old_x[0] = m2m_new_x[0];
    m2m_old_x[1] = m2m_new_x[1];
    m2m_old_y[0] = m2m_new_y[0];
    m2m_old_y[1] = m2m_new_y[1];

    for (i=0; i < 2; i++){
        n = read(fds[i], (void*)(&ev), BUF_SIZE*sizeof(struct input_event));
        if (n < (int)sizeof(struct input_event)){
            if (n == -1 && errno == EAGAIN){
                ;
            } else if (n == -1) {
                perror("reading device:");
                exit(1);
            }
        } else {
            //fprintf(stderr, "Event here (%d) ..", n);
            // input event read
            new_flg = 1;
            for (j=0; j < n/sizeof(struct input_event); j++){
                if (ev[j].type == EV_REL){
                    //fprintf(stderr, "EV_REL\n");
                    switch (ev[j].code) {
                        case REL_X:
                            m2m_new_x[i] += ev[j].value;
                            break;
                        case REL_Y:
                            m2m_new_y[i] += ev[j].value;
                            break;
                        default:
                            //fprintf(stderr, "Unknown EV_REL event code\n");
                            ;
                    }
                } else if (ev[j].type == EV_ABS){
                    //fprintf(stderr, "EV_ABS\n");
                    switch(ev[j].code) {
                        case ABS_X:
                            m2m_new_x[i] = ev[j].value;
                            break;
                        case ABS_Y:
                            m2m_new_y[i] = ev[j].value;
                            break;
                        default:
                            //fprintf(stderr, "Unknown EV_ABS event code\n");
                            ;
                    }
                } else if (ev[j].type == EV_KEY){
                    fprintf(stderr, "Keypress/release, value=%d\n", ev[j].value);
                    if (ev[j].value == 0){
                        func_mouse(0, GLUT_UP);
                    }else if (ev[j].value == 1){
                        func_mouse(0, GLUT_DOWN);
                    }
                } else {
                    ;
                    //fprintf(stderr, "Unknown(%d)\n", ev.type);
                }
            }
        }
    }

    if (new_flg){
        // compare new_* and old_* and generate delta events
        int dy=0, dx0=0, dx1=0, dx=0, dz=0;

        // y and z are not shared by our 2 input devices
        dy = m2m_new_y[0] - m2m_old_y[0];
        dy *= -1;
        dz = m2m_new_y[1] - m2m_old_y[1];

        // x is computed by averaging common (mouse dependent) axes
        dx0 = m2m_new_x[0] - m2m_old_x[0];
        dx1 = m2m_new_x[1] - m2m_old_x[1];
        dx = (dx0 + dx1) / 2;

        if (dx || dy || dz){
            func_motion(dx,dy,dz);
        }
    }
}


void m2m_cleanup(){
    if (fds[0] > 2)
        close(fds[0]);
    if (fds[1] > 2)
        close(fds[1]);
}


int m2m_init(char *evdev1, char *evdev2){
    fds[0] = open(evdev1, O_RDONLY);
    if (fds[0] == -1){
        perror("opening first device: ");
        m2m_cleanup();
        return -1;
    }
    fds[1] = open(evdev2, O_RDONLY);
    if (fds[1] == -1){
        perror("opening second device: ");
        m2m_cleanup();
        return -1;
    }

    // set read filedescriptors to be non-blocking
    if (fcntl(fds[0], F_SETFL, O_NONBLOCK) == -1){
        perror("seting read to be nonblocking on first device: ");
        m2m_cleanup();
        return -1;
    }
    if (fcntl(fds[1], F_SETFL, O_NONBLOCK) == -1){
        perror("seting read to be nonblocking on second device: ");
        m2m_cleanup();
        return -1;
    }

    return 0;
}

