#ifdef GL_ES
precision highp float;
#endif

uniform samplerCube text0;  //cubeMap texture
uniform float mixfactor;
uniform mat3 normalMx;



void main(void){
    

    // t goes cw to y and z goes cw to camera
    float x = 2.*(gl_PointCoord.s-0.5);
    float y = -2.*(gl_PointCoord.t-0.5);
    float z = -sqrt(max(0.,1.-x*x-y*y));
    
    vec3 pos = vec3(x,y,z);
    vec3 normal = normalize(pos);
    vec3 lerp_vn = normalMx *normal;
   
    vec4 reflcol = textureCube( text0, lerp_vn );
    
    if (z==0.) discard;

//    gl_FragColor = vec4((1.-mixfactor)*vec3(abs(normal.z))+mixfactor*reflcol.xyz, sqrt(abs(normal.z)));
    gl_FragColor = vec4(vec3(abs(normal.z)),sqrt(abs(normal.z)));
}

/*
uniform samplerCube cubeMapTexture;
uniform float mixfactor;

varying vec3 view;

void main(void){
    float x = 2.*(gl_PointCoord.s-0.5);
    float y = -2.*(gl_PointCoord.t-0.5);
    float z = sqrt(max(0.,1.-x*x-y*y));

    vec3 normal = normalize(vec3(x,y,z));
    vec3 reflTexCoord = reflect(view, normal);
    vec4 reflCol = textureCube(cubeMapTexture, reflTexCoord);

    gl_FragColor = vec4((1.-mixfactor)*vec3(z)+mixfactor*reflCol.xyz, z>0.?1.:0.);
    gl_FragDepth = gl_FragCoord.z-(z/(5000.-0.1));
}

*/