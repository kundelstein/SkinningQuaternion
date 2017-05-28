#include "renderer.h"
#include "mathlib.h"
#include <string>


namespace
{
	const glm::vec2 display_size_ (600,400);
	const std::string window_name_ ("SkinningGPU");
}


Renderer::Renderer ()
{
	proj_ = glm::mat4 (1);


	if (!glfwInit ())
	{
		printf ("glfwInit");
		return;
	}

	GLFWmonitor* output = NULL;
	glm::ivec2 position (16, 40);	// @NOTE: Window position.


	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);


	window_ = glfwCreateWindow ((int)display_size_.x, (int)display_size_.y, window_name_.c_str (), output, NULL);
	if (window_ == NULL)
	{
		glfwTerminate ();
		printf ("glfwCreateWindow");
		return;
	}


	glfwMakeContextCurrent (window_);


	// Now we have a temporary context, we can find out if we support OGL 3.x
	char* ver = (char*)glGetString (GL_VERSION);
	int major = ver[0] - '0';		// Major version integer.
	int minor = ver[2] - '0';		// Minor version integer.

	printf ("VideoApi:: Current OpenGL version: %d.%d (%s).", major, minor, ver);
	if (major < 3)
	{
		printf ("VideoApi::VideoApi(): Device does not support OpenGL 3.x!");
		return;
	}

	if (major == 3 && minor < 1)
	{
		printf ("VideoApi::VideoApi(): Device does not support OpenGL 3.1!");
		return;
	}


	if (glewInit () != GLEW_OK)
	{
		printf ("VideoApi::VideoApi(): Cannot initialise GLEW!");
		glfwTerminate ();
		return;
	}


	if (!GLEW_VERSION_3_1)
	{
		printf ("VideoApi::VideoApi(): Cannot initialise GLEW_VERSION_3_1!");
		glfwTerminate ();
	}


	while (glGetError () != GL_NO_ERROR)
	{
	}


	glClearColor (0.2f, 0.6f, 0.8f, 1.0f);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable (GL_BLEND);

	glCullFace (GL_BACK);
	glEnable (GL_CULL_FACE);

	glDepthFunc (GL_LEQUAL);
	glEnable (GL_DEPTH_TEST);

	glViewport (0, 0, static_cast<int>(display_size_.x), static_cast<int>(display_size_.y));
	proj_ = glm::perspective (glm::radians (45.0f), display_size_.x / display_size_.y, 0.01f, 1000.0f);
}


Renderer::~Renderer ()
{
	glfwTerminate ();
}


void Renderer::Buffer_Clear ()
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Renderer::PollEvents ()
{
	glfwPollEvents ();
}


void Renderer::SwapBuffer ()
{
	glfwSwapBuffers (window_);
}


bool Renderer::ProcessCheckClose () const
{
	if (glfwWindowShouldClose (window_))
		return true;

	if (glfwGetKey (window_, GLFW_KEY_ESCAPE))
		return true;

	return false;
}


glm::mat4 Renderer::GetProjectionMatrix () const
{
	return proj_;
}

