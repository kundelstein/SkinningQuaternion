#pragma once

#include "helpers.h"


class Renderer;
class Shader;
class MD5Example;
struct ShaderMatrices;


class Scene
{
public:

	Scene ();
	~Scene ();

	void		Loop ();

	void		Update (const float dt);
	void		Render ();

public:

	Renderer*	renderer_;
	Shader* shader_;
	MD5Example* model_;
	ShaderMatrices* shader_mat_arr_;
};

