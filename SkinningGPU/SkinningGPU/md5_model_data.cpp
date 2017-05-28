#include "md5_model_data.h"
#include "md5_controller.h"
#include "md5_anim_data.h"


MD5ModelData::MD5ModelData (const std::string& file_name)
{
	omesh_arr_ = NULL;

	vertex_buffer_id_arr_ = NULL;
	index_buffer_id_arr_ = NULL;
	num_index_arr_ = NULL;
	texture_arr_ = NULL;

	inv_bindpose_rot_arr_ = NULL;
	inv_bindpose_pos_arr_ = NULL;


	struct MD5Vert
	{
		glm::vec2			crd_;
		int					weight_start_;
		int					weight_num_;
	};


	struct MD5Tri
	{
		int					idx_arr_[3];
	};


	struct MD5Weight
	{
		int					joint_id_;
		float				bias_;
		glm::vec3			pos_;
	};


	// MD5JointInfo is not urgent data, relating 1:1 to MD5Joint, can be used in future for IK or anchoring systems (if forwarded somewhere).
	struct MD5JointInfo
	{
		std::string			name_;
		int					parent_id_;
	};


	struct MD5Mesh
	{
		std::string			shader_name_;
		MD5Vert*			vert_arr_;
		MD5Tri*				tri_arr_;
		MD5Weight*			weight_arr_;
		int					vert_num_;
		int					tri_num_;
		int					weight_num_;
	};


	std::ifstream file (file_name);
	assert (file.is_open ());

	std::streampos initpos = file.tellg ();
	file.seekg (0, std::ios::end);
	std::streampos filelength = file.tellg ();
	file.seekg (initpos);

	int imesh = 0;
	std::string param, junk;
	file >> param;


	int					joint_num = 0;
	MD5Joint*			joint_arr = NULL;
	MD5JointInfo*		joint_info_arr = NULL;
	MD5Mesh*			mesh_arr = NULL;


	while (!file.eof ())
	{
		if (param == "MD5Version")
		{
			int version;
			file >> version;
			assert (version == 10);

			printf ("MD5Version: %i\n", version);
		}
		else if (param == "numJoints")
		{
			file >> joint_num;

			joint_arr = new MD5Joint[joint_num];
			joint_info_arr = new MD5JointInfo[joint_num];

			inv_bindpose_rot_arr_ = new glm::quat[joint_num];
			inv_bindpose_pos_arr_ = new glm::vec3[joint_num];

			printf ("numJoints: %i\n", joint_num);
		}
		else if (param == "numMeshes")
		{
			file >> mesh_num_;
			mesh_arr = new MD5Mesh[mesh_num_];
			omesh_arr_ = new MD5ModelData::OGLMesh[mesh_num_];

			printf ("numMeshes: %i\n", mesh_num_);
		}
		else if (param == "joints")
		{
			file >> junk;
			for (int i = 0; i < joint_num; ++i)
			{
				MD5Joint& joint = joint_arr[i];
				MD5JointInfo& joint_info = joint_info_arr[i];

				file >> joint_info.name_ >> joint_info.parent_id_ >> junk;
				file >> joint.pos_[0] >> joint.pos_[1] >> joint.pos_[2];
				file >> junk >> junk;
				file >> joint.rot_[0] >> joint.rot_[1] >> joint.rot_[2];
				file >> junk;
				file.ignore (filelength, '\n');

				RemoveQuotes (joint_info.name_);

				joint.rot_.w = quat_w (joint.rot_);

			}
			file >> junk;


			for (int i = 0; i < joint_num; ++i)
			{
				const MD5Joint& joint = joint_arr[i];
				const glm::mat4 rotation = glm::toMat4 (joint.rot_);

				const glm::mat4 rotationInv = glm::transpose (rotation);
				const glm::mat4 translationInv = glm::translate (glm::mat4 (1), -joint.pos_);

				inv_bindpose_rot_arr_[i] = glm::inverse (joint.rot_);
				inv_bindpose_pos_arr_[i] = -joint.pos_;
			}
		}
		else if (param == "mesh")
		{
			MD5Mesh& mesh = mesh_arr[imesh++];

			file >> junk;
			file >> param;

			while (param != "}")
			{
				if (param == "shader")
				{
					file >> mesh.shader_name_;
					RemoveQuotes (mesh.shader_name_);

					std::string texture = mesh.shader_name_ + '\n';
					printf (texture.c_str ());
				}
				else if (param == "numverts")
				{
					file >> mesh.vert_num_;
					mesh.vert_arr_ = new MD5Vert[mesh.vert_num_];

					for (int i = 0; i < mesh.vert_num_; ++i)
					{
						MD5Vert& vert = mesh.vert_arr_[i];

						file >> junk >> junk >> junk; //vert vertIdx (
						file >> vert.crd_[0] >> vert.crd_[1] >> junk; // )
						file >> vert.weight_start_ >> vert.weight_num_;
					}
				}
				else if (param == "numtris")
				{
					file >> mesh.tri_num_;
					mesh.tri_arr_ = new MD5Tri[mesh.tri_num_];

					for (int i = 0; i < mesh.tri_num_; ++i)
					{
						MD5Tri& tri = mesh.tri_arr_[i];

						file >> junk >> junk; //tri triIdx
						file >> tri.idx_arr_[0] >> tri.idx_arr_[1] >> tri.idx_arr_[2];
					}
				}
				else if (param == "numweights")
				{
					file >> mesh.weight_num_;
					mesh.weight_arr_ = new MD5Weight[mesh.weight_num_];

					for (int i = 0; i < mesh.weight_num_; ++i)
					{
						MD5Weight& weight = mesh.weight_arr_[i];

						file >> junk >> junk; //weight weightIdx
						file >> weight.joint_id_ >> weight.bias_ >> junk; // (
						file >> weight.pos_[0] >> weight.pos_[1] >> weight.pos_[2];
						file >> junk; // )
					}
				}

				file >> param;
			}
		}

		file >> param;
	}


	for (int i = 0; i < mesh_num_; ++i)
	{
		MD5Mesh& mesh = mesh_arr[i];
		MD5ModelData::OGLMesh& finalmesh = omesh_arr_[i];

		finalmesh.texture_name_ = mesh.shader_name_;
		finalmesh.vert_num_ = mesh.vert_num_;
		finalmesh.index_num_ = mesh.tri_num_ * 3;

		finalmesh.vert_arr_ = new MD5ModelData::OGLVert[finalmesh.vert_num_];
		finalmesh.index_arr_ = new unsigned int[finalmesh.index_num_];


		for (int j = 0; j < mesh.vert_num_; ++j)
		{
			const MD5Vert& vert = mesh.vert_arr_[j];
			MD5ModelData::OGLVert& finalvert = finalmesh.vert_arr_[j];

			finalvert.pos_ = glm::vec3 (0);
			finalvert.crd_ = vert.crd_;
			finalvert.bone_indices_ = glm::vec4 (0);
			finalvert.bone_weights_ = glm::vec4 (0);

			// @TODO: Add weight limiting here.
			for (int k = 0; k < vert.weight_num_; ++k)
			{
				if (k >= 4)
					break;

				const MD5Weight& weight = mesh.weight_arr_[vert.weight_start_ + k];
				const MD5Joint& joint = joint_arr[weight.joint_id_];

				finalvert.pos_ += (joint.pos_ + joint.rot_ * weight.pos_) * weight.bias_;
				finalvert.bone_indices_[k] = static_cast<float>(weight.joint_id_);
				finalvert.bone_weights_[k] = weight.bias_;
			}
		}


		for (int j = 0; j < mesh.tri_num_; ++j)
		{
			finalmesh.index_arr_[j * 3 + 0] = mesh.tri_arr_[j].idx_arr_[2];
			finalmesh.index_arr_[j * 3 + 1] = mesh.tri_arr_[j].idx_arr_[1];
			finalmesh.index_arr_[j * 3 + 2] = mesh.tri_arr_[j].idx_arr_[0];
		}
	}


	delete[] joint_arr;
	delete[] joint_info_arr;

	for (int i = 0; i < mesh_num_; ++i)
	{
		delete[] mesh_arr[i].vert_arr_;
		delete[] mesh_arr[i].tri_arr_;
		delete[] mesh_arr[i].weight_arr_;
	}
	delete[] mesh_arr;
}


