#ifndef _CAPP_H_
    #define _CAPP_H_
 
#include <SDL.h>
 
#include "CEvent.h"
 
const int w = 1024;
const int h = 768;

class CApp : public CEvent {
    private:
        bool            Running;
        bool            Fullscreen;
        SDL_Surface*    Surf_Display;
        SDL_Joystick *joystick;

        // demo app variables
        int x,y,z;
        static const int SIZE=300;
 
    public:
        CApp();
        int OnExecute();
        virtual void OnJoyButtonUp(Uint8 which, Uint8 button);
        virtual void OnJoyAxis(Uint8 which, Uint8 axis, Sint16 value);
        virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);

        void renderCursor();
 
    public:
        bool OnInit();
        void OnEvent(SDL_Event* Event);
        void OnLoop();
        void OnExit();
        void OnRender();
        void OnCleanup();
};
 
#endif
