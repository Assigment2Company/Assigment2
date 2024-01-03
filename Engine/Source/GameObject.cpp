#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Component.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "InspectorPanel.h"
#include "imgui.h"
#include <algorithm>

//CLASSES FOR TESTING PURPOSE:
#include "MeshRendererComponent.h"
#include "TestComponent.h"

GameObject::GameObject(GameObject* parent)
	:mID((new LCG())->Int()), mName("GameObject"), mParent(parent),
	mIsRoot(parent == nullptr), mIsEnabled(true), mWorldTransformMatrix(float4x4::zero),
	mLocalTransformMatrix(float4x4::zero), mPosition(float3::zero), mScale(float3::zero),
	mRotation(Quat::identity)
{
	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

	AddSufix();
}

GameObject::GameObject(const GameObject& original)
	:mID((new LCG())->Int()), mName(original.mName), mParent(original.mParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix), mPosition(original.mPosition), mScale(original.mScale),
	mRotation(original.mRotation)
{

	AddSufix();

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}

	for (auto component : original.mComponents) {
		mComponents.push_back(component->Clone());
	}
}

GameObject::GameObject(const GameObject& original, GameObject* newParent)
	:mID((new LCG())->Int()), mName(original.mName), mParent(newParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix), mPosition(original.mPosition), mScale(original.mScale),
	mRotation(original.mRotation)
{

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}

	for (auto component : original.mComponents) {
		mComponents.push_back(component->Clone());
	}
}

GameObject::GameObject(const char* name, GameObject* parent)
	:mID((new LCG())->Int()), mName(name), mParent(parent),
	mIsRoot(parent == nullptr), mIsEnabled(true), mWorldTransformMatrix(float4x4::zero),
	mLocalTransformMatrix(float4x4::zero), mPosition(float3::zero), mScale(float3::zero),
	mRotation(Quat::identity)
{

	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

}

GameObject::~GameObject()
{
	for (GameObject* gameObject : mChildren) {
		delete gameObject;
		gameObject = nullptr;
	}
	mChildren.clear();
	for (Component* component : mComponents) {
		delete component;
		component = nullptr;
	}
	mComponents.clear();

}

void GameObject::RecalculateMatrices()
{
	mLocalTransformMatrix = float4x4::FromTRS(mPosition, mRotation, mScale);

	mWorldTransformMatrix = mParent->GetWorldTransform() * mLocalTransformMatrix;

	for (size_t i = 0; i < mChildren.size(); i++) {
		mChildren[i]->RecalculateMatrices();
	}

}

void GameObject::Update()
{
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		mComponents[i]->Update();
	}

	for (size_t i = 0; i < mChildren.size(); i++) {
		mChildren[i]->Update();
	}
}

void GameObject::DeleteChild(GameObject* child)
{
	auto childIterator = std::find(mChildren.begin(), mChildren.end(), child);
	mChildren.erase(childIterator);
	delete child;
	child = nullptr;
}

void GameObject::SetRotation(const Quat& rotation)
{
	mRotation = rotation;
	RecalculateMatrices();
}

void GameObject::SetPosition(const float3& position)
{
	mPosition = position;
	RecalculateMatrices();
}

void GameObject::SetScale(const float3& scale)
{
	mScale = scale;
	RecalculateMatrices();
}

void GameObject::DrawInspector() {
	ImGui::Text(mName.c_str());
	DrawTransform();

	componentIndex = 0; // Generate an unique identifier for each component

	for (Component* component : mComponents) {
		component->DrawEditor();
	}

	ImGui::Separator();
	AddComponentButton();
}

void GameObject::DrawHierarchy(const int selected)
{
	bool nodeOpen = true;
	if (!mIsRoot) {
		ImGui::Separator();
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				const GameObject* movedObject = (const GameObject*)payload->Data;
				movedObject->mParent->MoveChild(movedObject->GetID(), this->mParent, mID);
			}
			ImGui::EndDragDropTarget();
		}
		ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (mID == selected)
			baseFlags |= ImGuiTreeNodeFlags_Selected;
		if (mChildren.size() == 0) {
			baseFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}
		nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)mID, baseFlags, mName.c_str()) && (mChildren.size() > 0);
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
			App->GetScene()->SetSelectedObject(this);
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && !ImGui::IsItemToggledOpen()) {
			App->GetScene()->SetSelectedObject(this);
		}
		OnRightClick();
		/*****Begin Drag & Drop Code*******/
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("_TREENODE", this, sizeof(*this));

			ImGui::Text(mName.c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				const GameObject* movedObject = (const GameObject*)payload->Data;
				movedObject->mParent->MoveChild(movedObject->GetID(), this);
			}
			ImGui::EndDragDropTarget();
		}
		/*****End Drag & Drop Code*******/
	}
	if (nodeOpen) {
		for (auto child : mChildren) {
			child->DrawHierarchy(selected);
		}
	
		if (!mIsRoot) {
			ImGui::TreePop(); 
		}
	}
	if (mIsRoot) { // Dragging something to this Separator will move it to the end of root
		ImGui::Separator();
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				const GameObject* movedObject = (const GameObject*)payload->Data;
				movedObject->mParent->MoveChild(movedObject->GetID(), this);
			}
			ImGui::EndDragDropTarget();
		}
	}
	//OnLeftClick();

}

