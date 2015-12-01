uniform sampler2D Texture;

varying vec2 TexCoord;
 
void main() {
	vec3 color = texture2D(Texture, TexCoord).rgb;
	float gray = (color.r + color.g + color.b) / 3.0;

	gl_FragColor = (gray <= 0.3) ? vec4(gray - 0.3, gray - 0.3, gray + 0.1, 1.0) : vec4(1.0, 1.0, 1.0, 1.0);  
}