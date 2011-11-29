#include <SDL/SDL.h>   /* All SDL App's need this */
#include <stdio.h>

int main(int argc, char *argv[]) {
    
    printf("Initializing SDL.\n");
    
    /* Initialize defaults, Video and Audio */
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)==-1)) { 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }

    printf("%i joysticks were found.\n", SDL_NumJoysticks() );

    SDL_Joystick *joystick;
    for( int i=0; i < SDL_NumJoysticks(); i++ ) 
    {
        joystick = SDL_JoystickOpen(i);
        printf("    %s \taxes: %d \tbtns: %d\n", SDL_JoystickName(i),
                SDL_JoystickNumAxes(joystick),
                SDL_JoystickNumButtons(joystick));
        SDL_JoystickClose(joystick);
    }


    printf("Quiting SDL.\n");
    
    /* Shutdown all subsystems */
    SDL_Quit();

    exit(0);
}

