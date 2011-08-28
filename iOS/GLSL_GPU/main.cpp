/* =====================================================================================================================
 * File - main.cpp
 * ---------------------------------------------------------------------------------------------------------------------
 */

/* =====================================================================================================================
 * ---------------------------------------------------------------------------------------------------------------------
 */

/*
    const float textureSize = 2048.0; //size of the texture
    const float texelSize = 1.0 / textureSize; //size of one texel 
 

vec3 texture_dFdz( sampler2D textureSampler, vec2 uv )
{
    // in vertex shaders you should use texture2DLod instead of texture2D
    float tlF = texture2D(textureSampler, uv).g;
    float trF = texture2D(textureSampler, uv + vec2(texelSize, 0)).g;
    float blF = texture2D(textureSampler, uv + vec2(0, texelSize)).g;
    float brF = texture2D(textureSampler, uv + vec2(texelSize , texelSize)).g;
    
    float x = (trF-tlF, brF-blF, f.x);
    float y = (tlF-blF, trF-brF, f.y);
    float z = sqrt(1-x*x-y*y);
    return vec3(x,y,z);
}
*/
#include "GLInclude.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include "shader.h"
#include "bunny.h"
#include "bunnyh.h"
#include "objreader.h"
#include "object.h"
#include "vec.h"
#include "texture.h"
#include "voronoi.h"
#include "readimg.h"
#include "skybox.h"
#include "particles.h"
#include "chrono.h"
#include "quat.h"
#include "collide_utility.h"

using namespace std;

/* =====================================================================================================================
 * ---------------------------------------------------------------------------------------------------------------------
 */

const int Width	= 1024;
const int Height	= 768;


/* =====================================================================================================================
 * ---------------------------------------------------------------------------------------------------------------------
 */

int g_dxWindow;
int g_dyWindow;
Real3 positionTarget(0,0,14);
Real3 rotateTarget(0,0,0);
float scaleZTarget=1.;
float scaleTarget = 1;
float sphereSize= 5.;

Real3 rotateCurrent(rotateTarget);
Real3 positionCurrent(positionTarget);
float scaleZCurrent;
float scaleCurrent;

Matrix4 obj;
Matrix4 cam;
Skybox skybox;
const unsigned particleSize =100;
const real particleRadius = 20;
Particles particles;
DrawParticles drawParticles;

Chronometer& mainChrono = *Chronos::Singleton().add("mainChrono");

/* =====================================================================================================================
 * ---------------------------------------------------------------------------------------------------------------------
 */
Shader textureShader;
Shader cubemapShader;
Shader lambertShader;
Shader irradianceShader;
std::vector<Shader> shaders;
//std::vector<Shader>::iterator currentShader;
Shader* currentShader;

GLuint colorTexture;
GLuint normalTexture;
GLuint heightTexture;
GLuint cubeSpecTexture;
GLuint cubeDiffTexture;
int textureSize=0;
float parallaxScale=0.001;
float heightThreshold = 1.0;

float angle = 0;

// quat test
Real3 from (1,0,0);
Real3 to(1,1,1);
Matrix4 quatrot;


/* =====================================================================================================================
 * ---------------------------------------------------------------------------------------------------------------------
 */
 
bool showBasis=false;
Int2 deltaMousePos;
Int2 oldMousePos(0);


std::vector<Object> objs;
std::vector<Object> colobjs;

std::vector<ozcollide::AABBTreePoly*> coltrees;

    
void processMouseActiveMotion(int x, int y) {
    Int2 p(x,y);
    deltaMousePos = p - oldMousePos;
    rotateTarget[0] += 0.01*deltaMousePos[1];
    rotateTarget[1] += 0.01*deltaMousePos[0];
    oldMousePos = p;
}

void processMousePassiveMotion(int x, int y) {
    Int2 p(x,y);
    deltaMousePos = p - oldMousePos;
    oldMousePos = p;
}

