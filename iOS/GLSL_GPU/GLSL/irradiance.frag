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
