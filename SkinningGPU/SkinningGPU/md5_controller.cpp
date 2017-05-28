#include "md5_controller.h"
#include "md5_anim_data.h"
#include "md5_model_data.h"


MD5Controller::MD5Controller (MD5ModelData* model_data, MD5AnimData* anim_data)
{
	model_data_ = model_data;
	anim_data_ = anim_data;

	joint_num_ = anim_data->GetJointNum ();
	anim_time_ = 0.0f;

	animated_pos_arr_ = new glm::vec3[joint_num_];
	animated_rot_arr_ = new glm::quat[joint_num_];

	skinned_pos_arr_ = new glm::vec3[joint_num_];
	skinned_rot_arr_ = new glm::quat[joint_num_];
}


MD5Controller::~MD5Controller ()
{
	delete[] skinned_rot_arr_;
	delete[] skinned_pos_arr_;

	delete[] animated_rot_arr_;
	delete[] animated_pos_arr_;
}


void MD5Controller::Update (const float dt)
{
	anim_time_ += dt;
	if (anim_time_ > anim_data_->GetDuration ())
	{
		anim_time_ = 0.0f;
	}

	const float curframe = anim_time_ * float (anim_data_->GetFrameRate ());
	const int frame0 = (int)floorf (curframe);
	const int frame1 = (frame0 == anim_data_->GetFrameNum () - 1) ? 0 : frame0 + 1;

	const float alpha = curframe - float (frame0);
	const MD5Joint* const frameskel0_joint_arr = anim_data_->GetFrameSkeleton ()[frame0];
	const MD5Joint* const frameskel1_joint_arr = anim_data_->GetFrameSkeleton ()[frame1];

	// Interpolate skeletons
	for (int i = 0; i < joint_num_; ++i)
	{
		const MD5Joint& frame0 = frameskel0_joint_arr[i];
		const MD5Joint& frame1 = frameskel1_joint_arr[i];

		animated_pos_arr_[i] = glm::lerp (frame0.pos_, frame1.pos_, alpha);
		animated_rot_arr_[i] = glm::slerp (frame0.rot_, frame1.rot_, alpha);
	}

	// Prepare for render
	for (int i = 0; i < joint_num_; ++i)
	{
		const glm::quat rot = animated_rot_arr_[i] * model_data_->GetBindposeRot ()[i];

		skinned_rot_arr_[i] = rot;
		skinned_pos_arr_[i] = animated_pos_arr_[i] + rot * model_data_->GetBindposePos ()[i];
	}
}


glm::vec3* MD5Controller::GetSkinnedPos () const
{
	return skinned_pos_arr_;
}


glm::quat* MD5Controller::GetSkinnedRot () const
{
	return skinned_rot_arr_;
}

