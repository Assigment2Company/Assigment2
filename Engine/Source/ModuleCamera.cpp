#include "ModuleCamera.h"
#include "Math/float3.h"
#include "MathFunc.h"
#include "SDL.h"
#include "Application.h"
#include "ModuleInput.h"

ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}




bool ModuleCamera::Init()
{

    frustum.type = FrustumType::PerspectiveFrustum;
    frustum.pos = float3(0.0f, 1.0f, 2.0f);
    frustum.nearPlaneDistance = 0.1f;
    frustum.farPlaneDistance = 200.0f;
    frustum.horizontalFov = DegToRad(90.0f);
    frustum.verticalFov = 2.f * Atan(Tan(frustum.horizontalFov * 0.5f) / 1.3f);;

    frustum.front = float3(1, 0, 0);
    frustum.up = float3(0, 1, 0);
    int x, y;
    SDL_GetMouseState(&x, &y);
    mousePos = float2((float)x, (float)y);

    return true;
}

update_status ModuleCamera::Update()
{
    ProcessInput();
    return UPDATE_CONTINUE;
}

void ModuleCamera::Move(const float3& delta)
{
    frustum.pos = frustum.pos - delta;
}

void ModuleCamera::MoveForward(bool backwards)
{

    int sign = backwards ? 1 : -1;
    Move(sign * frustum.front.Normalized() * 0.1f);
    
}

void ModuleCamera::Rotate(float angle, const float3& axis)
{
    float3x3 rotationMatrix = float3x3::RotateAxisAngle(axis, DegToRad(angle));

    float3 newfront, newup;
    newfront = rotationMatrix.MulDir(frustum.front.Normalized());
    newup = rotationMatrix.MulDir(frustum.up.Normalized());
    frustum.front = newfront.Normalized();
    frustum.up = newup.Normalized();

}


void ModuleCamera::ProcessInput()
{

    float speed = 0.01f;
    int x, y;
    SDL_GetMouseState(&x, &y);

    float2 mouse_delta = float2(mousePos.x - x, mousePos.y - y);
    mousePos = float2((float)x, (float)y);


    if (App->GetInput()->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT)
        speed = speed * 2;
    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT) {

        if (App->GetInput()->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
            Move(-frustum.front.Normalized() * speed);

        if (App->GetInput()->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
            Move(frustum.front.Normalized() * speed);

        if (App->GetInput()->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
            Move(frustum.WorldRight().Normalized() * speed);

        if (App->GetInput()->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
            Move(-frustum.WorldRight().Normalized() * speed);

        if (App->GetInput()->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
            Move(frustum.up.Normalized() * speed);
        if (App->GetInput()->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
            Move(-frustum.up.Normalized() * speed);

        if (mouse_delta.x != 0)
            Rotate(mouse_delta.x , float3::unitY);
        if (mouse_delta.y != 0)
            Rotate(mouse_delta.y , frustum.WorldRight());
    }
    else if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT && App->GetInput()->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT) {
        

        float distanceCameraObject = (float3::zero - frustum.pos).Length();
        if (mouse_delta.x != 0)
            Move(-mouse_delta.x * speed * 2 * distanceCameraObject * frustum.WorldRight());
        if (mouse_delta.y != 0)
            Move(mouse_delta.y * speed * 2 * distanceCameraObject * frustum.up);
        float3 oldRight = frustum.WorldRight().Normalized();
        frustum.front = (float3::zero - frustum.pos).Normalized();
        frustum.up = Cross(oldRight, frustum.front).Normalized();

        float newDistance = (float3::zero - frustum.pos).Length();
        Move(-frustum.front.Normalized() * (newDistance - distanceCameraObject)); //Make sure you don't orbit away
    }
    else {

        if (App->GetInput()->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT)
            Rotate(speed * 100, frustum.WorldRight());
        if (App->GetInput()->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT)
            Rotate(-speed * 100, frustum.WorldRight());
        if (App->GetInput()->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT)
            Rotate(speed * 100, float3::unitY);
        if (App->GetInput()->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT)
            Rotate(-speed * 100, float3::unitY);
    }


    if (App->GetInput()->GetKey(SDL_SCANCODE_F) == KeyState::KEY_REPEAT)
    {
        //Make this a lookat function Change float3::zero for a float3 input vector
        float3 oldRight = frustum.WorldRight().Normalized();
        frustum.front = (float3::zero - frustum.pos).Normalized();
        frustum.up = Cross(oldRight, frustum.front).Normalized();
    }
}

void ModuleCamera::UpdateProjectionMatrix(int screenWidth, int screenHeight)
{
    // Recalculate the projection matrix with the new aspect ratio
    float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    frustum.horizontalFov = DegToRad(90.0f);
    frustum.verticalFov = 2.0f * atan(tan(frustum.horizontalFov * 0.5f) / aspectRatio);
}



