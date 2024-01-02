#include "MaterialEditPanel.h"
#include "imgui.h"
MaterialEditPanel::MaterialEditPanel() : Panel(MATERIALEDITPANEL, true)
{
}

MaterialEditPanel::~MaterialEditPanel()
{
}

void MaterialEditPanel::Draw(int windowFlags)
{
	if (ImGui::CollapsingHeader("Diffuse"))
	{
		//ImGui::ColorEdit3("Color", (float*)&material->diffuseColor);
		ImGui::Separator();
	}
	if (ImGui::CollapsingHeader("Specular Colour"))
	{
		//ImGui::ColorEdit3("Color", (float*)&material->diffuseColor);
		ImGui::Separator();
	}	
	if (ImGui::CollapsingHeader("Shininess"))
	{
		//ImGui::ColorEdit3("Color", (float*)&material->diffuseColor);
		ImGui::Separator();
	}
}
