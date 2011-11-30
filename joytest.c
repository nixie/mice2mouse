/*
 * File:    joytest.c
 * Date:    28.10.2011 23:31
 * Author:  Radek Fér <xferra00@stud.fit.vutbr.cz>
 * Course:  ITU
 * Description:
 *          Uses SDL to enumerate available joysticks and print info about them.
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

