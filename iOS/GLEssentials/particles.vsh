#ifdef GL_ES
precision highp float;
#endif


uniform mat4 mvpMx;
uniform float pointSize;
attribute vec4 _vx;


void main(void){
  	gl_Position	= mvpMx * _vx;
    gl_PointSize = pointSize;
}


/*
uniform mat4 cam2world;
varying vec3 view;


void main(void){
    gl_Position = ftransform();

    view = normalize(gl_ModelViewMatrix * gl_Vertex).xyz;
    view =( cam2world * vec4(view, 0) ).xyz;

    // gl_PointSize = 20;
}
*/