void ReshapeHandler(int w, int h)
{
	//stores the width and height
	g_dxWindow = w;
	g_dyWindow = h;
	glViewport(0, 0, g_dxWindow, g_dyWindow);


	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (60, (GLfloat)w / (GLfloat)h, .1, 1000.0);
	glMatrixMode (GL_MODELVIEW);
}

void IdleHandler(void) {
    rotateCurrent += 0.02*(rotateTarget-rotateCurrent);
    positionCurrent+= 0.02*(positionTarget-positionCurrent);
    scaleZCurrent+= 0.02*(scaleZTarget-scaleZCurrent);
    
    mainChrono.stop();
    real tt = mainChrono.m_lapse*1e-03;

    mainChrono.reset();
    mainChrono.start();

    particles.update(tt, Real3(0.01*(Width/2-oldMousePos[0]),0.01*(-Height/2+oldMousePos[1]),0)/*gravity*/, 
                     Real3(0.01*(Width/2-oldMousePos[0]),0.01*(-Height/2+oldMousePos[1]),0)/*shake*/);
//    particles.update(tt, Real3(0,0,0),Real3(0,-5,0));    

    collideElastic(particles);
    collideEachOther(particles);
    collideWithPolyTrees(particles, colobjs, coltrees);
    collideWithSphere(particles, sphereSize-2*(particleRadius*0.015));

    particles.step();
    
    // quat test
//    quatrot = toMatrix4(quatFromVectors(from, to));
    //std::cout<<quatrot<<std::endl;
    
    glutPostRedisplay();
}