void GameObject::OnLeftClick() {
}

void GameObject::OnRightClick() {
	ImGui::PushID(mID);
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {

		ImGui::OpenPopup("OptionsGO");
	}
	if (ImGui::BeginPopup("OptionsGO")) {
		if (ImGui::Selectable("Create GameObject")) {
				GameObject* gameObject = new GameObject(this);
				AddChild(gameObject);
				App->GetScene()->SetSelectedObject(gameObject);
		}

		if (!(App->GetScene()->GetSelectedGameObject()->IsRoot())) {
			if (ImGui::Selectable("Duplicate")) {
				GameObject* gameObject = new GameObject(*this);
				mParent->AddChild(gameObject);
				App->GetScene()->SetSelectedObject(gameObject);
			}
		}

		if (!(App->GetScene()->GetSelectedGameObject()->IsRoot())) {
			if (ImGui::Selectable("Delete")) {
				App->GetScene()->AddGameObjectToDelete(GetID());
				App->GetScene()->SetSelectedObject(App->GetScene()->GetRoot());
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
}

void GameObject::AddChild(GameObject* child, const int aboveThisId)
{
	child->mParent = this;
	bool inserted = false;
	if (aboveThisId != 0) {
		for (auto it = mChildren.cbegin(); it != mChildren.cend(); ++it) {
			if ((*it)->GetID() == aboveThisId)
			{
				mChildren.insert(it, child);
				inserted = true;
				break;
			}
		}
	}
	if (!inserted) {
		mChildren.push_back(child);
	}
}

void GameObject::MoveChild(const int id, GameObject* newParent, const int aboveThisId)
{
	if (mID == newParent->mID) { //Movement inside same object
		if (id == aboveThisId) { return; }
		std::vector<GameObject*>::iterator itMovedObject;
		std::vector<GameObject*>::iterator itTargetPosition = mChildren.end();
		for (auto it = mChildren.begin(); it != mChildren.cend(); ++it) {
			if ((*it)->GetID() == id) {
				itMovedObject = it;
			}
			else if ((*it)->GetID() == aboveThisId) {
				itTargetPosition = it;
			}
		}
		if (itMovedObject != mChildren.end()) {
			if (itMovedObject < itTargetPosition) {
				if (itMovedObject + 1 == itTargetPosition) { return; }
				std::rotate(itMovedObject, itMovedObject + 1, itTargetPosition);
			}
			else if (itMovedObject > itTargetPosition) {
				std::rotate(itTargetPosition, itMovedObject, itMovedObject + 1);
			}
		}
	}
	else {
		for (auto it = mChildren.cbegin(); it != mChildren.cend(); ++it)
		{
			if ((*it)->GetID() == id)
			{
				newParent->AddChild(*it, aboveThisId);
				mChildren.erase(it);
				break;
			}
		}
	}
}

void GameObject::AddSufix()
{
	bool found = true;
	int count = 1;
	int last_pos = -1;
	while (found) {
		std::string str = " (" + std::to_string(count) + ')';
		int pos = std::string::npos;
		for (auto gameObject : mParent->mChildren)
		{
			if (pos == -1) {
				//pos = gameObject->mName.find(str, gameObject->mName.size() - 4);
				pos = gameObject->mName.find(mName + str);
			}

		}

		if (pos == std::string::npos) {
			if (mParent->mChildren.size() > 0) {
				mName += str;
			}
			found = false;
		}
		else {
			count++;
			last_pos = pos;
		}

	}


}

void GameObject::DrawTransform() {
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		bool modifiedTransform = false;
		if (ImGui::BeginTable("transformTable", 2)) {
			ImGui::TableNextRow();
			ImGui::PushID(mID);
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Position");
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(1) / 4);
			modifiedTransform = modifiedTransform || ImGui::InputFloat("X", &mPosition.x);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Y", &mPosition.y);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Z", &mPosition.z);
			ImGui::PopItemWidth();
			ImGui::PopID();

			ImGui::TableNextRow();
			ImGui::PushID(mID+1);
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Rotation");
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(1) / 4);
			modifiedTransform = modifiedTransform || ImGui::InputFloat("X", &mRotation.x);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Y", &mRotation.y);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Z", &mRotation.z);
			ImGui::PopItemWidth();
			ImGui::PopID();

			ImGui::TableNextRow();
			ImGui::PushID(mID+2);
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Scale");
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(1) / 4);
			modifiedTransform = modifiedTransform || ImGui::InputFloat("X", &mScale.x);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Y", &mScale.y);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Z", &mScale.z);
			ImGui::PopItemWidth();
			ImGui::PopID();

			if (modifiedTransform) {
				RecalculateMatrices();
			}
		}
		ImGui::EndTable();
	}
}

