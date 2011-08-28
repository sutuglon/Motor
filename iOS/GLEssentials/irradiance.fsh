#ifdef GL_ES
precision highp float;
#endif

uniform samplerCube text0;  // cubemap
uniform sampler2D text1;    // color

uniform float mixfactor;
varying vec3 lerp_vn;
varying vec3 lerp_view;
varying vec2 lerp_uv;

void main(void){

    vec3 refldir = reflect(lerp_view, lerp_vn);
    vec4 reflcol = textureCube( text0, normalize(refldir) );
    vec4 textcol = texture2D(text1, lerp_uv.st);
    
    gl_FragColor = (mixfactor)*reflcol+(1.-mixfactor)*textcol;   
}


/*
uniform samplerCube cubeMapTexture;
uniform sampler2D colorTexture;

uniform float mixfactor;
varying vec3 normal;
varying vec3 view;

void main(void){
    vec3 reflTexCoord = reflect(view, normal);
    vec4 reflCol = textureCube(cubeMapTexture, reflTexCoord);

    gl_FragColor = (mixfactor)*reflCol+(1.-mixfactor)*texture2D(colorTexture, gl_TexCoord[0].st);;
    
}
*/