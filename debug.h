/*
 * File:    debug.h  (macro DEBUG used for preprocessor eliminable debugging)
 * Date:    21.10.2011 10:31
 * Author:  Radek Fér <xferra00@stud.fit.vutbr.cz>
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
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifndef NDEBUG
#define DEBUG(x) x
#else
#define DEBUG(x)
#endif

#endif

