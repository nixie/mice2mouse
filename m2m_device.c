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

// This program creates virtual device /dev/input/eventXY from 2 physical
// 2D pointers (events EV_REL or EV_ABS). This device offers EV_ABS type
// of events, but actually EV_REL events are generated -- it can be then
// easily used with GLUT or SDL event subsytem.
//
// Note, that uinput module must be loaded into kernel. Use evtest utility,
// to list available devices and to select the correct ('m2m') in user app.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <getopt.h>

#define ERR(ret) if (ret == -1) {\
                    fprintf(stderr, "At line %d :", __LINE__);\
                    perror(NULL); exit(EXIT_FAILURE); }

int fd_uinput;
int fd_mice[2];
int exit_flg=0;
int exit_ret=0;

#define BUF_SIZE 64
struct input_event ev[BUF_SIZE];

int m2m_old_x[2], m2m_old_y[2];
int m2m_new_x[2], m2m_new_y[2];

char uinput_path[] = "/dev/uinput";
char *mouseA_fname = NULL;
char *mouseB_fname = NULL;

// this can be altered at startup by commandline parameter -m (TODO)
// explanation:
//  From 2 mice, there are 4 independet axes to be merged into 3 axes and there
//  are many possible combinations, how to do that, so some mapping must exist
//  (and can be changed for everyone's taste).
//  Each input axes has own position in mapping[] array and value from this
//  position determines, to which output axis the relative movements should
//  be sent. Input mice axes placement in mapping is defined:
//      mapping[0]  ..  mouse A, x axis
//      mapping[1]  ..  mouse A, y axis
//      mapping[2]  ..  mouse B, x axis
//      mapping[3]  ..  mouse B, y axis
//
//  For example: mapping[3] == 'z' says: "relative movements from y axis
//  of mouse B translate to output movements in the axis z ..."
char mapping[] = "xyxz";    // mouse A controls xy plane and B controls xz plane

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
                // input event is there
                new_flg = 1;
                for (j=0; j < n/(int)sizeof(struct input_event); j++){
                    if (ev[j].type == EV_REL){
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
                                fprintf(stderr,"Unknown EV_REL event code\n");
                                ;
                        }
                    } else if (ev[j].type == EV_ABS){
                        switch(ev[j].code) {
                            case ABS_X:
                                m2m_new_x[i] = ev[j].value;
                                break;
                            case ABS_Y:
                                m2m_new_y[i] = ev[j].value;
                                break;
                            default:
                                fprintf(stderr,"Unknown EV_ABS event code\n");
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
            int input_delta[4];

            input_delta[0] = m2m_new_x[0] - m2m_old_x[0];
            input_delta[1] = m2m_new_y[0] - m2m_old_y[0];
            input_delta[2] = m2m_new_x[1] - m2m_old_x[1];
            input_delta[3] = m2m_new_y[1] - m2m_old_y[1];

            // if there are some changes (delta != 0), report them translated
            if (    input_delta[0] || input_delta[1] ||
                    input_delta[2] || input_delta[3]) {

                // translate movements from input axes to output axes
                int output_delta[3] = {0,0,0};
                for (int i=0; i<4; i++){
                    output_delta[mapping[i]-'x'] += input_delta[i];
                }

                m2m_send_rel(output_delta);
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


int m2m_mice_init(char *evdevA, char *evdevB){
    fd_mice[0] = open(evdevA, O_RDONLY);
    if (fd_mice[0] == -1){
        fprintf(stderr, "Error opening the A device %s:", evdevA);
        perror(NULL);
        m2m_cleanup();
        return -1;
    }
    fd_mice[1] = open(evdevB, O_RDONLY);
    if (fd_mice[1] == -1){
        fprintf(stderr, "Error opening the B device %s:", evdevA);
        perror(NULL);
        m2m_cleanup();
        return -1;
    }

    // set read filedescriptors to be non-blocking
    if (fcntl(fd_mice[0], F_SETFL, O_NONBLOCK) == -1){
        perror("seting read to be nonblocking on device A: ");
        m2m_cleanup();
        return -1;
    }
    if (fcntl(fd_mice[1], F_SETFL, O_NONBLOCK) == -1){
        perror("seting read to be nonblocking on device B: ");
        m2m_cleanup();
        return -1;
    }

    return 0;
}

int m2m_parse_cmdline(int argc, char *argv[]){
    int c;
    int m_flg=0;

    while ((c=getopt(argc, argv, "m:")) != -1){
        switch(c){
            case 'm':
                if (m_flg != 0){
                    fprintf(stderr, "Multiple mapping specification!\n");
                    return 1;
                }else{
                    m_flg = 1;

                    if (strlen(optarg) != 4){
                        fprintf(stderr,
                                "Mapping string must be of length 4!\n");
                        return 1;
                    }

                    // check mapping syntax
                    for (int i=0; i < 4; i++){
                        char c = optarg[i];
                        if (c >= 'x' && c <= 'z'){
                            continue;
                        }else{
                            fprintf(stderr, "Bad mapping specification!\n");
                            return 1;
                        }
                    }


                    // copy mapping to the mapping variable
                    memcpy(mapping, optarg, 4);
                }
                break;
            default:
                // getopt already printed error
                return 1;
        }
    }

    if (optind + 1 >= argc){
        fprintf(stderr, "Not enough parameters!\n");
        return 1;
    }

    mouseA_fname = (char*)argv[optind];
    mouseB_fname = argv[optind+1];

    return 0;
}


int main(int argc, char *argv[]){
    if (m2m_parse_cmdline(argc, argv) != 0){
        fprintf(stderr, "Usage: %s input_dev_A input_dev_B\n", argv[0]);
        return 1;
    }

    if (m2m_mice_init(mouseA_fname, mouseB_fname) != 0){
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