void DisplayHandler(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();  

    Matrix4 rotmx;
    Matrix4Mult(RotationY(-rotateCurrent[1]),RotationX(-rotateCurrent[0]),rotmx);
    Real3 campos = Matrix4RotateReal3(rotmx, positionCurrent);
    
    // set camera
	//gluLookAt (pos[0],pos[1],pos[2], 0, 0, 0, 0.0, 1.0, 0.0);
    // eye, at, up
    cam = LookAt(campos, Real3(0.f), Real3(0.f,1.f,0.f));
    
    
    scaleZTarget = 1.+(positionTarget[2]-14.)*0.1;
    scaleZTarget = scaleZTarget>1.?scaleZTarget:1.;
    
    

    
    // skybox    
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    currentShader = &textureShader;
    (*currentShader).bind();
    
        glUniform1i(glGetUniformLocation((*currentShader).getId(), "colorTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        
        glPushMatrix();
            // affected only by camera rotation
            glScalef(1,1,scaleZCurrent);
            Matrix4 rot(cam);
            SetCol(rot,3,Real3(0,0,0));
            glMultMatrixf(&rot[0]);
            skybox.drawCube();
        glPopMatrix();
    
    (*currentShader).unbind();

    // set camera
    bool inside = positionCurrent.norm()-1.<=sphereSize;

    
    glMultMatrixf(&cam[0]);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    
    // ball 
    
    glEnable(GL_TEXTURE_CUBE_MAP);
        
    // ball back
    
    glCullFace(GL_FRONT);
    Matrix4 camInv = OrthoInverse(cam);
    camInv[5]*= inside ? -1.:1.;
    float mixfactor = inside? 1.0:0.5;

//    currentShader = &cubemapShader;
//    (*currentShader).bind();
//
//        glUniformMatrix4fv(glGetUniformLocation((*currentShader).getId(),"cam2world"), 1, 0, &camInv[0]);
//        glUniform1fARB(glGetUniformLocation((*currentShader).getId(),"mixfactor"), mixfactor);
//
//        glUniform1i(glGetUniformLocation((*currentShader).getId(), "cubeMapTexture"), 0);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeSpecTexture);
//        
//        glPushMatrix();
//            glutSolidSphere(sphereSize, 50, 50);
//        glPopMatrix();
//    
//    (*currentShader).unbind();
//    
//    camInv[5]*= inside ? -1.:1.;
    
    
    // inside motive
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    
    currentShader = &irradianceShader;
    (*currentShader).bind();
        glUniformMatrix4fv(glGetUniformLocation((*currentShader).getId(),"cam2world"), 1, 0, &camInv[0]);
        glUniform1fARB(glGetUniformLocation((*currentShader).getId(),"mixfactor"), 0.3);

               
        glUniform1i(glGetUniformLocation((*currentShader).getId(), "cubeMapTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeDiffTexture);
        
        glUniform1i(glGetUniformLocation((*currentShader).getId(), "colorTexture"), 1);
        glActiveTexture(GL_TEXTURE1);
    
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            for(unsigned i=0;i<objs.size();i++) {
                const Object& obj = objs[i];
                glBindTexture(GL_TEXTURE_2D, obj.texture_);
                glVertexPointer(3, GL_FLOAT, 0, &obj.vx_[0]);
                glNormalPointer(GL_FLOAT, 0, &obj.vn_[0]);
                glTexCoordPointer(2, GL_FLOAT, 0, &obj.uv_[0]);                
                glDrawElements(	GL_TRIANGLES, obj.fc_.size()*3, GL_UNSIGNED_SHORT, &obj.fc_[0]);
            }
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

    
    (*currentShader).unbind();
    
    if (showBasis) {
        currentShader = &cubemapShader;
        (*currentShader).bind();

            glUniformMatrix4fv(glGetUniformLocation((*currentShader).getId(),"cam2world"), 1, 0, &camInv[0]);
            glUniform1fARB(glGetUniformLocation((*currentShader).getId(),"mixfactor"), 1.0);

                   
            glUniform1i(glGetUniformLocation((*currentShader).getId(), "cubeMapTexture"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeDiffTexture);
  
        
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);

                for(unsigned i=0;i<colobjs.size();i++) {
                    const Object& obj = colobjs[i];

                    glVertexPointer(3, GL_FLOAT, 0, &obj.vx_[0]);
                    glNormalPointer(GL_FLOAT, 0, &obj.vn_[0]);              
                    glDrawElements(	GL_TRIANGLES, obj.fc_.size()*3, GL_UNSIGNED_SHORT, &obj.fc_[0]);
                }
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);

        
        (*currentShader).unbind();
    
    }

    
    // particles
    glColor3ub(255,255,255);
    glCullFace(GL_BACK);
    //drawParticles.draw(particles, campos, camInv, cubeDiffTexture);
      

    glColor3ub(127,127,127);
    if(showBasis) {
        float scale = 0.1;
        for(unsigned i=0;i<colobjs.size();i++) {
            const Object& obj = colobjs[i];
            glDrawArrays(GL_POINTS, 0, obj.vx_.size());
            
            glColor3ub(60,60,255);
            glBegin(GL_LINES);
            for (unsigned j=0;j<obj.vn_.size();j++) {
                const Real3& a =obj.vx_[j];
                const Real3& b =obj.vx_[j]+scale*obj.vn_[j];
                glVertex3fv(&a[0]);
                glVertex3fv(&b[0]);
            }
            glEnd();
            glColor3ub(255,60,60);
            glBegin(GL_LINES);
            for (unsigned j=0;j<obj.tg_.size();j++) {
                const Real3& a =obj.vx_[j];
                const Real3& b =obj.vx_[j]+scale*Real3(obj.tg_[j][0],obj.tg_[j][1],obj.tg_[j][2]);
                glVertex3fv(&a[0]);
                glVertex3fv(&b[0]);
            }
            glEnd();
            glColor3ub(60,255,60);
            glBegin(GL_LINES);
            for (unsigned j=0;j<obj.tg_.size();j++) {
                Real3 nm(obj.vn_[j]);
                Real3 tg(obj.tg_[j][0],obj.tg_[j][1],obj.tg_[j][2]);
                Real3 bi = -obj.tg_[j][3]*crossProd(nm,tg);
                const Real3& a =obj.vx_[j];
                const Real3& b =obj.vx_[j]+scale*bi;
                glVertex3fv(&a[0]);
                glVertex3fv(&b[0]);
            }
            glEnd();
        }
    }
    

    // ball front
    
//    if (!inside) {
//    
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_BACK);
//        
//        currentShader = &cubemapShader;
//        (*currentShader).bind();
//        
//        
//            glUniformMatrix4fv(glGetUniformLocation((*currentShader).getId(),"cam2world"), 1, 0, &camInv[0]);
//            glUniform1fARB(glGetUniformLocation((*currentShader).getId(),"mixfactor"), 0.2);
//
//            glUniform1i(glGetUniformLocation((*currentShader).getId(), "cubeMapTexture"), 0);
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeSpecTexture);
//            
//            glPushMatrix();
//                glutSolidSphere(sphereSize, 50, 50);
//            glPopMatrix();
//        
//        (*currentShader).unbind();
//    
//    }
    
    // debug
    glDisable(GL_DEPTH_TEST);
    
/*
    glColor3ub(255,0,0);
    glBegin(GL_LINES);
    {
        Real3 a =Real3(0,0,0);
        Real3 b =10.*from;
        glVertex3fv(&a[0]);
        glVertex3fv(&b[0]);
    }
    glEnd();
    
    glColor3ub(0,255,0);
    glBegin(GL_LINES);
    {
        Real3 a =Real3(0,0,0);
        Real3 b =10.*to;
        glVertex3fv(&a[0]);
        glVertex3fv(&b[0]);
    }
    glEnd();
*/
    
    // lights 
    /*
    glPushMatrix ();
        glRotated ((GLdouble) angle, 1.0, 1.0, 0.0);
        GLfloat direction[] = { 0.0, 0.0, 10., 0.0 };
        glLightfv (GL_LIGHT0, GL_POSITION, direction);
        glTranslatef(direction[0], direction[1], direction[2]);

    glColor3f (0.0, 1.0, 1.0);
    glutWireCube (0.5);
    glPopMatrix();
    
    glPushMatrix();
    glRotated ((GLdouble) angle+120, 0.0, 1.0, 0.0);
    glTranslatef(direction[0], direction[1], direction[2]);
    glLightfv (GL_LIGHT1, GL_POSITION, direction);
    
    glColor3f (0.0, 10.0, 10.0);
    glutWireCube (0.5);
    glPopMatrix ();
    
    glPushMatrix();
    glRotated ((GLdouble) angle+240, 0.0, 1.0, 0.0);
    glTranslatef(direction[0], direction[1], direction[2]);
    glLightfv (GL_LIGHT2, GL_POSITION, direction);
    
    glColor3f (0.0, 10.0, 10.0);
    glutWireCube (0.5);
    glPopMatrix ();
    */
	//angle += 0.05f;
    glutSwapBuffers();

}


void KeyboardHandler(unsigned char key, int x, int y)
{
	switch ( key )
	{
        case '+':
            positionTarget[2] += 0.2;
            break;
        case '-':
            positionTarget[2] -= 0.2;
            break;
        case 'p':
            parallaxScale+=0.001;
            break;
        case 'P':
            parallaxScale-=0.001;
            break;
        case 't':
            heightThreshold+=0.051;
            break;
        case 'T':
            heightThreshold-=0.051;
            break;

//        case 's':
//            currentShader++;
//            if (currentShader==shaders.end()) 
//                currentShader=shaders.begin();
//            glutPostRedisplay();
//            break;

		case 27:
		case 'q':
		case 'Q':
			exit(0);
	}
}


void init(void) {
    glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//  glEnable(GL_CULL_FACE);
//  glCullFace(GL_BACK);
//  glCullFace(GL_FRONT);
//  glShadeModel(GL_FLAT);
    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_MULTISAMPLE);
    //glDepthFunc(GL_LESS);
    
    GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);

    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
