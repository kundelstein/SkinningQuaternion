#pragma once


#include "mathlib.h"
#include "helpers.h"
#include <string>


struct MD5Joint
{
	glm::vec3			pos_;
	glm::quat			rot_;
};


class MD5AnimData
{
public:

	MD5AnimData (const std::string& file_name);
	~MD5AnimData ();

	int					GetJointNum () const;
	int					GetFrameNum () const;
	float				GetDuration () const;
	int					GetFrameRate () const;
	MD5Joint**			GetFrameSkeleton () const;


private:

	MD5Joint**			frame_skeleton_arr_;

	float				frame_duration_;
	float				anim_duration_;

	int					frame_rate_;
	int					frame_num_;

	int					joint_num_;
};

