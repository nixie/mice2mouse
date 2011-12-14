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
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include <cstring>
#include "hunt_log.h"

HuntLog::HuntLog(){
    running = false;
    no_c = 0;
    no_i = 0;
}

HuntLog::~HuntLog(){
    std::vector<logit_t*>::iterator it;
    for (it = items.begin(); it != items.end(); it++){
        delete (*it);
    }
}


float timeval_diff(struct timeval &t1, struct timeval &t2){
    return (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec)*0.000001;
}

void HuntLog::log(enum logtype t){
    if (!running){
        if (t == correct){
            running = true;
            gettimeofday(&tstart, NULL);
        }else{
            return;
        }
    }

    if (t==correct){
        no_c++;
    }else{
        no_i++;
    }

    logit_t* item = new logit_t;
    item->type = t;
    struct timeval tmp;
    gettimeofday(&tmp, NULL);
    item->time = timeval_diff(tstart, tmp);
    items.push_back(item);
}


void HuntLog::save(){
    if (items.size() < 2){
        return;
    }

    // logs/2011-12-03_12:34:11.log
    char filename[30];
    memset(filename, 0x00, 30);
    strftime(filename, 25, "logs/%F_%H:%M:%S", localtime(&tstart.tv_sec));
    strncpy(filename+strlen(filename), ".log", 5);
    std::cerr << "Saving to file " << filename << std::endl;

    std::fstream fstr(filename, std::fstream::out);

    fstr << "# Hunt log for " << getlogin() << " from "
        << ctime(&tstart.tv_sec);
    fstr << "# time\tcorrectness\n";

    std::vector<logit_t*>::iterator it;
    for (it = items.begin()+1; it != items.end(); it++){
        fstr << (*it)->time << "\t" << (*it)->type << std::endl;
    }

    fstr.close();
}