void GameObject::AddComponentButton() {
	// Calculate the width of the current ImGui window
	float windowWidth = ImGui::GetWindowWidth();

	// Calculate the X position to center the button
	float buttonWidth = 150.0f; // Desired width for the button
	float posX = (windowWidth - buttonWidth) * 0.5f;

	// Sets the X position of the cursor to center the button
	ImGui::SetCursorPosX(posX);

	bool hasMeshRendererComponent = false;
	bool hasMaterialComponent = false;

	// Check if the components exist in mComponents
	for (Component* component : mComponents) {
		if (component->GetType() == ComponentType::MESHRENDERER) {
			hasMeshRendererComponent = true;
		}
		else if (component->GetType() == ComponentType::TEST) {
			hasMaterialComponent = true;
		}

		// If both types of components are found, no need to continue iterating
		if (hasMeshRendererComponent && hasMaterialComponent) {
			break;
		}
	}

	// Draw the button centered if components are missing
	if (!hasMeshRendererComponent || !hasMaterialComponent) {
		if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
			// Popup Menu
			ImGui::OpenPopup("AddComponentPopup");
		}
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		// Draw the menu Item if the component is available
		if (!hasMeshRendererComponent && ImGui::MenuItem("Mesh Renderer")) {
			//**********************************************************
			//Component* newComponent = CreateComponent(ComponentType::MESHRENDERER);
			CreateComponent(ComponentType::MESHRENDERER);
		}
		if (!hasMaterialComponent && ImGui::MenuItem("Test")) {
			//**********************************************************
			//Component* newComponent = CreateComponent(ComponentType::MATERIAL);
			CreateComponent(ComponentType::TEST);
		}

		ImGui::EndPopup();
	}
}

//**********************************************************
//Component* GameObject::CreateComponent(ComponentType type) {
void GameObject::CreateComponent(ComponentType type) {
	Component* newComponent = nullptr;

	// Creating the component according to its type
	switch (type) {
		case ComponentType::MESHRENDERER:
			//ONLY FOR TEST PURPOSE
			newComponent = new MeshRendererComponent(this);
			break;
		case ComponentType::TEST:
			//ONLY FOR TEST PURPOSE        
			newComponent = new TestComponent(this);
			break;
		default:
			break;
	}

	if (newComponent) {
		mComponents.push_back(newComponent);
	}

	//return newComponent;
}

void GameObject::DeletePopup(Component* component, int headerPosition) {
	// Create a unique identifier for the Delete Popup
	ImGui::PushID(componentIndex); // Work correctly without this function, its necessary?

	std::string popupID = "ComponentOptions_" + std::to_string(componentIndex);

	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();

	// Set header position detection
	min.y -= ImGui::GetStyle().FramePadding.y + headerPosition;
	max.y += ImGui::GetStyle().FramePadding.y - headerPosition;

	// Open Delete Popup with righ click when header is open
	if (ImGui::IsMouseHoveringRect(min, max) && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup(popupID.c_str());
	}

	// Open Delete Popup with right click when header is close
	if (ImGui::BeginPopupContextItem(popupID.c_str())) {
		ImGui::OpenPopup(popupID.c_str());
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup(popupID.c_str())) {
		if (ImGui::MenuItem("Delete Component")) {
			// Delete currently selected component
			RemoveComponent(component);
			ImGui::CloseCurrentPopup(); // Close popup menu after deleting
		}
		ImGui::EndPopup();
	}

	ImGui::PopID(); // Reset ID for next component PushID
	componentIndex++;
}

void GameObject::RemoveComponent(Component* component) {
	auto it = std::find(mComponents.begin(), mComponents.end(), component);
	if (it != mComponents.end()) {
		mComponents.erase(it);
		delete component; // Remove the component completely
	}
}