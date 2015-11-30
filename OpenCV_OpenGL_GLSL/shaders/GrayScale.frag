uniform sampler2D Texture;

varying vec2 TexCoord;
 
void main() {
	vec3 color = texture2D(Texture, TexCoord).rgb;
	float gray = (color.r + color.g + color.b) / 3.0;
	vec3 grayscale = vec3(gray);     
	gl_FragColor = vec4(grayscale, 1.0);
}