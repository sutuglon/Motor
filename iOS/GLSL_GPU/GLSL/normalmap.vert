	attribute vec4 tangent;
	varying vec3 lightVec;

    

void main()
{
	gl_TexCoord[0] =  gl_MultiTexCoord0;
    
	vec3 n = normalize (gl_NormalMatrix * gl_Normal);
	vec3 t = normalize (gl_NormalMatrix * vec3(tangent.xyz));
	vec3 b = -tangent.w*cross (n, t);
	
	vec3 vertexPosition = vec3(gl_ModelViewMatrix *  gl_Vertex);
	vec3 lightDir = normalize(gl_LightSource[0].position.xyz - vertexPosition);

	vec3 v;
	v.x = dot (lightDir, t);
	v.y = dot (lightDir, b);
	v.z = dot (lightDir, n);
	lightVec = normalize (v);
    

	gl_Position = ftransform();
}

