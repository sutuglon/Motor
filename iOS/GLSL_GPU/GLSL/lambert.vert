void DirectionalLight(in int i, in vec3 normal,
                      inout vec4 ambient, inout vec4 diffuse)
{
    float nDotVP; float nDotHV; float pf;
    

    nDotVP = max(0.0, dot(normal, normalize(vec3(gl_LightSource[i].position))));
    ambient += gl_LightSource[i].ambient;
    diffuse += gl_LightSource[i].diffuse * nDotVP;
}

void main() {
    /*Clear the light intensity accumulators*/
    int i;
    vec4 amb, diff;
    vec4 color;
    vec3 normal;
    
    normal = gl_NormalMatrix * gl_Normal;
    normal = normalize(normal);
    
    amb = vec4(0.0); diff = vec4(0.0);
    /* Loop through enabled lights, compute contribution from each */
    for (i = 0; i < 8; i++) {
        if (gl_LightSource[i].position.w == 0.0) 
            DirectionalLight(i, normal, amb, diff);
    }
    color = /*gl_FrontLightModelProduct.sceneColor + 
            amb * gl_FrontMaterial.ambient +*/
            diff * gl_FrontMaterial.diffuse;
    gl_FrontColor = color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}