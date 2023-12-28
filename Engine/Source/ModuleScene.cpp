#include "ModuleScene.h"
#include "GameObject.h"

ModuleScene::ModuleScene() {
	mRoot = new GameObject("SampleScene", nullptr);
	mSelectedGameObject = mRoot;
}

ModuleScene::~ModuleScene()
{
	delete mRoot;
}

bool ModuleScene::Init()
{
	return true;
}

update_status ModuleScene::Update()
{
	mRoot->Update();
	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate()
{
	if (!mGameObjectsToDelete.empty()) {

	}
	return UPDATE_CONTINUE;
}

void ModuleScene::DrawInspector()
{
	if (mSelectedGameObject != mRoot) {
		mSelectedGameObject->DrawInspector();
	}
	
}

void ModuleScene::DrawHierarchy()
{
	mRoot->DrawHierarchy(mSelectedGameObject->GetID());
}

void ModuleScene::SetSelectedObject(GameObject* gameObject)
{
	mSelectedGameObject = gameObject;
}

void ModuleScene::DeleteGameObject(GameObject* gameObject){

	for (auto child : gameObject->GetChildren()) {

		std::_Vector_iterator<std::_Vector_val<std::_Simple_types<unsigned int>>> idIterator = std::find(mGameObjectsToDelete.begin(), mGameObjectsToDelete.end(), child->GetID());
		if (idIterator != mGameObjectsToDelete.end()) {
			mGameObjectsToDelete.erase(idIterator);
		}
	}

	if (!mGameObjectsToDelete.empty() && !gameObject->GetChildren().empty()) {
		for (auto child : gameObject->GetChildren()) {
			DeleteGameObject(gameObject);
		}
	}
}
