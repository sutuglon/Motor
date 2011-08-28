
#include "GLInclude.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include "shader.h"

#include "objreader.h"
#include "object.h"
#include "vec.h"
#include "texture.h"
#include "readimg.h"
#include "skybox.h"
#include "particles.h"
#include "chrono.h"
#include "quat.h"
#include "collide_utility.h"
#include "bundle.h"
#include "chemain.h"

using namespace std;

namespace  {

    
    int Width;
    int Height;
    const real nearPlane = 0.1;
    const real farPlane = 1000;
    
    // OpenGL Matrices
    Matrix4 projectionMx;
    Matrix4 modelMx;
    Matrix4 viewMx;
    Matrix4 mvMx;
    Matrix4 mvpMx;
    
    // camera
    Matrix4 w2cam;
    Matrix4 cam2w;
    
    Shader textureShader;
    Shader cubemapShader;
    Shader particleShader;
    Shader irradianceShader;
    Shader sphereShader;
    Shader normalShader;
    const Shader* currentShader;
    
    // regular objects
    std::vector<Object> objs;
    std::vector<Object> colObjs;
    std::vector<DrawObject> drawObjs;
//    Object myobj;
//    DrawObject mydraw;
    
    GLfloat angle;

    // irregular objects

    
    enum eCubeTextureChannels {eSpec,eDiff};
    GLuint cubeTexture[2];
    
    const unsigned particleSize =200;
    const float particleRadius =8.;
    const float particleRadiusFactor = 0.008;
    Particles particles;
    DrawParticles drawParticles;
    
    // collisions
    std::vector<ozcollide::AABBTreePoly*> coltrees;
    
    // indata (multitouch, accelerometer)
    RunData data;
    
    // rotation and position
    Real3 cameraTargets[3]= {
        Real3 (0,-1.,0),
        Real3 (1,-1.,0.5),
        Real3 (-0.4,-1.,0)
    };
    int cameraTargetsIndex = 0;
    Real3 cameraPointTarget(cameraTargets[cameraTargetsIndex]);

    Real3 positionTarget(0,0,6);
    Real3 rotateTarget(0,0,0);
    real scaleTarget=1;

    
    Real3 cameraPointCurrent(cameraPointTarget);
    Real3 rotateCurrent(rotateTarget);
    Real3 positionCurrent(positionTarget);
    real scaleCurrent=scaleTarget;
    
    Chronometer& mainChrono = *Chronos::Singleton().add("mainChrono");
    
    Chronometer& idleChrono = *Chronos::Singleton().add("idleChrono");
    Chronometer& drawChrono = *Chronos::Singleton().add("drawChrono");

}

void ReshapeHandler(int w, int h)
{

	//stores the width and height
	Width = w;
	Height = h;
	glViewport(0, 0, Width, Height);

   	projectionMx = Matrix4Perspective(60, (float)Width / (float)Height, nearPlane, farPlane);
    
}

