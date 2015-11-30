uniform sampler2D Texture;

varying vec2 TexCoord;

void main (void)  
{     
   gl_FragColor = texture2D(Texture, TexCoord).bgra;
}