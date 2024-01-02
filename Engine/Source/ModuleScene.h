#pragma once
#include "Module.h"

class GameObject;

class ModuleScene : public Module
{
public:

	ModuleScene();
	~ModuleScene();
	bool Init() override;
	update_status Update() override;

	void DrawInspector();
	void DrawHierarchy();

	GameObject* GetRoot() const { return mRoot; }
	GameObject* GetSelectedGameObject() const { return mSelectedGameObject; }

	void SetSelectedObject(GameObject* gameObject);

private:
	GameObject* mRoot;
	GameObject* mSelectedGameObject;
};
