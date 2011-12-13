#ifndef _DEMO_DRAWING_H_
#define _DEMO_DRAWING_H_

#include "BaseApp.h"
#include "CEvent.h"

class DrawingApp : public BaseApp {
    private:
        std::vector<point3D_t*> drawing;
        point3D_t last_drawn;
        int painting_btn;
    public:
        virtual void renderApp();
        virtual void renderApp_first();
        virtual void appInit();

        virtual void appBtnUp(int button);
        virtual char* appHelp(void);
        virtual void appBtnDown(int button);
        virtual void appMotion(int axis, int delta);
        virtual void appKeyDown(SDLKey sym);
        
};

#endif
