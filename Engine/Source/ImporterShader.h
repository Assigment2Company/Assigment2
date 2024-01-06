#pragma once

#include <vector>

struct ResourceShader
{
	const char* mShaderName;
	unsigned int mId;
	unsigned int mType;
};

struct ResourceProgram
{
	unsigned int mProgramId;

	unsigned int vertexShader;
	unsigned int fragmentShader;

	struct VertexShaderAttribute
	{
		unsigned int mLocation;
		unsigned int mCount;
	};

	std::vector<VertexShaderAttribute> mAttributes;

	void CreateProgram(ResourceShader* vertex, ResourceShader* fragment);
};

namespace Importer
{
	namespace Shader
	{
		void Import(const char* filePath);

		void Save(const ResourceShader* ourShader, char** fileBuffer);

		void Load(const char* fileName);

		ResourceShader* CreateShader(unsigned int type, const char* shaderSource);
	}
};

