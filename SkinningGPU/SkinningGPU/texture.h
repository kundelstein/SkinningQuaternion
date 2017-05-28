#pragma once


#include "renderer.h"


class Texture
{
public:

	Texture (const char* file_name);
	~Texture ();

	void SetTexture ();


private:

	unsigned int texture_id_;
};

