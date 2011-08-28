

#include "GLInclude.h"
#include "draw.h"
#include "testCollide.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

using namespace std;

int glutTest(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize (SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(100,100);
    glutCreateWindow ("Collide");
    Draw::init();
    glutReshapeFunc (Draw::reshape);

    Collide::init(SCREEN_WIDTH,SCREEN_HEIGHT);
    glutMainLoop();
    return 0;
}

int main(int argc, char **argv)
{
    return glutTest(argc, argv);
}

