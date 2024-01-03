#pragma once
#include "Component.h"
#include "Geometry/AABB.h"
#include "GameObject.h"

class Material;
struct Mesh;

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* ownerGameObject);
	MeshRendererComponent(const MeshRendererComponent& original);
	//~MeshRendererComponent();

	void Draw();
	void Load();

	void Update() override;
	void DrawEditor() override;
	Component* Clone() override;

private:
	void LoadVBO();
	void LoadEBO();
	void LoadVAO();

	Mesh* mMesh;
	Material* material;
	AABB mAABB;
	GameObject* gameObject;
};

