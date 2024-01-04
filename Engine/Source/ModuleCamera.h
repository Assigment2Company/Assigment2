#pragma once
#include "Module.h"
#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"


class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();


	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	void move(const float3& delta);
	void moveForward(bool backwards);
	void rotate(float angle, const float3& axis);

	const float4x4& GetViewProjMatrix() const { return frustum.ViewProjMatrix(); }
	const float4x4& GetInvViewProjMatrix() const { float4x4 vpMat = frustum.ViewProjMatrix(); vpMat.Inverse(); return vpMat; }
	const float3& GetCameraPos() const { return frustum.pos; }
	const float4x4& GetProjectionMatrix();
	const float4x4& GetViewMatrix();
	void ProcessInput();
	void UpdateProjectionMatrix(int screenWidth, int screenHeight);

	


private:
	float2 mousePos;
	Frustum frustum;
};

