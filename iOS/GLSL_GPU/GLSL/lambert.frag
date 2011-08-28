uniform sampler2D colorTexture;

void main() {
	/* Set the output color of our current pixel*/
	gl_FragColor = vec4(gl_Color.xyz,1)*texture2D(colorTexture, gl_TexCoord[0].st);
}
