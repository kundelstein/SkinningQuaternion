#include "md5_example.h"
#include "md5_model_data.h"
#include "md5_anim_data.h"
#include "md5_controller.h"


MD5Example::MD5Example (const char* filename1, const char* filename2)
{
	model_data_ = new MD5ModelData (filename1);
	anim_data_ = new MD5AnimData (filename2);

	anim_ctrl_ = new MD5Controller (model_data_, anim_data_);

	model_data_->CreateBuffers ();
	model_data_->CreateTextures ();
}


MD5Example::~MD5Example ()
{
	delete anim_ctrl_;
	delete anim_data_;
	delete model_data_;
}


void MD5Example::Update (float dt)
{
	anim_ctrl_->Update (dt);
}

void MD5Example::Render ()
{
	model_data_->Render ();
}

