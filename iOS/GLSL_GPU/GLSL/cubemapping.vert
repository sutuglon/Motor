uniform mat4 cam2world;
varying vec3 normal;
varying vec3 view;

void main(void){
    gl_Position = ftransform();
    normal = normalize(gl_NormalMatrix * gl_Normal);
    view = normalize(gl_ModelViewMatrix * gl_Vertex).xyz;
 
    normal =( cam2world * vec4(normal, 0) ).xyz;
    view =( cam2world * vec4(view, 0) ).xyz;

}

/*
varying vec3 normal;


void main ()
{
    normal = gl_NormalMatrix * gl_Normal;
    gl_Position = ftransform();
}
*/