
#ifdef GL_ES
precision highp float;
#endif


uniform samplerCube text0;  //cubeMap texture
uniform float mixfactor;
varying vec3 lerp_vn;
varying vec3 lerp_view;

void main(void){
    vec3 refldir = reflect(lerp_view, lerp_vn);
    vec4 reflCol = textureCube(text0, normalize(refldir) );
    
    gl_FragColor = vec4(reflCol.xyz,mixfactor);
    //gl_FragColor = vec4(normalize(lerp_view),1);
    // gl_FragColor = vec4(normalize(lerp_view),mixfactor);
    
}