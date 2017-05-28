#include "shader.h"


namespace
{
	const int MAX_BONES = 33;
}


Shader::Shader (char* vertex_name, char* fragment_name)
{
	vertex_shader_id_ = InitShader (vertex_name, GL_VERTEX_SHADER);
	fragment_shader_id_ = InitShader (fragment_name, GL_FRAGMENT_SHADER);

	program_id_ = glCreateProgram ();
	glAttachShader (program_id_, vertex_shader_id_);
	glAttachShader (program_id_, fragment_shader_id_);
	glLinkProgram (program_id_);

	int status;
	glGetProgramiv (program_id_, GL_LINK_STATUS, &status);
	if (status != 1)
	{
		OutputLinkerError (program_id_);
		return;
	}

	loca_sampler_ = GetUniform ("u_diffuseTexture");
	loca_world_ = GetUniform ("u_world");
	loca_view_ = GetUniform ("u_view");
	loca_proj_ = GetUniform ("u_proj");
	loca_pos_ = GetUniform ("u_skinPos");
	loca_rot_ = GetUniform ("u_skinRot");
}


Shader::~Shader ()
{
	glUseProgram (0);
	glDetachShader (program_id_, vertex_shader_id_);
	glDetachShader (program_id_, fragment_shader_id_);

	glDeleteShader (vertex_shader_id_);
	glDeleteShader (fragment_shader_id_);
	glDeleteProgram (program_id_);
}


void Shader::SetShader ()
{
	glUseProgram (program_id_);
}


unsigned int Shader::GetUniform (const char* name)
{
	int location = glGetUniformLocation (program_id_, name);
	assert (location > -1);
	return location;
}


bool Shader::SetShaderParameters (ShaderMatrices* model_mat_arr, glm::vec3* skinned_pos, glm::quat* skinned_rot)
{
	glUniform1i (loca_sampler_, 0);
	glUniformMatrix4fv (loca_world_, 1, GL_FALSE, glm::value_ptr (model_mat_arr->world_));
	glUniformMatrix4fv (loca_view_, 1, GL_FALSE, glm::value_ptr (model_mat_arr->view_));
	glUniformMatrix4fv (loca_proj_, 1, GL_FALSE, glm::value_ptr (model_mat_arr->proj_));
	glUniform3fv (loca_pos_, MAX_BONES, glm::value_ptr (skinned_pos[0]));
	glUniform4fv (loca_rot_, MAX_BONES, glm::value_ptr (skinned_rot[0]));

	return true;
}


const char* Shader::ReadShaderFromFile (char* file_name)
{
	FILE* file = fopen (file_name, "rb");
	if (!file)
		return NULL;

	int error = fseek (file, 0, SEEK_END);
	if (error != 0)
		return NULL;

	long file_size = ftell (file);
	rewind (file);

	char* buffer = new char[file_size + 1];

	long count = fread (buffer, sizeof (char), file_size, file);
	if (count != file_size)
		return NULL;

	buffer[file_size] = '\0';

	error = fclose (file);
	if (error != 0)
		return NULL;

	return const_cast<const char*>(buffer);
}


void Shader::OutputShaderError (unsigned int shader_id)
{
	int buffer_size;
	glGetShaderiv (shader_id, GL_INFO_LOG_LENGTH, &buffer_size);

	char* buffer = new char[buffer_size];
	if (!buffer)
		return;

	glGetShaderInfoLog (shader_id, buffer_size, 0, buffer);
	printf (buffer);

	delete[] buffer;
	buffer = NULL;
}


void Shader::OutputLinkerError (unsigned int program_id)
{
	int buffer_size;
	glGetProgramiv (program_id, GL_INFO_LOG_LENGTH, &buffer_size);

	char* buffer = new char[buffer_size];
	if (!buffer)
		return;

	glGetProgramInfoLog (program_id, buffer_size, 0, buffer);
	printf (buffer);

	delete[] buffer;
	buffer = NULL;
}


GLuint Shader::InitShader (char* file_name, unsigned int type)
{
	const char* shader_code = ReadShaderFromFile (file_name);
	assert (shader_code);

	const GLuint shader_id = glCreateShader (type);
	glShaderSource (shader_id, 1, &shader_code, 0);
	glCompileShader (shader_id);

	int status;
	glGetShaderiv (shader_id, GL_COMPILE_STATUS, &status);
	if (status != 1)
	{
		OutputShaderError (shader_id);
		return 0;
	}

	delete[] shader_code;

	return shader_id;
}

