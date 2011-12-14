/*
   Date:     04.11.2011 17:26
   Author:   Radek Fér

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
#ifndef __MICE2MOUSE_H__
#define __MICE2MOUSE_H__


// this module provides some functions, where applications can register
// its functions called, when there is an update from virtual 3Dmouse.
// They are GLUT-like ...

// func will be called after some of mouse buttons are pressed
// btn: number identifing button pressed (BTN_0..5)
void m2m_mouseFunc(void (*func)(int btn, int state));

// 3D coordinates has changed
void m2m_relMotionFunc(void (*func)(int dx, int dy, int dz));

// this must be registered in the main module of client application with
// glutIdleFunc()
void m2m_workHorse();

// m2m initialization
int m2m_init(char *evdev1, char *evdev2);

#endif

