uniform sampler2D Texture;

varying vec2 TexCoord;
 
void main() {
	vec3 color = texture2D(Texture, TexCoord).rgb;
	vec3 white = vec3(1.0, 1.0, 1.0);

	vec2 pos = mod(gl_FragCoord.xy, vec2(50.0)) - vec2(25.0);
	float dist_squared = dot(pos, pos);

	gl_FragColor = mix(vec4(color, 1.0), vec4(white, 1.0), smoothstep(380.25, 420.25, dist_squared));  
}