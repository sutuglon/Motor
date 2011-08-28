uniform samplerCube cubeMapTexture;
uniform float mixfactor;
varying vec3 normal;
varying vec3 view;

void main(void){
    vec3 reflTexCoord = reflect(view, normal);
    /*vec3 refrTexCoord = refract(-view, normal, 1.);*/
    
    vec4 reflCol = textureCube(cubeMapTexture, reflTexCoord);
    
    /*vec4 refrCol = textureCube(cubeMapTexture, refrTexCoord);*/
    /*gl_FragColor = mix(refrCol,reflCol,mixfactor);*/
    
    gl_FragColor = vec4(reflCol.xyz,mixfactor);
    
}


/*
varying vec3 normal;
uniform samplerCube cubeMapTexture;

void main ()
{
    vec3 norm = normalize(normal);
    vec4 frag = textureCube(cubeMapTexture, norm);

    gl_FragColor =  frag;
}*/
/*
const float Eta = 0.66;
const float FresnelPower = 5.0;
const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));

void main(void){
    vec3 reflTexCoord = reflect(view, normal);
    vec3 refrTexCoord = refract(-view, normal, 1.);
    vec4 reflCol = textureCube(cubeMapTexture, reflTexCoord);
    vec4 refrCol = textureCube(cubeMapTexture, refrTexCoord);
    float ratio = min(1.,max(0.,F + (1.0 - F) * pow((1.0 - dot(-view, normal)), FresnelPower)));

    gl_FragColor = vec4(mix(refrCol,reflCol,ratio).xyz,mixfactor);

}
*/
