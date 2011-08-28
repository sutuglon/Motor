
#ifdef GL_ES
precision highp float;
#endif

uniform mat4 mvMx; 
uniform mat4 mvpMx;
uniform mat3 cam2worldMx;
uniform mat3 normalMx;

attribute vec4 _vx;
attribute vec3 _vn;

varying vec3 lerp_vn;
varying vec3 lerp_view;

void main(void){
    
  	gl_Position	= mvpMx * _vx;
    
    vec3 normal = normalize( normalMx*_vn ) ;
    vec4 view = mvMx*_vx;

    lerp_vn = cam2worldMx * normal;
    lerp_view = cam2worldMx * view.xyz;
}



