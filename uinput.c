// This program creates virtual device /dev/input/eventXY from 2 physical
// 2D pointers (events EV_REL or EV_ABS). This device offers EV_ABS type
// of events, but actually it is EV_REL events generated -- it can be then
// easily used with GLUT or SDL event subsytem.
//
// Note, that uinput module must be loaded into kernel. Use evtest utility,
// to list available devices and to select the correct ('m2m') in user app.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <GL/glut.h>

#define ERR(ret) if (ret == -1) { perror(NULL); exit(EXIT_FAILURE); }

int fd_uinput;
int fd_mice[2];
int exit_flg=0;
int exit_ret=0;

#define BUF_SIZE 64
struct input_event ev[BUF_SIZE];

int m2m_old_x[2], m2m_old_y[2];
int m2m_new_x[2], m2m_new_y[2];

char uinput_path[] = "/dev/uinput";

void signal_handler(int __attribute__((__unused__))signo){
    exit_flg=1;
}

int m2m_uinput_init(){
    fd_uinput = open(uinput_path, O_WRONLY | O_NONBLOCK);
    if(fd_uinput < 0) {
        fprintf(stderr, "Cannot open %s!\n", uinput_path);
        exit(EXIT_FAILURE);
    }

    int ret;
    ret = ioctl(fd_uinput, UI_SET_EVBIT, EV_ABS); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_ABSBIT, ABS_X); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_ABSBIT, ABS_Y); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_ABSBIT, ABS_Z); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_EVBIT, EV_KEY); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_KEYBIT, BTN_0); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_KEYBIT, BTN_1); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_KEYBIT, BTN_2); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_KEYBIT, BTN_3); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_KEYBIT, BTN_4); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_KEYBIT, BTN_5); ERR(ret);
    ret = ioctl(fd_uinput, UI_SET_EVBIT, EV_SYN); ERR(ret);


    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));

    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "m2m");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0xfedc;
    uidev.id.version = 1;

    ret = write(fd_uinput, &uidev, sizeof(uidev));
    //fprintf(stderr, "size: %d, written: %d\n", sizeof(uidev), ret);

    ret = ioctl(fd_uinput, UI_DEV_CREATE);
    ERR(ret);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    return 0;
}

int m2m_send_rel(int delta[3]){

    struct input_event iev;
    memset(&iev, 0, sizeof(iev));
    iev.type = EV_ABS;
    gettimeofday(&iev.time, NULL);

    int i;
    for (i=0; i < 3; i++){
        iev.code = ABS_X + i;
        iev.value = delta[i];

        write(fd_uinput, &iev, sizeof(iev));
    }

    memset(&iev, 0, sizeof(iev));
    iev.type = EV_SYN;
    gettimeofday(&iev.time, NULL);
    iev.code = SYN_REPORT;
    iev.value = 0;
    write(fd_uinput, &iev, sizeof(iev));

    return 0;
}

int m2m_send_key(int code, int value){
    struct input_event iev;
    memset(&iev, 0, sizeof(iev));
    iev.type = EV_KEY;
    gettimeofday(&iev.time, NULL);

    iev.code = code;
    iev.value = value;

    write(fd_uinput, &iev, sizeof(iev));

    memset(&iev, 0, sizeof(iev));
    iev.type = EV_SYN;
    gettimeofday(&iev.time, NULL);
    iev.code = SYN_REPORT;
    iev.value = 0;
    write(fd_uinput, &iev, sizeof(iev));

    return 0;
}


int m2m_main_loop(){
    // wait for events from mice and then inject it merged into our m2m device
    fd_set readfds;

    while (1){
        FD_ZERO(&readfds);
        FD_SET(fd_mice[0], &readfds);
        FD_SET(fd_mice[1], &readfds);

        int i,j, n, new_flg = 0;
        m2m_old_x[0] = m2m_new_x[0];
        m2m_old_x[1] = m2m_new_x[1];
        m2m_old_y[0] = m2m_new_y[0];
        m2m_old_y[1] = m2m_new_y[1];

        // just block until some of file descriptor is ready
        n = select(fd_mice[1]+1, &readfds, NULL, NULL, NULL);
        if (n == -1){
            if (errno == EINTR && exit_flg){
                fprintf(stderr, "Signal received, exiting.\n");
                // exit with success, closing program intentionally isnt an
                // error
                exit_ret = EXIT_SUCCESS;
                break;
            }

            perror("select");
            exit_ret = EXIT_FAILURE;
            break;
        }


        for (i=0; i < 2; i++){
            n = read(fd_mice[i], (void*)(&ev),
                    BUF_SIZE*sizeof(struct input_event));
            if (n < (int)sizeof(struct input_event)){
                if (n == -1 && errno == EAGAIN){
                    ;
                } else if (n == -1) {
                    perror("reading device");
                    exit_ret = 1;
                    return 0;
                }
            } else {
                //fprintf(stderr, "Event here (%d) ..", n);
                // input event read
                new_flg = 1;
                for (j=0; j < n/(int)sizeof(struct input_event); j++){
                    if (ev[j].type == EV_REL){
                        //fprintf(stderr, "EV_REL(%d)\n", ev[j].code);
                        switch (ev[j].code) {
                            case REL_X:
                                m2m_new_x[i] += ev[j].value;
                                break;
                            case REL_Y:
                                m2m_new_y[i] += ev[j].value;
                                break;
                            case REL_WHEEL:
                                //fprintf(stderr,"wheel(%d)\n", ev[j].value);
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
                        //fprintf(stderr, "Keypress/release, code=%d, val=%d\n",
                                //ev[j].code, ev[j].value);
                        if (ev[j].code >= BTN_LEFT && ev[j].code <= BTN_MIDDLE){
                            
                            // translate code into 0..2 range
                            int newcode = ev[j].code - BTN_LEFT;
                            // translate it into 0..5 range
                            newcode += 3*i;
                            // add BTN_MISC offset
                            newcode += BTN_MISC;

                            m2m_send_key(newcode, ev[j].value);
                        }

                    } else {
                        //fprintf(stderr, "Event of type:%d\n", ev[j].type);
                        ;
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
    return exit_ret;
}
