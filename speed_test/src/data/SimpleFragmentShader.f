precision mediump float;
varying vec2 UV;
varying float alfa;
uniform sampler2D myTextureSampler;
void main()
{
	gl_FragColor = texture2D( myTextureSampler, UV );
	gl_FragColor.a =gl_FragColor.a* alfa;
	//gl_FragColor.a = 1.0f;
}
