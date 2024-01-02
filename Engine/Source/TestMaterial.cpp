#include "TestMaterial.h"

TestMaterial::TestMaterial()
{
}

TestMaterial::TestMaterial(const TestMaterial& original)
{
}

void TestMaterial::Draw()
{

}

void TestMaterial::Load()
{
    LoadVBO();
}

void TestMaterial::Update()
{
    Draw();
}

void TestMaterial::DrawEditor()
{
}

Component* TestMaterial::Clone()
{
    return new TestMaterial(*this);
}

void TestMaterial::LoadVBO()
{

}

void TestMaterial::LoadEBO()
{
}

void TestMaterial::LoadVAO()
{
}
