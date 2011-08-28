/*
void main() {
    float s = gl_TexCoord[0].s-0.5;
    float t = gl_TexCoord[0].t-0.5;
    float val = 1.0-5.*(s*s+t*t);
	gl_FragColor = vec4(val,val,val,val);
}
*/

uniform samplerCube cubeMapTexture;
uniform float mixfactor;

varying vec3 view;
/*varying vec3 pos;*/ 

void main(void){

    /*gl_FragColor = vec4(reflCol.xyz,mixfactor);*/
    
    float x = 2.*(gl_TexCoord[0].s-0.5);
    float y = -2.*(gl_TexCoord[0].t-0.5);
    float z = sqrt(max(0.,1.-x*x-y*y));
    /*float val = 1.0-5.*(x*x+y*y);*/
    vec3 normal = normalize(vec3(x,y,z));
    vec3 reflTexCoord = reflect(view, normal);
    vec4 reflCol = textureCube(cubeMapTexture, reflTexCoord);


    if (z==0.) discard;

    gl_FragColor = vec4((1.-mixfactor)*vec3(z)+mixfactor*reflCol.xyz, z<0.5?z:1.);
    gl_FragDepth = gl_FragCoord.z-(z/(5000.-0.1));
    


	/*gl_FragColor = vec4(val,val,val,val);*/
   
}

