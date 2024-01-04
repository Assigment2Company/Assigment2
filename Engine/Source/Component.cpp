#include "Component.h"
#include "imgui.h"
#include "GameObject.h"

Component::Component(GameObject* owner, ComponentType type) : mOwner(owner), mType(type), mIsEnabled(true)
{
}

bool Component::IsComponentOpen()
{
	ImGui::PushID(componentIndex);
		std::string popupID = componentName + std::to_string(componentIndex);
		bool isOpen = ImGui::CollapsingHeader(popupID.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

	//bool isOpen = ImGui::CollapsingHeader(componentName, ImGuiTreeNodeFlags_DefaultOpen);

	if (GetOwner()) {
		GetOwner()->DeletePopup(this);
	}

	ImGui::PopID();

	//componentIndex++;

	return isOpen;
	//**************************************************************

	/*
	bool isOpen = ImGui::CollapsingHeader(componentName, ImGuiTreeNodeFlags_DefaultOpen);

	if (GetOwner()) {
		GetOwner()->DeletePopup(this);
	}

	return isOpen;
	*/
}
