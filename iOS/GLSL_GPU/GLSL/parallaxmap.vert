	attribute vec4 tangent;
	varying vec3 lightVec;
    varying vec3 eyeVec;

    varying vec3 n;
    varying vec3 t;
    varying vec3 b;
    
void main()
{
	gl_TexCoord[0] =  gl_MultiTexCoord0;
    
	 n = normalize (gl_NormalMatrix * gl_Normal);
	 t = normalize (gl_NormalMatrix * vec3(tangent.xyz));
	 b = -tangent.w*cross (n, t);
	
	vec3 vertexPosition = vec3(gl_ModelViewMatrix *  gl_Vertex);
	vec3 lightDir = normalize(gl_LightSource[0].position.xyz - vertexPosition);
    
	vec3 v;
	v.x = dot (lightDir, t);
	v.y = dot (lightDir, b);
	v.z = dot (lightDir, n);
	lightVec = normalize (v);
    
    vec3 view = -vertexPosition;
    
    /* camera is at position (0,0,0)!*/
	v.x = dot (view, t);
	v.y = dot (view, b);
	v.z = dot (view, n);
	eyeVec = normalize(v);

	gl_Position = ftransform();
}

