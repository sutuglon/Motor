#ifdef GL_ES
precision highp float;
#endif

uniform mat4 mvMx; 
uniform mat4 mvpMx;
uniform mat3 cam2worldMx;
uniform mat3 normalMx;

attribute vec4 _vx;
attribute vec3 _vn;
attribute vec2 _uv;

varying vec3 lerp_vn;
varying vec3 lerp_view;
varying vec2 lerp_uv;

void main(void){
  	gl_Position	= mvpMx * _vx;
 
    vec3 normal = normalize( normalMx*_vn ) ;
    vec4 view = -mvMx*_vx;
    
    lerp_vn = cam2worldMx * normal;
    lerp_view = cam2worldMx * view.xyz;
    
	lerp_uv = _uv;
}

/*
uniform mat4 cam2world;
varying vec3 normal;
varying vec3 view;

void main(void){
    gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
    normal = normalize(gl_NormalMatrix * gl_Normal);
    view = normalize(gl_ModelViewMatrix * gl_Vertex).xyz;
    
    normal =( cam2world * vec4(normal, 0) ).xyz;
    view =( cam2world * vec4(view, 0) ).xyz;
}

*/