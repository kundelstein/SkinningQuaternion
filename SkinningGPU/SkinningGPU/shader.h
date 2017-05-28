#pragma once


#include "renderer.h"
#include <fstream>
#include "mathlib.h"


struct ShaderMatrices
{
	glm::mat4 world_;
	glm::mat4 view_;
	glm::mat4 proj_;
};


class Shader
{
public:

	Shader (char* vertex_name, char* fragment_name);
	~Shader ();

	void SetShader ();
	bool SetShaderParameters (ShaderMatrices*, glm::vec3* pos, glm::quat* rot);


private:


	const char* ReadShaderFromFile (char*);
	void OutputShaderError (unsigned int);
	void OutputLinkerError (unsigned int);

	unsigned int GetUniform (const char*);
	GLuint InitShader (char* name, unsigned int type);


private:

	unsigned int program_id_;
	unsigned int vertex_shader_id_;
	unsigned int fragment_shader_id_;

	int loca_sampler_;
	int loca_world_;
	int loca_view_;
	int loca_proj_;
	int loca_pos_;
	int loca_rot_;
};

