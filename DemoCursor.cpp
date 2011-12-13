
#include "BaseApp.h"
#include "GL/glut.h"

class CursorDemo : public BaseApp {
    public:
        virtual char *appHelp(){
            static char text[] = 
                "Welcome to m2m introduction demo :)\n"
                "You can press down the mouse middle\n"
                "button and rotate with space beeing\n"
                "displayed ... Try it!\n\n"
                "After You are done with exploration,\n"
                "press Escape, next demo is waiting\n"
                "for You ...";
            return text;
        }

};

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    CursorDemo theApp;
    return theApp.OnExecute();
}
