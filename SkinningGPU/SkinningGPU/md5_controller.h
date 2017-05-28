#pragma once


#include "mathlib.h"
#include "helpers.h"


class MD5AnimData;
class MD5ModelData;


class MD5Controller
{
public:

	MD5Controller (MD5ModelData* model_data, MD5AnimData* anim_data);
	~MD5Controller ();

	void				Update (const float dt);

	glm::vec3*			GetSkinnedPos () const;
	glm::quat*			GetSkinnedRot () const;


public:

	MD5ModelData*		model_data_;
	MD5AnimData*		anim_data_;

	glm::vec3*			animated_pos_arr_;
	glm::quat*			animated_rot_arr_;

	glm::vec3*			skinned_pos_arr_;
	glm::quat*			skinned_rot_arr_;

	int					joint_num_;
	float				anim_time_;
};

