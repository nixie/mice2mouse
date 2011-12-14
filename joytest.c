/*
 * Date:    28.10.2011 23:31
 * Author:  Radek Fér <xferra00@stud.fit.vutbr.cz>
 * Description:
 *          Uses SDL to enumerate available joysticks and print info about them.
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
#include <SDL/SDL.h>   /* All SDL App's need this */
#include <stdio.h>

int main(int argc, char *argv[]) {
    if((SDL_Init(SDL_INIT_JOYSTICK)==-1)) { 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }

    printf("%i joysticks were found.\n", SDL_NumJoysticks() );

    SDL_Joystick *joystick;
    for( int i=0; i < SDL_NumJoysticks(); i++ ) 
    {
        joystick = SDL_JoystickOpen(i);
        printf("    %s \taxes:%2d  btns:%2d  balls:%2d  hats:%2d\n",
                SDL_JoystickName(i),
                SDL_JoystickNumAxes(joystick),
                SDL_JoystickNumButtons(joystick),
                SDL_JoystickNumBalls(joystick),
                SDL_JoystickNumHats(joystick));
        SDL_JoystickClose(joystick);
    }

    SDL_Quit();
    exit(0);
}

