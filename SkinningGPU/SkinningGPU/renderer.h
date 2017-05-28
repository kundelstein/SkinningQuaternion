#pragma once


#define GLEW_STATIC 
#include "GL/glew.h"
#include "GL/wglew.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "mathlib.h"


class Renderer
{
public:

	Renderer ();
	~Renderer ();

	void					Buffer_Clear ();
	void					PollEvents ();
	void					SwapBuffer ();
	bool					ProcessCheckClose () const;
	glm::mat4				GetProjectionMatrix () const;


private:

	glm::mat4				proj_;
	GLFWwindow*				window_;
};


