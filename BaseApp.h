#ifndef _BASEAPP_H_
#define _BASEAPP_H_

#include <vector> 
#include <SDL.h>
 
#include "CEvent.h"
 
const int w = 1024;
const int h = 768;
const int N_BTNS = 6;
typedef struct point3D {
    int x,y,z;
    point3D(int _x=0, int _y=0, int _z=0) : x(_x), y(_y), z(_z) {}
} point3D_t;


void printStringUsingGlutBitmapFont(char *string, void *font,
        int x, int y, int z, float r, float g, float b);

class BaseApp : public CEvent {
    private:
        bool            Running;
        bool            Fullscreen;
        SDL_Surface*    Surf_Display;
        SDL_Joystick *  joystick;
        int desktop_w, desktop_h;

        // demo app variables
    public:
        int x,y,z;
        bool buttons[N_BTNS];    // 2 mice, 3 buttons for each mouse
        static const int SIZE=390;
        static const int TEXT_MAX = 200;
        char coords[TEXT_MAX];
        char params[TEXT_MAX];
        float time_increment;
        int displacement;
    private:
        SDL_TimerID timer_id;
        static const int TIMER_INTERVAL = 20;


        // periodic model moving
        bool paused;
        float rot_x, rot_y, rot_z;
        float mytime;



        static unsigned int timer_callback(unsigned int interval, void *param);
        int axis_rot_transform(int axis, int *value);
    public:
        BaseApp();
        int OnExecute();
        virtual void OnJoyButtonUp(Uint8 which, Uint8 button);
        virtual void OnJoyButtonDown(Uint8 which, Uint8 button);
        virtual void OnJoyAxis(Uint8 which, Uint8 axis, Sint16 value);
        virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);

        virtual void renderCursor();
        virtual void renderGrid();
        virtual void renderApp();
        virtual void renderApp_first(){}

        virtual void appInit(){}
        virtual void appTimer(){}
        virtual void appBtnUp(int button){}
        virtual void appBtnDown(int button){}
        virtual void appMotion(int axis, int delta){}
        virtual void appKeyDown(SDLKey sym){}
        
 
    public:
        bool OnInit();
        void OnEvent(SDL_Event* Event);
        void OnLoop();
        void OnExit();
        void OnRender();
        void OnCleanup();
};
 
#endif
