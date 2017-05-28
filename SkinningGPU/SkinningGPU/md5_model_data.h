#pragma once


#include "renderer.h"
#include "texture.h"
#include "mathlib.h"
#include "helpers.h"
#include <string>
#include <fstream>


class MD5ModelData
{
private:

	struct OGLVert
	{
		glm::vec3			pos_;
		glm::vec2			crd_;
		glm::vec4			bone_indices_;
		glm::vec4			bone_weights_;
	};


	struct OGLMesh
	{
		std::string			texture_name_;
		OGLVert*			vert_arr_;
		unsigned int*		index_arr_;
		unsigned int		vert_num_;
		unsigned int		index_num_;
	};


public:

	MD5ModelData (const std::string& file_name);
	~MD5ModelData ();

	void				Render ();

	void				CreateBuffers ();
	void				DestroyBuffers ();

	void				CreateTextures ();
	void				DestroyTextures ();

	glm::quat*			GetBindposeRot () const;
	glm::vec3*			GetBindposePos () const;

private:

	OGLMesh*			omesh_arr_;
	Texture**			texture_arr_;

	glm::quat*			inv_bindpose_rot_arr_;
	glm::vec3*			inv_bindpose_pos_arr_;

	unsigned int*		vertex_buffer_id_arr_;
	unsigned int*		index_buffer_id_arr_;
	unsigned int*		num_index_arr_;

	int					mesh_num_;
};

