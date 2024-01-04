#include "TestComponent.h"
#include "imgui.h"

TestComponent::TestComponent(GameObject* ownerGameObject, int id)
	:Component(ownerGameObject, ComponentType::TEST)
{
	componentName = "Test Component";

	componentIndex=id;
}

TestComponent::TestComponent(const TestComponent& original)
	:Component(original.GetOwner(), ComponentType::TEST)
{
	
}

void TestComponent::Draw()
{

}

void TestComponent::Load()
{
    LoadVBO();
}

void TestComponent::Update()
{
    Draw();
}

void TestComponent::DrawEditor()
{
	if (IsComponentOpen()) {
		// SIMULATED CONTENT FOR TEST PURPOSES:
		ImGui::Text("Color: (R: 255, G: 0, B: 0) (TEST)");
		ImGui::Text("Texture: DefaultTexture (TEST)");
	}
}

Component* TestComponent::Clone()
{
    return new TestComponent(*this);
}

void TestComponent::LoadVBO()
{

}

void TestComponent::LoadEBO()
{
}

void TestComponent::LoadVAO()
{
}
