uniform sampler2D colorTexture;

void main() {
	// Set the output color of our current pixel
	gl_FragColor = texture2D(colorTexture, gl_TexCoord[0].st);
    
}
