#pragma once
#include "Component.h"
#include "Geometry/AABB.h"

class Material;
struct Mesh;
class TestMaterial : public Component
{
public:
	TestMaterial();
	TestMaterial(const TestMaterial& original);
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



};