void IdleHandler(const RunData& rundata) {
    
    idleChrono.reset();    
    idleChrono.start();
    
    data = rundata;
    Real3 cloudpos = Get3Col(drawParticles.cloud_.mx_,3);
    Real3 delta = Real3(5*data.deltaPos_[0]/Width,-5*data.deltaPos_[1]/Height,0);
    

    if (data.touchType_ == 1) {
        switch(data.touchTaps_) {
            case 1:
                cameraTargetsIndex=(++cameraTargetsIndex)%3;
                cameraPointTarget = cameraTargets[cameraTargetsIndex];
                break;
            case 2:
                rotateTarget=Real3(0.);
                cameraTargetsIndex = 0;
                cameraPointTarget = cameraTargets[cameraTargetsIndex];
                scaleTarget=1.;
                break;
        }
    }
    else if(data.touchType_ == 2) {
        switch(data.touchFingers_) {
            case 1:
                rotateTarget += delta;
                break;
            case 2:
                //
                scaleTarget += data.deltaDistance_/(Width/scaleCurrent);
                break;
        }
    }

   
    // limits
    scaleTarget = scaleTarget < 6.0? scaleTarget:6.;
    scaleTarget = scaleTarget > 0.5? scaleTarget:0.5;

    real upperYconst = Pi/2-0.1;
    real lowerYconst = -upperYconst;
    
    rotateTarget[1] = rotateTarget[1]>upperYconst?upperYconst:rotateTarget[1];
    rotateTarget[1] = rotateTarget[1]<lowerYconst?lowerYconst:rotateTarget[1];
    
    // interpolation
    rotateCurrent += .2*(rotateTarget-rotateCurrent);
    positionCurrent+= .2*(positionTarget-positionCurrent);
    
    scaleCurrent +=.2*(scaleTarget-scaleCurrent);
    cameraPointCurrent += .2*(cameraPointTarget-cameraPointCurrent);
    
   	projectionMx = Matrix4Perspective(60*(1./scaleCurrent), (float)Width / (float)Height, nearPlane, farPlane);

    
    mainChrono.stop();
//    real tt = mainChrono.m_lapse*1e-03;

    mainChrono.reset();
    mainChrono.start();
    /*
    Real3 gravity = Matrix4RotateReal3(cam2w, data.lowAcc_); 

    particles.update(tt, gravity, Real3(0,0,0));
//    particles.update(tt, Real3(0,-1,0), Real3(0,0,0));
    
    collideLinearSweep (particles, sphereSize);

    collideElasticLinear(particles, particleRadius*particleRadiusFactor);  //12 100pt
    collideEachOtherQuad2(particles, particleRadius*particleRadiusFactor);//4
    collideWithPolyTrees(particles, drawParticles.cloud_.mx_, colObjs, coltrees); //2
    collideWithSphere(particles, sphereSize-2*(particleRadius*particleRadiusFactor));//0.1

    particles.step();
    */
    idleChrono.stop();
    //std::cout<<"idle"<<idleChrono.m_lapse*1e-03<<std::endl;
}

void DisplayHandler(void)
{
    
    drawChrono.reset();    
    drawChrono.start();
    
    
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  	glCullFace(GL_BACK);


    Matrix4 rotmx=Matrix4Identity();
    Matrix4Mult(rotmx, RotationY(-rotateCurrent[0]),RotationX(rotateCurrent[1]));
    Real3 camPos = Matrix4RotateReal3(rotmx, positionCurrent);
    
    // set camera eye, at, up
    w2cam = Matrix4LookAt(camPos, cameraPointCurrent, Real3(0.f,1.f,0.f));
    Matrix4Mult(w2cam, Matrix4Scale(1.5,1.5,1.5),w2cam);
    cam2w = Matrix4OrthoInverse(w2cam);
    Matrix3 cam2wRot = Matrix4GetRotation(cam2w);
    
    bool inside = false;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    

	// objs

  	glCullFace(GL_BACK);
    
    
    // models 21
    
    for (int i =0;i<static_cast<int>(objs.size()-1);i++)
    {
        const DrawObject& draw = drawObjs[i];
        const Object& obj = objs[i];
        currentShader =  draw.shader_;
        (*currentShader).bind();
        
        float mixfactor = 0.2;
        
        modelMx = draw.mx_;
        viewMx = w2cam;
        
        Matrix4Mult(mvMx, viewMx, modelMx);
        Matrix4Mult(mvpMx, projectionMx, mvMx);
        
        Matrix3 normalMx = Matrix4GetRotation(mvMx);
        Matrix3Inverse(normalMx, normalMx);
        normalMx = Matrix3Transpose(normalMx);
        
        int a = 0;

        glUniform4f(a = glGetUniformLocation((*currentShader).getId(), "_lp0"), -10., 5., 0., 1.);
        glUniform4f(a = glGetUniformLocation((*currentShader).getId(), "_lp1"), 10., 0., 10., 1.);
        glUniform4f(a = glGetUniformLocation((*currentShader).getId(), "_lp2"), 10., 0., -10., 1.);
        glUniformMatrix4fv(a = glGetUniformLocation((*currentShader).getId(), "mvMx"), 1, GL_FALSE, &mvMx[0]);
        glUniformMatrix4fv(a = glGetUniformLocation((*currentShader).getId(), "mvpMx"), 1, GL_FALSE, &mvpMx[0]);
        glUniformMatrix3fv(a = glGetUniformLocation((*currentShader).getId(),"cam2worldMx"), 1, 0, &cam2wRot[0]);
        glUniformMatrix3fv(a = glGetUniformLocation((*currentShader).getId(),"normalMx"), 1, 0, &normalMx[0]);
        glUniform1f(a = glGetUniformLocation((*currentShader).getId(),"mixfactor"), mixfactor);

        glUniform1i(a= glGetUniformLocation((*currentShader).getId(), "text0") , 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, draw.textId_[DrawObject::eText0]);
        
        glUniform1i(glGetUniformLocation((*currentShader).getId(), "text1") , 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, draw.textId_[DrawObject::eText1]);
        
        glUniform1i(glGetUniformLocation((*currentShader).getId(), "text2") , 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, draw.textId_[DrawObject::eText2]);

        glBindVertexArray(draw.vao_);
        glDrawElements(GL_TRIANGLES, obj.fc_.size()*3, GL_UNSIGNED_SHORT, 0);
        (*currentShader).unbind();    
    }
    drawChrono.stop();
    //std::cout<<"draw"<<drawChrono.m_lapse*1e-03<<std::endl;
}



