/*
 * File:    hunt_log.h
 * Date:    29.11.2011 14:56
 * Author:  xferra00
 * Project: 
 * Description:
 *          
 */
#ifndef __HUNT_LOG_H__
#define __HUNT_LOG_H__

#include <vector>

enum logtype {incorrect, correct};

typedef struct log_item {
    enum logtype type;
    float time;
} logit_t;

class HuntLog {
    public:
        HuntLog();
        ~HuntLog();
        void log(enum logtype t);
        void save();
        bool isRunning() {return running;}
        int corrects() {return no_c;}
        int incorrects() {return no_i;}
    private:
        std::vector<logit_t*> items;
        struct timeval tstart;
        bool running;
        int no_c,no_i;    // number of correct and incorrect logs
};

#endif

