#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Component.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "InspectorPanel.h"
#include "imgui.h"
#include <algorithm>
#include "MeshRendererComponent.h"
#include "TestComponent.h"
#include <MathFunc.h>
#include "TransformOps.h"

GameObject::GameObject(GameObject* parent)
	:mID(LCG().Int()), mName("GameObject"), mParent(parent),
	mIsRoot(parent == nullptr)
{
	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

	AddSuffix();
}

GameObject::GameObject(const GameObject& original)
	:mID(LCG().Int()), mName(original.mName), mParent(original.mParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix)

{

	AddSuffix();

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}

	for (auto component : original.mComponents) {
		mComponents.push_back(component->Clone(this));
	}
}

GameObject::GameObject(const GameObject& original, GameObject* newParent)
	:mID(LCG().Int()), mName(original.mName), mParent(newParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix)
{

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}

	for (auto component : original.mComponents) {
		mComponents.push_back(component->Clone(this));
	}
}

GameObject::GameObject(const char* name, GameObject* parent)
	:mID(LCG().Int()), mName(name), mParent(parent),
	mIsRoot(parent == nullptr)
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


/*template<class T>
T* GameObject::GetComponent() {
	T& GameObject::GetComponent() {
		for (auto&& component : components) {
			if (component->IsClassType(T::Type))
				return *static_cast<T*>(component.get());
		}

		return *std::unique_ptr<T>(nullptr);
	}
}
*/
Component* GameObject::GetComponent(ComponentType type)
{
	for (auto component : mComponents) {
		if (component->GetType() == type) {
			return component;
		}
	}
	return nullptr;
}

void GameObject::RecalculateMatrices()
{
	//mLocalTransformMatrix = float4x4::FromTRS(mPosition, Quat::FromEulerXYZ(DegToRad(mRotation.x), DegToRad(mRotation.y), DegToRad(mRotation.z)), mScale);

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

	DeleteComponents();
}

void GameObject::ResetTransform()
{
	SetPosition(float3::zero);
	SetRotation(float3::zero);
	SetScale(float3::one);
}

void GameObject::DeleteChild(GameObject* child)
{
	auto childIterator = std::find(mChildren.begin(), mChildren.end(), child);
	mChildren.erase(childIterator);
	delete child;
	child = nullptr;
}

void GameObject::AddComponentToDelete(Component* component)
{
	mComponentsToDelete.push_back(component);
}


void GameObject::SetRotation(const float3& rotation)
{
	//mLocalTransformMatrix.SetRotatePart(Quat::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z)));
	
	float3 translation, scale;
	Quat auxRotation;
	mLocalTransformMatrix.Decompose(translation, auxRotation, scale);
	
	
	mLocalTransformMatrix = float4x4::FromTRS(translation, Quat::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z)), scale);
	
	RecalculateMatrices();
	
}

void GameObject::SetPosition(const float3& position)
{
	mLocalTransformMatrix.SetTranslatePart(position);
	RecalculateMatrices();
}

void GameObject::SetScale(const float3& scale)
{
	
	//mLocalTransformMatrix.RemoveScale();
	//ScaleOp scaleOp = mLocalTransformMatrix.Scale(scale);
	//mLocalTransformMatrix =  mLocalTransformMatrix * scaleOp;
	float3 translation, auxScale;
	Quat rotation;
	mLocalTransformMatrix.Decompose(translation, rotation, auxScale);
	if (!rotation.IsFinite()) {
		rotation = Quat::identity;
	}


	mLocalTransformMatrix = float4x4::FromTRS(translation, rotation, scale);
	 
	//mLocalTransformMatrix = mLocalTransformMatrix * float4x4::Scale(scale);
	RecalculateMatrices();
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

	child->mLocalTransformMatrix = mWorldTransformMatrix.Inverted() * child->mWorldTransformMatrix;

	if (!inserted) {
		mChildren.push_back(child);
	}
}

GameObject* GameObject::RemoveChild(const int id)
{
	GameObject* movedObject = nullptr;
	std::vector<GameObject*>::iterator itTargetPosition = mChildren.end();
	for (auto it = mChildren.begin(); it != mChildren.cend(); ++it) {
		if ((*it)->GetID() == id) {
			movedObject = *it;
			mChildren.erase(it);
			break;
		}
	}
	return movedObject;

}