void Init(int backingWidth, int backingHeight) {

   
    
    // normal mapping shader setup
    normalShader.compile("normalmap");
    glBindAttribLocation(normalShader.getId(), DrawObject::eVxIndex, "_vx");
    glBindAttribLocation(normalShader.getId(), DrawObject::eVnIndex, "_vn");
    glBindAttribLocation(normalShader.getId(), DrawObject::eTgIndex, "_tg");
    glBindAttribLocation(normalShader.getId(), DrawObject::eUvIndex, "_uv");
    normalShader.link();


//    // sphere shader setup
//    sphereShader.compile("sphere");
//    glBindAttribLocation(sphereShader.getId(), DrawObject::eVxIndex, "_vx");
//    glBindAttribLocation(sphereShader.getId(), DrawObject::eVnIndex, "_vn");
//    sphereShader.link();
//    
//    // cubemap shader setup
//    cubemapShader.compile("cubemapping");
//    glBindAttribLocation(cubemapShader.getId(), DrawObject::eVxIndex, "_vx");
//    glBindAttribLocation(cubemapShader.getId(), DrawObject::eVnIndex, "_vn");
//    cubemapShader.link();
//    
//    // particle shader setup
//    particleShader.compile("particles");
//    glBindAttribLocation(particleShader.getId(), DrawObject::eVxIndex, "_vx");
//    particleShader.link();
//    
//    // irradiance shader setup
//    irradianceShader.compile("irradiance");
//    glBindAttribLocation(irradianceShader.getId(), DrawObject::eVxIndex, "_vx");
//    glBindAttribLocation(irradianceShader.getId(), DrawObject::eVnIndex, "_vn");
//    glBindAttribLocation(irradianceShader.getId(), DrawObject::eUvIndex, "_uv");
//    irradianceShader.link();
//
    // cubemaps
    std::vector<std::string> specCubeFiles;
    const std::string envName = "snowfield";
    specCubeFiles.push_back(GetBundleResourcePath(envName+"_px.png"));
    specCubeFiles.push_back(GetBundleResourcePath(envName+"_nx.png"));
    specCubeFiles.push_back(GetBundleResourcePath(envName+"_py.png")); 
    specCubeFiles.push_back(GetBundleResourcePath(envName+"_ny.png")); 
    specCubeFiles.push_back(GetBundleResourcePath(envName+"_pz.png")); 
    specCubeFiles.push_back(GetBundleResourcePath(envName+"_nz.png")); 

    
    cubeTexture[eSpec] = makeGLCubeMapTexture(specCubeFiles);
    SetGLTextureFilter(GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_LINEAR);
    
    // diffuse
    std::vector<std::string> diffCubeFiles;

    diffCubeFiles.push_back(GetBundleResourcePath(envName+"_d_px.png") );
    diffCubeFiles.push_back(GetBundleResourcePath(envName+"_d_nx.png") );
    diffCubeFiles.push_back(GetBundleResourcePath(envName+"_d_py.png") ); 
    diffCubeFiles.push_back(GetBundleResourcePath(envName+"_d_ny.png") ); 
    diffCubeFiles.push_back(GetBundleResourcePath(envName+"_d_pz.png") ); 
    diffCubeFiles.push_back(GetBundleResourcePath(envName+"_d_nz.png") ); 

    
    cubeTexture[eDiff] = makeGLCubeMapTexture(diffCubeFiles);
    SetGLTextureFilter(GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_LINEAR);
    
    // keka setup
    std::vector<std::string> objModelNames;
//    objModelNames.push_back("keka");
//    objModelNames.push_back("keko");
//    objModelNames.push_back("koko");
//    objModelNames.push_back("wood");    
//    objModelNames.push_back("snow");    
//    objModelNames.push_back("snowman");
    objModelNames.push_back("pulmonTris");
    
    real scale = 4.;
    Real3 trans(0,-1.,0);
    for (unsigned i =0;i<objModelNames.size();i++)
    {
        objs.push_back(Object());
        colObjs.push_back(Object());
        drawObjs.push_back(DrawObject());
        Object& obj = objs[objs.size()-1];
//        Object& colobj = colObjs[objs.size()-1];
        DrawObject& draw = drawObjs[drawObjs.size()-1];

        readObjectFromFile(obj, GetBundleResourcePath(objModelNames[i]+".bin").c_str());
        
        obj.calculateTangents();
        obj.centerPivot();

        obj.uniformScale(scale);
        obj.uniformTranslate(trans);
        
//        readObjectFromFile(colobj, GetBundleResourcePath(objModelNames[i]+"_col.bin").c_str());
//        colobj.uniformScale(scale);
//        colobj.uniformTranslate(trans);
//        prepare4SphereCollision(colobj, particleRadius*particleRadiusFactor);

        draw.init(obj, GL_STATIC_DRAW);
        draw.textId_[DrawObject::eText0] = cubeTexture[eSpec];
        draw.textId_[DrawObject::eText1] = makeGL2DTexture(GetBundleResourcePath(objModelNames[i]+"_d.png").c_str());
        SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        SetGLTextureFilter(GL_TEXTURE_2D,GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        
        draw.textId_[DrawObject::eText2] = makeGL2DTexture(GetBundleResourcePath(objModelNames[i]+"_n.png").c_str());
        SetGLTextureBorder(GL_TEXTURE_2D, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        SetGLTextureFilter(GL_TEXTURE_2D,GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

        draw.shader_ = &normalShader;
//        draw.shader_ = &irradianceShader;
//        draw.shader_ = &cubemapShader;
    }

    // ball setup
    trans= trans+Real3(0,1.5,0);
    {
        objs.push_back(Object());
        drawObjs.push_back(DrawObject());
        
        Object& obj = objs[objs.size()-1];
        DrawObject& draw = drawObjs[drawObjs.size()-1];
        
        ObjReader objReader0;
        ObjReader::Data dataobj = objReader0.read( GetBundleResourcePath("sphere.obj").c_str() );
        initObjectFromOBJData(obj,dataobj);
        obj.centerPivot();
        //obj.uniformScale(scale);
        obj.uniformTranslate(trans);
        
        draw.init(obj, GL_STATIC_DRAW);
        draw.textId_[DrawObject::eText0] = makeGLCubeMapTexture(specCubeFiles);
        SetGLTextureFilter(GL_TEXTURE_CUBE_MAP, GL_LINEAR, GL_LINEAR);
        
        draw.shader_ = &cubemapShader;
    }

    GetGLError();
    
    
    // Set up OpenGL state that will never change
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    angle = 0;

    buildCollisionTree(colObjs,coltrees);
    
    mainChrono.reset();
    mainChrono.start();
    
    idleChrono.reset();
}

void Cleanup(void) {
    for (unsigned i=0;i<drawObjs.size();i++) {
        drawObjs[i].destroy();
    }
}

