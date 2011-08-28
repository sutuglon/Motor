
#ifdef GL_ES
precision highp float;
#endif

uniform samplerCube text0;  // cubemap
uniform sampler2D text1;    // colormap
uniform sampler2D text2;    // normalmap
uniform float mixfactor;

varying vec3 lerp_vn;
varying vec3 lerp_view;
varying vec3 lerp_light0;
varying vec3 lerp_light1;
varying vec3 lerp_light2;
varying vec2 lerp_uv;

varying vec3 lerp_normal;
varying vec3 lerp_binormal;
varying vec3 lerp_tangent;

// todo
void main(void){
    
	vec3 normal = normalize( 2.0 * texture2D (text2, lerp_uv.st).rgb - 1.0 );
    mat3 tangent2worldMx = mat3(lerp_tangent, lerp_binormal, lerp_normal);
    normal = tangent2worldMx * vec3(normal.x,-normal.y,normal.z);
    
    //vec3 normal = normalize( texture2D (text2, lerp_uv.st).rgb );
    
	float lamberFactor0 = max (dot (vec3(lerp_light0.x,-lerp_light0.y,lerp_light0.z), normal), 0.0) ;
	float lamberFactor1 = max (dot (vec3(lerp_light1.x,-lerp_light1.y,lerp_light1.z), normal), 0.0) ;
	float lamberFactor2 = max (dot (vec3(lerp_light2.x,-lerp_light2.y,lerp_light2.z), normal), 0.0) ;
    float lamberFactor = 0.5*(lamberFactor0+lamberFactor1);

    vec3 refldir = reflect(lerp_view, normal);
    
    vec4 reflcol = textureCube( text0, normalize(refldir) );
	vec4 texcol = texture2D (text1, lerp_uv.st);
    vec4 bumpcol = vec4(texcol.xyz,1.0);
    
    gl_FragColor = mix(bumpcol,reflcol,mixfactor);
    
//    gl_FragColor = reflcol;

//    gl_FragColor = vec4(lamberFactor,lamberFactor,lamberFactor,1.0);
}

