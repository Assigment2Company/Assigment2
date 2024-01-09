#ifndef _MODULE_CAMERA_H_
#define _MODULE_CAMERA_H_

#include "Module.h"
#include "MathGeoLibFwd.h"
#include "Math/float2.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"


class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();


	bool Init() override;
	update_status Update() override;
	void Move(const float3& delta);
	void MoveForward(bool backwards);
	void Rotate(float angle, const float3& axis);

	const float4x4& GetViewProjMatrix() const { return frustum.ViewProjMatrix(); }
	const float4x4& GetInvViewProjMatrix() const { float4x4 vpMat = frustum.ViewProjMatrix(); vpMat.Inverse(); return vpMat; }
	const float3& GetCameraPos() const { return frustum.pos; }
	void ProcessInput();
	void UpdateProjectionMatrix(int screenWidth, int screenHeight);

private:
	float2 mousePos;
	Frustum frustum;
};

#endif /* _MODULE_CAMERA_H_ */
