#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"

#include "ModuleCamera.h"
#include "ModuleRenderTest.h"
#include "ModuleDebugDraw.h"


Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules[0] = window = new ModuleWindow();
	modules[1] = render = new ModuleOpenGL();
	modules[2] = input = new ModuleInput();
	modules[3] = scene = new ModuleScene();
	modules[4] = test = new ModuleRenderTest();
	modules[5] = editor = new ModuleEditor();
	modules[6] = camera = new ModuleCamera();
	modules[7] = debugDraw = new ModuleDebugDraw();
}

Application::~Application()
{
	for(int i = 0; i != NUM_MODULES; ++i)
    {
        delete modules[i];
		modules[i] = nullptr;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(int i = 0; i != NUM_MODULES; ++i)
		ret = modules[i]->Init();

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	for (int i = 0; i != NUM_MODULES; ++i)
		ret = modules[i]->PreUpdate();

	for (int i = 0; i != NUM_MODULES; ++i)
		ret = modules[i]->Update();

	for (int i = 0; i != NUM_MODULES; ++i)
		ret = modules[i]->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (int i = 0; i != NUM_MODULES; ++i)
		ret = modules[i]->CleanUp();

	return ret;
}
