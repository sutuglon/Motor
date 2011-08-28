	uniform sampler2D colorTexture;
	uniform sampler2D normalTexture;
    
   
     
	varying vec3 lightVec;

void main()
{
  
    
      
        
   
                 
              
	vec3 normal = 2.0 * texture2D (normalTexture, gl_TexCoord[0].st).rgb - 1.0;
	normal =  normalize(normal);
	
	float lamberFactor = max (dot (vec3(lightVec.x,-lightVec.y,lightVec.z), normal), 0.0) ;
	vec4 diffuseMaterial;
    
	if (lamberFactor > 0.0)
	{
		diffuseMaterial = texture2D (colorTexture, gl_TexCoord[0].st);
		gl_FragColor =	diffuseMaterial * lamberFactor ;
	}

}			
