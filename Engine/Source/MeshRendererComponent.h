#pragma once
#include "Component.h"
#include "Geometry/AABB.h"

class Material;
struct Mesh;

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* ownerGameObject);
	MeshRendererComponent(const MeshRendererComponent& original);


	void Reset() override;
	void Load();

	void Update() override;
	void DrawEditor() override;
	Component* Clone() override;

private:

	void RightClickPopup() override;
	Mesh* mMesh;
	Material* mMaterial;
	AABB mAABB;
};

