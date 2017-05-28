#include "scene.h"
#include "renderer.h"
#include "shader.h"
#include "md5_example.h"
#include "md5_model_data.h"
#include "md5_anim_data.h"
#include "md5_controller.h"


Scene::Scene ()
{
	renderer_ = new Renderer ();
	shader_ = new Shader ("skinning.vert", "skinning.frag");
	model_ = new MD5Example ("Assets/Models/boblampclean.md5mesh", "Assets/Animations/boblampclean.md5anim");

	shader_mat_arr_ = new ShaderMatrices ();
	shader_mat_arr_->world_ = glm::rotate (glm::mat4 (1), glm::radians (-90.0f), glm::vec3 (1, 0, 0));
	shader_mat_arr_->view_ = glm::lookAt (glm::vec3 (0.0f, 25.0f, 200.0f), glm::vec3 (0.0f, 25.0f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f));
	shader_mat_arr_->proj_ = renderer_->GetProjectionMatrix ();
}


Scene::~Scene ()
{
	delete shader_mat_arr_;
	delete model_;
	delete shader_;
	delete renderer_;
}


void Scene::Loop ()
{
	bool want_exit = false;

	while (!want_exit)
	{
		Update (1.0f / 60.0f);
		Render ();

		want_exit = renderer_->ProcessCheckClose ();
	}
}



void Scene::Update (const float dt)
{
	renderer_->PollEvents ();

	model_->Update (dt);
}


void Scene::Render ()
{
	renderer_->Buffer_Clear ();

	shader_->SetShader ();
	shader_->SetShaderParameters (shader_mat_arr_, model_->anim_ctrl_->GetSkinnedPos (), model_->anim_ctrl_->GetSkinnedRot ());

	model_->Render ();

	renderer_->SwapBuffer ();
}

