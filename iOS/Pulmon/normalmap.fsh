
#ifdef GL_ES
precision highp float;
#endif

uniform float Roughness;
uniform samplerCube text0;  // cubemap
uniform sampler2D text1;    // colormap
uniform sampler2D text2;    // normalmap
uniform float mixfactor;

varying vec3 lerp_vn;
varying vec3 lerp_view;
varying vec2 lerp_uv;

varying vec3 lerp_normal;
varying vec3 lerp_binormal;
varying vec3 lerp_tangent;

// todo
void main(void){
    
	vec3 normal = normalize( 2.0 * texture2D (text2, lerp_uv.st).rgb - 1.0 );
    mat3 tangent2worldMx = mat3(lerp_tangent, lerp_binormal, lerp_normal);
    normal = tangent2worldMx * vec3(normal.x,-normal.y,normal.z);
    
    vec3 reflected = reflect(lerp_view, normal); // in world coords
    

    float NdotV = dot(normalize(lerp_view),normalize(reflected));
    float specular =pow(max(0.0,NdotV),10.*Roughness);
    float diffuse = 0.07;
    
    vec4 reflcol = textureCube( text0, normalize(reflected) );
	vec4 texcol = texture2D (text1, lerp_uv.st);
//    vec3 ambient = vec3(.1,.04,.7);
    vec4 bumpcol = vec4(texcol.xyz+texcol.xyz*0.1,1.0);

    gl_FragColor = bumpcol  + reflcol*diffuse + reflcol*specular;
    //gl_FragColor = vec4(specular,specular,specular,1.0);
//    gl_FragColor = mix(bumpcol,reflcol,mixfactor);

}

