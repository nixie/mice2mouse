#ifndef _CAPP_H_
#define _CAPP_H_

#include <vector> 
#include <SDL.h>
#include <SDL_ttf.h>
 
#include "CEvent.h"
 
const int w = 1024;
const int h = 768;

typedef struct point3D {
    int x,y,z;
    point3D(int _x=0, int _y=0, int _z=0) : x(_x), y(_y), z(_z) {}
} point3D_t;

class DrawingDemo : public CEvent {
    private:
        bool            Running;
        bool            Fullscreen;
        SDL_Surface*    Surf_Display;
        SDL_Joystick *  joystick;
        int desktop_w, desktop_h;

        // demo app variables
        int x,y,z;
        static const int SIZE=300;
        bool rh_left_btn; // right hand, left button pressed
        std::vector<point3D_t*> drawing;
        point3D_t last_drawn;
        SDL_TimerID timer_id;
        static const int TIMER_INTERVAL = 20;

        static const int TEXT_MAX = 200;
        char coords[TEXT_MAX];
        char params[TEXT_MAX];

        // periodic model moving
        bool paused;
        float displac_x, displac_y;
        float mytime;
        int displacement;
        float time_increment;



        static unsigned int timer_callback(unsigned int interval, void *param);
 
    public:
        DrawingDemo();
        int OnExecute();
        virtual void OnJoyButtonUp(Uint8 which, Uint8 button);
        virtual void OnJoyButtonDown(Uint8 which, Uint8 button);
        virtual void OnJoyAxis(Uint8 which, Uint8 axis, Sint16 value);
        virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);

        void renderCursor();
        void renderGrid();
        void renderDrawing();
 
    public:
        bool OnInit();
        void OnEvent(SDL_Event* Event);
        void OnLoop();
        void OnExit();
        void OnRender();
        void OnCleanup();
};
 
#endif
