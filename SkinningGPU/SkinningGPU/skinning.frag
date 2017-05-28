#version 150


in vec2 v_texCoord;
uniform sampler2D u_diffuseTexture;


void main ()
{
	gl_FragColor = texture2D (u_diffuseTexture, v_texCoord);
}

