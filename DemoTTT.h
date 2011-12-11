#ifndef _DEMO_TTT_H_
#define _DEMO_TTT_H_

#include "BaseApp.h"
#include "CEvent.h"

typedef enum occup {ANY=0, PLAYER1, PLAYER2} occup_t;


struct tick3D {
    int x, y, z;    // units are blocks
    int r, g, b;    // color of this tick
};
        

class TTTApp : public BaseApp {
    public:
        static const int n_blks=6;
    private:
        occup_t in_turn;
        occup_t ttt_state[n_blks][n_blks][n_blks];

    public:
        static const int bs=SIZE/n_blks;
        static const int bs_half=bs/2;
        virtual void renderApp();

        virtual void appInit();
        virtual void appBtnUp(int button);
        virtual void appBtnDown(int button);
        virtual void appMotion(int axis, int delta);
        virtual void appKeyDown(SDLKey sym);
        
};

#endif
