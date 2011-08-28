#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D text0;
varying vec2 lerp_uv;

void main() {
    gl_FragColor = vec4(texture2D(text0, lerp_uv.st, 1.0));
}
