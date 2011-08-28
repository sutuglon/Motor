	uniform sampler2D colorTexture;
	uniform sampler2D normalTexture;
    uniform sampler2D heightTexture;
    uniform samplerCube cubeMapTexture;
    
    uniform float textureSize;
    uniform float scale;
    uniform float heightThreshold;
    
	varying vec3 lightVec;
    varying vec3 eyeVec;

    varying vec3 n;
    varying vec3 t;
    varying vec3 b;


    float mag =scale*textureSize;
    const float aa = 300.0;
    const float nn =50.0;
    const float step = 1.0/nn;
    float texelSize = 1.0 / textureSize;

vec4 texture2DBilinear( sampler2D textureSampler, vec2 uv )
{
    vec4 tl = texture2D(textureSampler, uv);
    vec4 tr = texture2D(textureSampler, uv + vec2(texelSize, 0));
    vec4 bl = texture2D(textureSampler, uv + vec2(0, texelSize));
    vec4 br = texture2D(textureSampler, uv + vec2(texelSize , texelSize));
    vec2 f = fract( uv.xy * textureSize ); 
    vec4 tA = mix( tl, tr, f.x ); 
    vec4 tB = mix( bl, br, f.x ); 
    return mix( tA, tB, f.y );
}

vec2 dFtextureBi( sampler2D sampler, vec2 uv)
{
    float ts = texelSize;
    float r = texture2D(sampler, uv + vec2(ts, 0)).r;
    float l = texture2D(sampler, uv + vec2(-ts, 0)).r;
    float t = texture2D(sampler, uv + vec2(0, ts)).r;
    float b = texture2D(sampler, uv + vec2(0 , -ts)).r;
    
    return vec2(r-l,t-b);
}

vec2 getXYdF(vec2 tcoord) {
    return vec2(clamp(aa*abs(dFdx(tcoord.x)),0.0,1.0),clamp(aa*abs(dFdy(tcoord.y)),0.0,1.0));
}

float getHeight(vec2 tcoord) {
    /*return texture2D(heightTexture,tcoord).g-0.5;*/
    return texture2D(heightTexture,tcoord).r;
}

vec3 getNormal(vec2 tcoord) {
    /*return normalize(2.0 * texture2D (normalTexture, tcoord).rgb - 1.0);*/
    /*return dFtextureBi(heightTexture,tcoord);*/
    
    return normalize( vec3(-mag*dFtextureBi(heightTexture,tcoord),1.0) );
}


vec4 getDiffuseMaterial (vec2 tcoord, float height) {
    
    vec4 ret;
    /*ret =texture2D (colorTexture, tcoord);
    */
    vec4 c1=vec4(172.0/255.0,142.0/255.0,100.0/255.0,1);
    vec4 c2=vec4(56.0/255.0,67.0/255.0,53.0/255.0,1);
    /*height+=0.5;*/
    height*=heightThreshold;
    float t =height*height*height;
    ret = t*c2+(1.0-t)*c1;

    return ret;
}

void main()
{
    vec2 dt = scale * vec2(eyeVec.x,-eyeVec.y)/nn;
    vec2 textcoord = gl_TexCoord[0].st;
    vec2 oriTextCoord = textcoord;
    
    float level = 0.0;
	float height = getHeight(textcoord);

    vec2 XYdF = getXYdF(textcoord);
    
    if (level<height) {
        while (level<height) {
            level+=step; textcoord +=dt;
            height = getHeight(textcoord);
        }
    }
    else {
        while (height<level) {
            level-=step; textcoord -=dt;
            height = getHeight(textcoord);
        }
    }

    vec3 normal = getNormal(textcoord);
    normal = mix( normal , vec3(0.0,0.0,1.0), max(XYdF.x,XYdF.y) );
	vec4 diffuseMaterial = getDiffuseMaterial(textcoord, height);
    
    float shadowFactor = 1.0;
/*
    dt = vec2(lightVec.x,-lightVec.y) * scale / nn;
    while (level>=height && level<1.0) {
        level+=step; textcoord +=dt;
        height = getHeight(textcoord);
    }
    if (level<1.0) {
        shadowFactor = 0.2;
    }
*/
    vec3 norm = normalize(normal);
    mat3 T2W = mat3(t,b,n);
    vec3 wnorm = T2W*norm; 	
    vec4 diffuseIrr = textureCube(cubeMapTexture, wnorm);

    float lamberFactor = max (dot (vec3(lightVec.x,-lightVec.y,lightVec.z), normal), 0.0) ;
    float t = 0.4;
	if (lamberFactor > 0.0)
	{
		gl_FragColor = t*diffuseMaterial + (1.0-t)*diffuseIrr*diffuseMaterial /**lamberFactor * shadowFactor*/;
	}   
}