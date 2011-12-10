
#include "BaseApp.h"
#include "GL/glut.h"

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    BaseApp theApp;
    return theApp.OnExecute();
}
