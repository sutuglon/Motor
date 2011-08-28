uniform mat4 cam2world;

varying vec3 view;
/*varying vec3 pos;*/

void main(void){
    gl_Position = ftransform();
    /*pos = (gl_ModelViewMatrix * gl_Vertex).xyz;*/
    view = normalize(gl_ModelViewMatrix * gl_Vertex).xyz;
    view =( cam2world * vec4(view, 0) ).xyz;
}

/*
void main()
{	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = ftransform();
}
*/