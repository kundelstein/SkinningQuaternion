#include "md5_anim_data.h"
#include <fstream>


MD5AnimData::MD5AnimData (const std::string& file_name)
{
	frame_skeleton_arr_ = NULL;
	frame_num_ = 0;



	struct MD5AnimJoint
	{
		int					parent_id_;
		int					flags_;
		int					start_frame_id_;
	};


	struct MD5Bound
	{
		glm::vec3			bb_min_;
		glm::vec3			bb_max_;
	};


	std::ifstream file (file_name);
	assert (file.is_open ());


	std::streampos initpos = file.tellg ();
	file.seekg (0, std::ios::end);
	std::streampos filelength = file.tellg ();
	file.seekg (initpos);

	std::string param, junk;
	file >> param;


	int					animated_component_num = 0;
	MD5AnimJoint*		joint_info_arr = NULL;
	MD5Bound*			bound_arr = NULL;
	MD5Joint*			base_frame_arr = NULL;


	while (!file.eof ())
	{
		if (param == "MD5Version")
		{
			int version;

			file >> version;
			assert (version == 10);

			printf ("MD5Version: %i\n", version);
		}
		else if (param == "numFrames")
		{
			file >> frame_num_;
			bound_arr = new MD5Bound[frame_num_];
			frame_skeleton_arr_ = new MD5Joint*[frame_num_];

			printf ("numFrame: %i\n", frame_num_);
		}
		else if (param == "numJoints")
		{
			file >> joint_num_;
			joint_info_arr = new MD5AnimJoint[joint_num_];
			base_frame_arr = new MD5Joint[joint_num_];

			printf ("numJoints: %i\n", joint_num_);
		}
		else if (param == "frameRate")
		{
			file >> frame_rate_;
			printf ("frameRate: %i\n", frame_rate_);
		}
		else if (param == "numAnimatedComponents")
		{
			file >> animated_component_num;
			printf ("numAnimatedComponents: %i\n", animated_component_num);
		}
		else if (param == "hierarchy")
		{
			file >> junk;
			for (int i = 0; i < joint_num_; ++i)
			{
				MD5AnimJoint& joint = joint_info_arr[i];

				file >> junk >> joint.parent_id_ >> joint.flags_ >> joint.start_frame_id_;
				file.ignore (filelength, '\n');
			}
			file >> junk;
		}
		else if (param == "bounds")
		{
			file >> junk;
			for (int i = 0; i < frame_num_; ++i)
			{
				MD5Bound& bound = bound_arr[i];

				file >> junk;
				file >> bound.bb_min_[0] >> bound.bb_min_[1] >> bound.bb_min_[2];
				file >> junk >> junk;
				file >> bound.bb_max_[0] >> bound.bb_max_[1] >> bound.bb_max_[2];
				file >> junk;
			}
			file >> junk;
		}
		else if (param == "baseframe")
		{
			file >> junk;
			for (int i = 0; i < joint_num_; ++i)
			{
				MD5Joint& joint = base_frame_arr[i];

				file >> junk;
				file >> joint.pos_[0] >> joint.pos_[1] >> joint.pos_[2];
				file >> junk >> junk;
				file >> joint.rot_[0] >> joint.rot_[1] >> joint.rot_[2];
				file >> junk;
			}
			file >> junk;
		}
		else if (param == "frame")
		{
			int frame_id;

			file >> frame_id >> junk;

			float* frame_data_arr = new float[animated_component_num];

			for (int i = 0; i < animated_component_num; ++i)
			{
				file >> frame_data_arr[i];
			}

			MD5Joint* const frameskel_joint_arr = new MD5Joint[joint_num_];
			frame_skeleton_arr_[frame_id] = frameskel_joint_arr;


			const int joint_num = joint_num_;

			for (int i = 0; i < joint_num; ++i)
			{
				unsigned int j = 0;

				const MD5AnimJoint& jointinfo = joint_info_arr[i];

				MD5Joint& baseanim = frameskel_joint_arr[i];

				baseanim = base_frame_arr[i];

				if (jointinfo.flags_ & 1)
				{
					baseanim.pos_[0] = frame_data_arr[jointinfo.start_frame_id_ + j++];
				}
				if (jointinfo.flags_ & 2)
				{
					baseanim.pos_[1] = frame_data_arr[jointinfo.start_frame_id_ + j++];
				}
				if (jointinfo.flags_ & 4)
				{
					baseanim.pos_[2] = frame_data_arr[jointinfo.start_frame_id_ + j++];
				}
				if (jointinfo.flags_ & 8)
				{
					baseanim.rot_[0] = frame_data_arr[jointinfo.start_frame_id_ + j++];
				}
				if (jointinfo.flags_ & 16)
				{
					baseanim.rot_[1] = frame_data_arr[jointinfo.start_frame_id_ + j++];
				}
				if (jointinfo.flags_ & 32)
				{
					baseanim.rot_[2] = frame_data_arr[jointinfo.start_frame_id_ + j++];
				}

				if (jointinfo.flags_ & (8 | 16 | 32))
				{
					baseanim.rot_.w = quat_w (baseanim.rot_);
				}


				if (jointinfo.parent_id_ >= 0)
				{
					const MD5Joint& parentjoint = frameskel_joint_arr[jointinfo.parent_id_];
					const MD5Joint& baseparent = parentjoint;

					baseanim.pos_ = baseparent.pos_ + baseparent.rot_ * baseanim.pos_;
					baseanim.rot_ = baseparent.rot_ * baseanim.rot_;
				}
			}

			delete[] frame_data_arr;

			file >> junk;
		}

		file >> param;
	}


	frame_duration_ = 1.0f / float (frame_rate_);
	anim_duration_ = frame_duration_ * float (frame_num_);

	printf ("finished reading md5 anim\n");


	delete[] joint_info_arr;
	delete[] bound_arr;
	delete[] base_frame_arr;
}


MD5AnimData::~MD5AnimData ()
{
	if (frame_skeleton_arr_)
	{
		for (int i = 0; i < frame_num_; ++i)
		{
			delete[] frame_skeleton_arr_[i];
		}
		delete[] frame_skeleton_arr_;
	}
}


int MD5AnimData::GetJointNum () const
{
	return joint_num_;
}


int MD5AnimData::GetFrameNum () const
{
	return frame_num_;
}


float MD5AnimData::GetDuration () const
{
	return anim_duration_;
}


int MD5AnimData::GetFrameRate () const
{
	return frame_rate_;
}


MD5Joint** MD5AnimData::GetFrameSkeleton () const
{
	return frame_skeleton_arr_;
}

