

#ifdef GL_ES
precision highp float;
#endif
//uniform vec3 camPos;
uniform mat4 mvMx; 
uniform mat4 mvpMx;
uniform mat3 cam2worldMx;
uniform mat3 normalMx;

attribute vec4 _vx;
attribute vec4 _tg;
attribute vec3 _vn;
attribute vec2 _uv;

varying vec3 lerp_vn;
varying vec3 lerp_view;
varying vec2 lerp_uv;

varying vec3 lerp_normal;
varying vec3 lerp_tangent;
varying vec3 lerp_binormal;

void main(void){
    
   
  	gl_Position	= mvpMx * _vx;
    
    vec3 normal = normalize ( normalMx*_vn ) ;
    vec3 tangent = normalize ( normalMx*vec3(_tg) ) ;
    vec3 binormal = -_tg.w * cross(normal,tangent);
    vec4 view = mvMx*_vx;
    vec3 vertex = vec3(mvMx * _vx);

    // make these two in world coord (so they can access cubemap right)
    lerp_vn = cam2worldMx * normal;
    lerp_view = cam2worldMx * view.xyz;
    
	lerp_uv = _uv;
    lerp_normal = normal;
    lerp_tangent = tangent.xyz;
    lerp_binormal = binormal.xyz;

}