void MD5ModelData::CreateBuffers ()
{
	vertex_buffer_id_arr_ = new unsigned int[mesh_num_];
	index_buffer_id_arr_ = new unsigned int[mesh_num_];
	num_index_arr_ = new unsigned int[mesh_num_];

	glGenBuffers (mesh_num_, vertex_buffer_id_arr_);
	glGenBuffers (mesh_num_, index_buffer_id_arr_);

	for (int i = 0; i < mesh_num_; ++i)
	{
		num_index_arr_[i] = omesh_arr_[i].index_num_;

		glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_id_arr_[i]);
		glBufferData (GL_ARRAY_BUFFER, sizeof (OGLVert) * omesh_arr_[i].vert_num_, omesh_arr_[i].vert_arr_, GL_STATIC_DRAW);

		size_t textureOffset = sizeof (glm::vec3);
		size_t indicesOffset = sizeof (glm::vec2) + textureOffset;
		size_t weightsOffset = sizeof (glm::vec4) + indicesOffset;
		glVertexAttribPointer (0, 3, GL_FLOAT, false, sizeof (OGLVert), 0);
		glVertexAttribPointer (1, 2, GL_FLOAT, false, sizeof (OGLVert), (void*)textureOffset);
		glVertexAttribPointer (2, 4, GL_FLOAT, false, sizeof (OGLVert), (void*)indicesOffset);
		glVertexAttribPointer (3, 4, GL_FLOAT, false, sizeof (OGLVert), (void*)weightsOffset);

		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, index_buffer_id_arr_[i]);
		glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (unsigned int) * omesh_arr_[i].index_num_, omesh_arr_[i].index_arr_, GL_STATIC_DRAW);
	}
}