//   glClearColor (0.0, 0.0, 0.0, 0.0);
//   glShadeModel (GL_SMOOTH);
//   glEnable(GL_LIGHTING);
//   glEnable(GL_LIGHT0);

    //adding here the mouse processing callbacks
    glutMotionFunc(processMouseActiveMotion);
    glutPassiveMotionFunc(processMousePassiveMotion);
//    

//    shaders.reserve(100);
//    shaders.push_back(Shader());
//    shaders[shaders.size()-1].setup("lambert.vert","lambert.frag");
//    shaders.push_back(Shader());
//    shaders[shaders.size()-1].setup("normalmap.vert","normalmap.frag");
//    shaders.push_back(Shader());
//    shaders[shaders.size()-1].setup("parallaxmap.vert","parallaxmap.frag");

    textureShader.setup("texture.vert","texture.frag");
    lambertShader.setup("lambert.vert","lambert.frag");
    cubemapShader.setup("cubemapping.vert","cubemapping.frag");
	irradianceShader.setup("irradiance.vert","irradiance.frag");
    
    //shaders[shaders.size()-1].setup("parallaxmap.vert","parallaxmap.frag");
   
//    currentShader = shaders.begin();
    
//    float f = 50;
//    int sizex=512;
//    int sizey=512;
//    Texture<unsigned char> text(sizex,sizey,1);
//    
//
//    for (int k=0;k<3;k++) {
//        float ff = f/(10*k+1);
//        for(int x=sizex; x--; ) { // Loops are best comparing the counter with zero, so this counts down.
//            for(int y=sizey; y--;) {
//                float c = max(0.0, min(Voronoi::Get(x/ff,y/ff, 0.5, Voronoi::Difference21, Voronoi::Length), 1.0) );
//                /*c = c>0.1?1.0:c;*/
//                c/=pow(2,(2-k));
//                unsigned char B= static_cast<unsigned char>(floor(255*c));
//                text(y,x,0)+=B;
//            } 
//        }
//    }
    
