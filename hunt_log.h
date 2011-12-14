/*
 * Date:    29.11.2011 14:56
 * Author:  xferra00
 */

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