void MD5ModelData::DestroyBuffers ()
{
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers (mesh_num_, index_buffer_id_arr_);

	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glDeleteBuffers (mesh_num_, vertex_buffer_id_arr_);

	delete[] index_buffer_id_arr_;
	delete[] vertex_buffer_id_arr_;
	delete[] num_index_arr_;
}


void MD5ModelData::CreateTextures ()
{
	// @TODO: Please note, this is huge lazyness from my part. Textures can be shared between several Meshes so you should use some kind of Map instead.
	texture_arr_ = new Texture*[mesh_num_];

	for (int i = 0; i < mesh_num_; ++i)
	{
		texture_arr_[i] = new Texture (omesh_arr_[i].texture_name_.c_str ());
	}
}


void MD5ModelData::DestroyTextures ()
{
	for (int i = 0; i < mesh_num_; ++i)
	{
		delete texture_arr_[i];
	}
	delete[] texture_arr_;
}


MD5ModelData::~MD5ModelData ()
{
	DestroyTextures ();
	DestroyBuffers ();

	delete[] inv_bindpose_pos_arr_;
	delete[] inv_bindpose_rot_arr_;

	for (int i = 0; i < mesh_num_; ++i)
	{
		delete[] omesh_arr_[i].vert_arr_;
		delete[] omesh_arr_[i].index_arr_;
	}


	delete[] omesh_arr_;
}


void MD5ModelData::Render ()
{
	for (int i = 0; i < mesh_num_; ++i)
	{
		texture_arr_[i]->SetTexture ();

		glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_id_arr_[i]);

		size_t textureOffset = sizeof (glm::vec3);
		size_t indicesOffset = sizeof (glm::vec2) + textureOffset;
		size_t weightsOffset = sizeof (glm::vec4) + indicesOffset;

		glVertexAttribPointer (0, 3, GL_FLOAT, false, sizeof (OGLVert), 0);
		glEnableVertexAttribArray (0);

		glVertexAttribPointer (1, 2, GL_FLOAT, false, sizeof (OGLVert), (void*)textureOffset);
		glEnableVertexAttribArray (1);

		glVertexAttribPointer (2, 4, GL_FLOAT, false, sizeof (OGLVert), (void*)indicesOffset);
		glEnableVertexAttribArray (2);

		glVertexAttribPointer (3, 4, GL_FLOAT, false, sizeof (OGLVert), (void*)weightsOffset);
		glEnableVertexAttribArray (3);


		glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, index_buffer_id_arr_[i]);
		glDrawElements (GL_TRIANGLES, num_index_arr_[i], GL_UNSIGNED_INT, 0);
	}
}


glm::quat* MD5ModelData::GetBindposeRot () const
{
	return inv_bindpose_rot_arr_;
}


glm::vec3* MD5ModelData::GetBindposePos () const
{
	return inv_bindpose_pos_arr_;
}

