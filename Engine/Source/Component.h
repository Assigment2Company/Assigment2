#pragma once

enum class ComponentType
{
	MESHRENDERER, MATERIAL
};

class Component
{
public:
	virtual	void Enable() { mIsEnabled = true; };
	virtual	void Disable() { mIsEnabled = false; }
	virtual	void Update() = 0;
	virtual	void DrawEditor() = 0; //Do not call ImGui::Begin() inside this function.
	virtual Component* Clone() = 0;

	virtual ~Component() {}

	// Get the type of the component
	ComponentType GetType() const { return mType; }

	// Set the type of the component
	void SetType(ComponentType type) { mType = type; }

private:
	ComponentType mType;
	bool mIsEnabled;
};

