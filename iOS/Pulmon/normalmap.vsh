

#ifdef GL_ES
precision highp float;
#endif

uniform mat4 mvMx; 
uniform mat4 mvpMx;
uniform mat3 cam2worldMx;
uniform mat3 normalMx;
uniform vec4 _lp0;
uniform vec4 _lp1;
uniform vec4 _lp2;

attribute vec4 _vx;
attribute vec4 _tg;
attribute vec3 _vn;
attribute vec2 _uv;

varying vec3 lerp_vn;
varying vec3 lerp_view;
varying vec2 lerp_uv;
varying vec3 lerp_light0;
varying vec3 lerp_light1;
varying vec3 lerp_light2;
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
    
    vec3 lightDir0 = normalize(_lp0.xyz - vertex);
    vec3 lightDir1 = normalize(_lp1.xyz - vertex);
    vec3 lightDir2 = normalize(_lp2.xyz - vertex);
    
    lerp_vn = cam2worldMx * normal;
    lerp_view = cam2worldMx * view.xyz;
	lerp_uv = _uv;

    lerp_light0 = normalize ( vec3( dot(lightDir0,tangent), dot(lightDir0,binormal), dot(lightDir0,normal) ) );
    lerp_light1 = normalize ( vec3( dot(lightDir1,tangent), dot(lightDir1,binormal), dot(lightDir1,normal) ) );
    lerp_light2 = normalize ( vec3( dot(lightDir2,tangent), dot(lightDir2,binormal), dot(lightDir2,normal) ) );
    
    lerp_normal = normal;
    lerp_tangent = tangent.xyz;
    lerp_binormal = binormal.xyz;

}
