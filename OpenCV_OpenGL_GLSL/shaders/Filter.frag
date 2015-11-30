uniform sampler2D Texture;

uniform vec2 TextureSize;
uniform float Kernel[9];
uniform float KernelWeight;

varying vec2 TexCoord;
 
void main() {
   	vec2 onePixel = vec2(1.0, 1.0) / TextureSize;
   	vec4 colorSum =
    texture2D(Texture, TexCoord + onePixel * vec2(-1, -1)) * Kernel[0] +
    texture2D(Texture, TexCoord + onePixel * vec2( 0, -1)) * Kernel[1] +
    texture2D(Texture, TexCoord + onePixel * vec2( 1, -1)) * Kernel[2] +
    texture2D(Texture, TexCoord + onePixel * vec2(-1,  0)) * Kernel[3] +
    texture2D(Texture, TexCoord + onePixel * vec2( 0,  0)) * Kernel[4] +
    texture2D(Texture, TexCoord + onePixel * vec2( 1,  0)) * Kernel[5] +
    texture2D(Texture, TexCoord + onePixel * vec2(-1,  1)) * Kernel[6] +
    texture2D(Texture, TexCoord + onePixel * vec2( 0,  1)) * Kernel[7] +
    texture2D(Texture, TexCoord + onePixel * vec2( 1,  1)) * Kernel[8] ;
 
   	// Divide the sum by the weight but just use rgb
   	// we'll set alpha to 1.0
   	gl_FragColor = vec4((colorSum / KernelWeight).rgb, 1.0);
}