//    textureSize = 1024;
//    colorTexture = makeGL2DTexture("aama_b2.png");
//    normalTexture = make2DTexture("aamaText3_n.png");
//    heightTexture = make2DTexture("aamaText3_d.png");
//    normalTexture = make2DTexture("aamaText1024_n.png");
//    heightTexture = make2DTexture("aamaText1024_d.png");
//    normalTexture = makeGL2DTexture("aamaText1024_n.png");
//    heightTexture = makeGL2DTexture(text);
//    normalTexture = make2DTexture("copa_n.png");
//    heightTexture = make2DTexture("copa_h.png");
//    SetGLTextureFilter(GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
//    SetGLTextureBorder(GL_TEXTURE_2D, GL_REPEAT, GL_REPEAT);
    colorTexture = makeGL2DTexture("cm_px.png");


    // specular
    std::vector<std::string> faceFiles;
//    const std::string envName = "desert";
    const std::string envName = "snowField";
    faceFiles.push_back(envName+"_px.png");
    faceFiles.push_back(envName+"_nx.png");
    faceFiles.push_back(envName+"_py.png"); 
    faceFiles.push_back(envName+"_ny.png"); 
    faceFiles.push_back(envName+"_pz.png"); 
    faceFiles.push_back(envName+"_nz.png"); 
    
    skybox.init(faceFiles);

    cubeSpecTexture = makeGLCubeMapTexture(faceFiles);
    SetGLTextureFilter(GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_LINEAR);
    
    // diffuse
    faceFiles.clear();
    faceFiles.push_back(envName+"_d_px.png");
    faceFiles.push_back(envName+"_d_nx.png");
    faceFiles.push_back(envName+"_d_py.png"); 
    faceFiles.push_back(envName+"_d_ny.png"); 
    faceFiles.push_back(envName+"_d_pz.png"); 
    faceFiles.push_back(envName+"_d_nz.png"); 
    

    cubeDiffTexture = makeGLCubeMapTexture(faceFiles);
    SetGLTextureFilter(GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_LINEAR);

    // particles
    particles.init(particleSize,particleRadius);
    randomizeInSphere(particles,sphereSize);

    std::vector<std::string> particlesFiles;
    particlesFiles.push_back("particle_diff.png");
//    faceFiles.push_back("desert_nx.png");

    drawParticles.init(particles, particlesFiles);

    mainChrono.reset();
    mainChrono.start();

    std::vector<std::string> objModelNames;
    objModelNames.push_back("pulmonTris");
