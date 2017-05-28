#include "texture.h"
#include <SOIL2.h>


Texture::Texture (const char* file_name)
{
	glActiveTexture (GL_TEXTURE0);

	glGenTextures (1, &texture_id_);
	glBindTexture (GL_TEXTURE_2D, texture_id_);

	int iWidth, iHeight;
	unsigned char* pPixels = SOIL_load_image (file_name, &iWidth, &iHeight, 0, SOIL_LOAD_RGBA);
	assert (pPixels);

	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pPixels);
	SOIL_free_image_data (pPixels);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap (GL_TEXTURE_2D);
}


Texture::~Texture ()
{
	glDeleteTextures (1, &texture_id_);
}


void Texture::SetTexture ()
{
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, texture_id_);
}

