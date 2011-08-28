#ifdef GL_ES
precision highp float;
#endif

uniform mat4 mvpMx;
attribute vec4 _vx;
attribute vec2 _uv;
varying vec2 lerp_uv;

void main()
{	
  	gl_Position	= mvpMx * _vx;
    lerp_uv = _uv;
}