//    objModelNames.push_back("keka");
//    objModelNames.push_back("keko");
//    objModelNames.push_back("koko");
//    objModelNames.push_back("wood");    
//    objModelNames.push_back("snow");    
//    objModelNames.push_back("snowman");

    real scale = 10.;
    Real3 trans(0,-3.,0);
    for (unsigned i=0;i<objModelNames.size();i++) {
        objs.push_back(Object());
        colobjs.push_back(Object());

        Object& obj = objs[i];
//        Object& colobj = colobjs[i];
        
//        initObjectFromRawData(obj, bunnyh_vertex, bunnyh_vertex_size, bunnyh_indices, bunnyh_indices_size);
//        obj.uniformScale(scale);
//        initObjectFromRawData(colobj, bunny_vertex, bunny_vertex_size, bunny_indices, bunny_indices_size, 35.);
//        colobj.uniformScale(scale);


        ObjReader objReader0;
        ObjReader::Data dataobj = objReader0.read((objModelNames[i]+".obj").c_str());
        initObjectFromOBJData(obj,dataobj);
//
//
        
        writeObjectToFile(obj, (objModelNames[i]+".bin").c_str());
//        readObjectFromFile(obj, (objModelNames[i]+".bin").c_str());
        
//        for (unsigned j=0;j<pepe.fc_.size();j++) {
//            std::cout<< pepe.fc_[j]<<std::endl;
//            std::cout<< obj.fc_[j]<<std::endl;
//        }
        
//        ObjReader objReader1;
//        ObjReader::Data datacol = objReader1.read((objModelNames[i]+"_col.obj").c_str());
//        initObjectFromOBJData(colobj,datacol);
////
//        writeObjectToFile(colobj, (objModelNames[i]+"_col.bin").c_str());
//        readObjectFromFile(colobj, (objModelNames[i]+"_col.bin").c_str());
        
        
        obj.texture_ = makeGL2DTexture((objModelNames[i]+"_d.png").c_str());
        obj.uniformScale(scale);
        obj.uniformTranslate(trans);
        obj.centerPivot();
        //obj.calculateNormals();
        //prepare4SphereCollision(obj, particleRadius*0.015);
        

        //colobj.centerPivot();
        //colobj.calculateNormals();
//        colobj.uniformScale(scale);
//        colobj.uniformTranslate(trans);
//        prepare4SphereCollision(colobj, particleRadius*0.015);
    }
    
    buildCollisionTree(colobjs,coltrees);

    
//    ObjReader reader;
//    ObjReader::Data data = reader.read("aamatext3.obj");
//    ObjReader::Data data = reader.read("chalicetri.obj");
    
//    objs.push_back(Object());
//    Object& obj = objs[0];
//    initObjectFromOBJData(obj,data);
//    obj.calculateTangents();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH ); //set up the double buffering

	glutInitWindowPosition(10, 50);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("OpenGL Shader Language Demo");

	glutDisplayFunc(DisplayHandler);
	glutIdleFunc(IdleHandler);
	glutReshapeFunc(ReshapeHandler);
	glutKeyboardFunc(KeyboardHandler);

	glClearColor(.5, 0.5, 0.5, 1.0);

    init();

	glutMainLoop();

	return 0;
}



  
//  	int cam2WorldLoc = glGetUniformLocation((*currentShader).getId(),"cam2world");
//    glUniformMatrix4fv(cam2WorldLoc, 1, 0, &camInv[0]);

//	int heightThresholdLoc = glGetUniformLocation((*currentShader).getId(),"heightThreshold");
//	glUniform1fARB(heightThresholdLoc, heightThreshold);
//
//	int parallaxScaleLoc = glGetUniformLocation((*currentShader).getId(),"scale");
//	glUniform1fARB(parallaxScaleLoc, parallaxScale);
//       
//	int textureSizeLoc = glGetUniformLocation((*currentShader).getId(),"textureSize");
//	glUniform1fARB(textureSizeLoc, textureSize);

