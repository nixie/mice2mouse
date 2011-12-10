#include "BaseApp.h"
#include "CEvent.h"

class DrawingApp : public BaseApp {
    private:
        std::vector<point3D_t*> drawing;
        point3D_t last_drawn;
    public:
        virtual void renderApp();

        virtual void appBtnUp(int button);
        virtual void appBtnDown(int button);
        virtual void appMotion(int axis, int delta);
        virtual void appKeyDown(SDLKey sym);
        
};