void GameObject::AddSuffix()
{
	bool found = true;
	int count = 1;
	int last_pos = -1;
	while (found) {
		std::string str = " (" + std::to_string(count) + ')';
		int pos = std::string::npos;

		std::string nameWithSufix = mName + str;
		for (auto gameObject : mParent->mChildren)
		{
			if (pos == -1) {
				//pos = gameObject->mName.find(str, gameObject->mName.size() - 4);
				pos = gameObject->mName.find(nameWithSufix);
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

void GameObject::CreateComponent(ComponentType type) {
	Component* newComponent = nullptr;

	switch (type) {
		case ComponentType::MESHRENDERER:
			newComponent = new MeshRendererComponent(this);
			break;
		case ComponentType::TEST:    
			newComponent = new TestComponent(this);
			break;
		default:
			break;
	}

	if (newComponent) {
		mComponents.push_back(newComponent);
	}
}

void GameObject::DeleteComponents() {
	for (auto component : mComponentsToDelete)
	{
		auto it = std::find(mComponents.begin(), mComponents.end(), component);
		if (it != mComponents.end()) {
			mComponents.erase(it);
			delete component;
			component = nullptr;
		}
	}
}

/******************************************************************************
 ***						GUI DRAWING FUNCTIONS							***
 ******************************************************************************/


void GameObject::DragAndDropSource()
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("_TREENODE", this, sizeof(*this));

		ImGui::Text(mName.c_str());
		ImGui::EndDragDropSource();
	}

}

void GameObject::DragAndDropTarget(bool reorder) {
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
		{
			bool isParent = false;
			const GameObject* movedObject = (const GameObject*)payload->Data;

			GameObject* parent = mParent;

			while (parent != nullptr) {
				if (parent->mID == movedObject->mID) {
					isParent = true;
				}
				parent = parent->mParent;
			}

			if (!isParent) {
				GameObject* pMovedObject = movedObject->mParent->RemoveChild(movedObject->GetID());
				if (reorder) { mParent->AddChild(pMovedObject, mID); }
				else { AddChild(pMovedObject); }
			}

		}
		ImGui::EndDragDropTarget();
	}
}

void GameObject::DrawTransform() {
	bool headerOpen = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
	
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {

		ImGui::OpenPopup("TransformOptions");
	}
	if (ImGui::BeginPopup("TransformOptions")) {
		if (ImGui::Selectable("Reset")) {
			ResetTransform();
		}
		ImGui::EndPopup();
	}

	if (headerOpen) {
		if (ImGui::BeginTable("transformTable", 4)) {
			//ImGui::TableSetupColumn("columns", 0 , -FLT_MIN);
			
			float3 position = mLocalTransformMatrix.TranslatePart();
			float3 rotation = RadToDeg(mLocalTransformMatrix.ToEulerXYZ());
			float3 scale = mLocalTransformMatrix.GetScale();

			ImGui::TableNextRow();
			ImGui::PushID(mID);
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Text("Position");
			ImGui::PopItemWidth();
			
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("X");
			ImGui::SameLine();
			if (ImGui::DragFloat("##X", &position.x, 0.05f))
			{
				SetPosition(position);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Y");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Y", &position.y, 0.05f))
			{
				SetPosition(position);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Z");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Z", &position.z, 0.05f))
			{
				SetPosition(position);
			}
			ImGui::PopItemWidth();

			ImGui::PopID();


			ImGui::TableNextRow();
			ImGui::PushID(mID + 1);
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Text("Rotation");
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("X");
			ImGui::SameLine();
			if (ImGui::DragFloat("##X", &rotation.x, 0.05f))
			{
				SetRotation(rotation);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Y");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Y", &rotation.y, 0.05f))
			{
				SetRotation(rotation);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Z");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Z", &rotation.z, 0.05f))
			{
				SetRotation(rotation);
			}
			ImGui::PopItemWidth();

			ImGui::PopID();


			ImGui::TableNextRow();
			ImGui::PushID(mID + 2);

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Text("Scale");
			
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("X");
			ImGui::SameLine();
			if (ImGui::DragFloat("##X", &scale.x, 0.05f, 0.0f, 100000.0f))
			{
				SetScale(scale);
			}

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Y");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Y", &scale.y, 0.05f, 0.0f, 100000.0f))
			{
				SetScale(scale);
			}

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Z");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Z", &scale.z, 0.05f, 0.0f, 100000.0f))
			{
				SetScale(scale);
			}
			ImGui::PopItemWidth();

			ImGui::PopID();
			
		}
		ImGui::EndTable();
	}

}

void GameObject::AddComponentButton() {
	float windowWidth = ImGui::GetWindowWidth();
	float buttonWidth = 150.0f; // Desired width for the button
	float posX = (windowWidth - buttonWidth) * 0.5f;

	ImGui::SetCursorPosX(posX);

	bool hasMeshRendererComponent = false;
	bool hasMaterialComponent = false;

	for (Component* component : mComponents) {
		if (component->GetType() == ComponentType::MESHRENDERER) {
			hasMeshRendererComponent = true;
		}
		else if (component->GetType() == ComponentType::TEST) {
			hasMaterialComponent = true;
		}

		if (hasMeshRendererComponent && hasMaterialComponent) {
			break;
		}
	}

	if (!hasMeshRendererComponent || !hasMaterialComponent) {
		if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
			ImGui::OpenPopup("AddComponentPopup");
		}
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		if (!hasMeshRendererComponent && ImGui::MenuItem("Mesh Renderer")) {
			CreateComponent(ComponentType::MESHRENDERER);
		}
		if (!hasMaterialComponent && ImGui::MenuItem("Test")) {
			CreateComponent(ComponentType::TEST);
		}

		ImGui::EndPopup();
	}
}

void GameObject::DrawInspector() {
	char nameArray[100];
	strcpy_s(nameArray, mName.c_str());
	ImGui::PushID(mID);
	ImGui::InputText("##rename", nameArray, IM_ARRAYSIZE(nameArray));
	ImGui::PopID();
	mName = nameArray;
	DrawTransform();

	componentIndex = 0;

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
		DragAndDropTarget(true);
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
		DragAndDropSource();
	}
	else {
		nodeOpen = ImGui::CollapsingHeader(mName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
	}
	DragAndDropTarget();
	if (nodeOpen) {
		for (auto child : mChildren) {
			child->DrawHierarchy(selected);
		}

		if (!mIsRoot) {
			ImGui::TreePop();
		}
	}
	if (mIsRoot) {
		ImGui::Separator();
		DragAndDropTarget();
	}

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

		if (!mIsRoot) {
			if (ImGui::Selectable("Duplicate")) {
				GameObject* gameObject = new GameObject(*this);
				//mParent->AddChild(gameObject);
				App->GetScene()->AddGameObjectToDuplicate(gameObject);
				App->GetScene()->SetSelectedObject(gameObject);
			}
		}

		if (!mIsRoot) {
			if (ImGui::Selectable("Delete")) {
				App->GetScene()->AddGameObjectToDelete(this);
				App->GetScene()->SetSelectedObject(App->GetScene()->GetRoot());
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
}