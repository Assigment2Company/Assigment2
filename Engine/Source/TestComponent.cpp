#include "TestComponent.h"
#include "imgui.h"

TestComponent::TestComponent() {}

TestComponent::TestComponent(GameObject* ownerGameObject) {
	gameObject = ownerGameObject;
}

TestComponent::TestComponent(const TestComponent& original)
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
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
		// SIMULATED CONTENT FOR TEST PURPOSES:
		ImGui::Text("Color: (R: 255, G: 0, B: 0) (TEST)");
		ImGui::Text("Texture: DefaultTexture (TEST)");
	}

	if (gameObject) {
		// Activate Delete Popup for every Component
		gameObject->DeletePopup(this, 38);
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