//    colorLoc = glGetUniformLocation((*currentShader).getId(), "colorTexture");
//    glUniform1i(colorLoc, 0);
//    glActiveTexture(GL_TEXTURE0);        
//    glBindTexture(GL_TEXTURE_2D, colorTexture);
//    
//    int normalLoc = glGetUniformLocation((*currentShader).getId(), "normalTexture");
//    glUniform1i(normalLoc, 1);
//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, normalTexture);
    
//    int heightLoc = glGetUniformLocation((*currentShader).getId(), "heightTexture");
//    glUniform1i(heightLoc, 2);
//    glActiveTexture(GL_TEXTURE2);
//    glBindTexture(GL_TEXTURE_2D, heightTexture);
    
//    int cubeLoc = glGetUniformLocation((*currentShader).getId(), "cubeMapTexture");
//    glUniform1i(cubeLoc, 3);
//    glActiveTexture(GL_TEXTURE3);
//    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
//    glEnable(GL_TEXTURE_CUBE_MAP);
    
    //int tangentLoc = glGetAttribLocation((*currentShader).getId(), "tangent");   
    
//    glMultMatrixf(&cam[0]);
//    glPushMatrix();

    
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_NORMAL_ARRAY);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//    glEnableVertexAttribArray(tangentLoc);
//
//
//    for(unsigned i=0;i<objs.size();i++) {
//        glVertexPointer(3, GL_FLOAT, 0, &objs[i].vx_[0]);
//        glNormalPointer(GL_FLOAT, 0, &objs[i].vn_[0]);
//        glTexCoordPointer(2, GL_FLOAT, 0, &objs[i].uv_[0]);
//        glVertexAttribPointer(tangentLoc, 4, GL_FLOAT,GL_FALSE, 0, &objs[i].tg_[0]);
//        glDrawElements(	GL_TRIANGLES, objs[i].fc_.size()*3, GL_UNSIGNED_INT, &objs[i].fc_[0]);
//    }

   
    //glutSolidTeapot(0.5);
//    glutSolidSphere(sphereSize, 50, 50);
//    
//    (*currentShader).unbind();

//    glPointSize(5);
//    glColor3ub(127,127,127);
//    if(showBasis) {
//        for(unsigned i=0;i<objs.size();i++) {
//            glDrawArrays(GL_POINTS, 0, objs[i].vx_.size());
//            
//            glColor3ub(60,60,255);
//            glBegin(GL_LINES);
//            for (unsigned j=0;j<objs[i].vx_.size();j++) {
//                const Real3& a =objs[i].vx_[j];
//                const Real3& b =objs[i].vx_[j]+0.01*objs[i].vn_[j];
//                glVertex3fv(&a[0]);
//                glVertex3fv(&b[0]);
//            }
//            glEnd();
//            glColor3ub(255,60,60);
//            glBegin(GL_LINES);
//            for (unsigned j=0;j<objs[i].vx_.size();j++) {
//                const Real3& a =objs[i].vx_[j];
//                const Real3& b =objs[i].vx_[j]+0.01*Real3(objs[i].tg_[j][0],objs[i].tg_[j][1],objs[i].tg_[j][2]);
//                glVertex3fv(&a[0]);
//                glVertex3fv(&b[0]);
//            }
//            glEnd();
//            glColor3ub(60,255,60);
//            glBegin(GL_LINES);
//            for (unsigned j=0;j<objs[i].vx_.size();j++) {
//                Real3 nm(objs[i].vn_[j]);
//                Real3 tg(objs[i].tg_[j][0],objs[i].tg_[j][1],objs[i].tg_[j][2]);
//                Real3 bi = -objs[i].tg_[j][3]*crossProd(nm,tg);
//                const Real3& a =objs[i].vx_[j];
//                const Real3& b =objs[i].vx_[j]+0.01*bi;
//                glVertex3fv(&a[0]);
//                glVertex3fv(&b[0]);
//            }
//            glEnd();
//        }
//    }
    
//    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisableClientState(GL_NORMAL_ARRAY);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    glDisableVertexAttribArray(tangentLoc);
    
