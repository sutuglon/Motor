#include <iostream>
#include "GLInclude.h"
#include "resourcemap.h"
#include "readimg.h"


int setupGLTexture(  unsigned char * const image, int width, int height, int channels, int texName) ;
void display(void);

static GLuint texName;

int kWindowWidth = 256; 
int kWindowHeight = 256;

int res = 1024;

void init(void)
{
  
    std::cout << "Hello, PNG Read!\n";



    std::string filePath = "mastercard.png";
    
    Image img;
    if (!img.load(filePath) )
        return;


    glEnable (GL_DEPTH_TEST);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glGenTextures(1, &texName);

    // setup opengl texture
    setupGLTexture(const_cast<unsigned char* const>(img.getBits()), img.getWidth(), img.getHeight(), img.getChannels(), texName);
}

// int setupGLTexture char* int int int Function loads image from buffer into

int setupGLTexture(  unsigned char * const image, int width, int height, int channels, int texName) 
{
    if (image == NULL) {
        return 0;
    }
    unsigned char* img = (unsigned char*)malloc(res*res*sizeof(unsigned char)*4);
    memset(img, 255, res*res*4*sizeof(unsigned char));
    for (int i=0;i<height;i++) {
        for (int j=0;j<width;j++) {
            unsigned ix = 4*(i*width+j);
            unsigned ixx = 4*((i)*res+j);
            //image[ix]=0;
            //image[ix+1]=0;
            //image[ix+2]=255;
//            image[ix+3]=255;
//            printf("%d \tr%d \tg%d \tb%d \ta%d\n", i, image[ix],image[ix+1],image[ix+2],image[ix+3]);

            img[ixx]=image[ix];
            img[ixx+1]=image[ix+1];
            img[ixx+2]=image[ix+2];
            img[ixx+3]=image[ix+3];
              
        }
    }

    printf("(load Texture) width: %d height: %d channels:%d\n", width, height, channels); 
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res,res, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
        delete img;

		// Typical Texture Generation Using Data From The Bitmap
//		glBindTexture(GL_TEXTURE_2D, texName);
//		glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, image);
//		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    return 1; 
}


void display(void) 
{
    if (!kWindowHeight)
    return;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glTranslatef (0, 0, -20);

    glEnable (GL_TEXTURE_2D);
    glBindTexture (GL_TEXTURE_2D, texName); 

    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);


    glBegin (GL_QUADS);
    glTexCoord2f (0.0f,0.0f); // lower left corner of image 
    glVertex3f (-10.0f, -10.0f, 0.0f);
    glTexCoord2f (1.0f, 0.0f); // lower right corner of image 
    glVertex3f (10.0f, -10.0f, 0.0f);
    glTexCoord2f (1.0f, 1.0f); // upper right corner of image 
    glVertex3f (10.0f, 10.0f, 0.0f);
    glTexCoord2f (0.0f, 1.0f); // upper left corner of image 
    glVertex3f (-10.0f, 10.0f, 0.0f);
    glEnd ();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glutSwapBuffers();
}

void resize (int w, int h)
{
    kWindowWidth = w;
    kWindowHeight = h;

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(55, (GLfloat) w / (GLfloat) h, 1, 9999);
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 27:    // ESC
    case 'Q':   
    case 'q':
        exit(0);
    break;
    default:
    break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow("PNG Texture Example");


    init();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}
