#pragma once


#include "mathlib.h"
#include "helpers.h"


class MD5Controller;
class MD5AnimData;
class MD5ModelData;


class MD5Example
{
public:

	MD5Example (const char* filename1, const char* filename2);
	~MD5Example ();

	void				Update (float);
	void				Render ();


public:

	MD5Controller*		anim_ctrl_;
	MD5AnimData*		anim_data_;
	MD5ModelData*		model_data_;
};


