#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <GL/glut.h>


// TODO:
//  * select()
//  * destroy m2m device on SIGTERM/SIGINT

#define ERR(ret) if (ret == -1) { perror(NULL); exit(EXIT_FAILURE); }

int fd_uinput;
int fd_mice[2];

#define BUF_SIZE 64
struct input_event ev[BUF_SIZE];

int m2m_old_x[2], m2m_old_y[2];
int m2m_new_x[2], m2m_new_y[2];

char uinput_path[] = "/dev/uinput";


int m2m_uinput_init(){
    fd_uinput = open(uinput_path, O_WRONLY | O_NONBLOCK);
    if(fd_uinput < 0) {
        fprintf(stderr, "Cannot open %s!\n", uinput_path);
        exit(EXIT_FAILURE);
    }

    int ret;
    ret = ioctl(fd_uinput, UI_SET_EVBIT, EV_REL); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_EVBIT, EV_SYN); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_RELBIT, REL_X); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_RELBIT, REL_Y); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_RELBIT, REL_Z); ERR(ret);


    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "Mice2Mouse virtual device");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0xfedc;
    uidev.id.version = 1;

    ret = write(fd_uinput, &uidev, sizeof(uidev));
    fprintf(stderr, "size: %d, written: %d\n", sizeof(uidev), ret);

    ret = ioctl(fd_uinput, UI_DEV_CREATE);
    ERR(ret);

    return 0;
}

int m2m_send_rel(int delta[3]){

    struct input_event iev;
    memset(&iev, 0, sizeof(iev));
    iev.type = EV_REL;
    gettimeofday(&iev.time, NULL);

    int i, ret;
    for (i=0; i < 3; i++){
        iev.code = REL_X + i;
        iev.value = delta[i];

        ret = write(fd_uinput, &iev, sizeof(iev));
        fprintf(stderr, "2size: %d, written: %d\n", sizeof(iev), ret);
    }

    memset(&iev, 0, sizeof(iev));
    iev.type = EV_SYN;
    gettimeofday(&iev.time, NULL);
    iev.code = SYN_REPORT;
    iev.value = 0;
    ret = write(fd_uinput, &iev, sizeof(iev));
    fprintf(stderr, "2size: %d, written: %d\n", sizeof(iev), ret);

    return 0;
}



int m2m_main_loop(){
    // wait for events from mice and then inject it merged into our m2m device


    while (1){
        // TODO use select()!
        int i,j, n, new_flg = 0;
        m2m_old_x[0] = m2m_new_x[0];
        m2m_old_x[1] = m2m_new_x[1];
        m2m_old_y[0] = m2m_new_y[0];
        m2m_old_y[1] = m2m_new_y[1];
        for (i=0; i < 2; i++){
            n = read(fd_mice[i], (void*)(&ev),
                    BUF_SIZE*sizeof(struct input_event));
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
                for (j=0; j < n/(int)sizeof(struct input_event); j++){
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
                                //fprintf(stderr,"Unknown EV_REL event code\n");
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
                                //fprintf(stderr,"Unknown EV_ABS event code\n");
                                ;
                        }
                    } else if (ev[j].type == EV_KEY){
                        fprintf(stderr, "Keypress/release, value=%d\n",
                                ev[j].value);
                        if (ev[j].value == 0){
                            ;
                            //func_mouse(0, GLUT_UP);
                        }else if (ev[j].value == 1){
                            ;
                            //func_mouse(0, GLUT_DOWN);
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
                int delta[3] = {dx, dy, dz};
                m2m_send_rel(delta);
            }
        }
    }

    return 0;
}





void m2m_cleanup(){
    if (fd_mice[0] > 2)
        close(fd_mice[0]);
    if (fd_mice[1] > 2)
        close(fd_mice[1]);
}


int m2m_mice_init(char *evdev1, char *evdev2){
    fd_mice[0] = open(evdev1, O_RDONLY);
    if (fd_mice[0] == -1){
        perror("opening first device: ");
        m2m_cleanup();
        return -1;
    }
    fd_mice[1] = open(evdev2, O_RDONLY);
    if (fd_mice[1] == -1){
        perror("opening second device: ");
        m2m_cleanup();
        return -1;
    }

    // set read filedescriptors to be non-blocking
    if (fcntl(fd_mice[0], F_SETFL, O_NONBLOCK) == -1){
        perror("seting read to be nonblocking on first device: ");
        m2m_cleanup();
        return -1;
    }
    if (fcntl(fd_mice[1], F_SETFL, O_NONBLOCK) == -1){
        perror("seting read to be nonblocking on second device: ");
        m2m_cleanup();
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[]){
    if (argc != 3){
        fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
        return 1;
    }

    if (m2m_mice_init(argv[1], argv[2]) != 0){
        return 1;
    }

    if (m2m_uinput_init() != 0){
        return 1;
    }

    m2m_main_loop();
    m2m_cleanup();
    ioctl(fd_uinput, UI_DEV_DESTROY);
    return 0;
}
