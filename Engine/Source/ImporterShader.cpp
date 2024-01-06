#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterShader.h"

#include "glew.h"

#include <string>


void Importer::Shader::Import(const char* filePath)
{
	ResourceShader* shader = new ResourceShader;
	char* fileBuffer = nullptr;
	App->GetFileSystem()->Load(filePath, &fileBuffer);

	//LOG("%s", fileBuffer);

	std::string name = filePath;
	int pos = name.find_last_of("/");
	name = name.substr(++pos);
	shader->mShaderName = name.c_str();

	Shader::Save(shader, &fileBuffer);

	Shader::Load(shader->mShaderName);
}

void Importer::Shader::Save(const ResourceShader* ourShader, char** buffer)
{
	std::string path = LIBRARY_SHADER_PATH;
	path += ourShader->mShaderName;

	App->GetFileSystem()->Save(path.c_str(),(*buffer), strlen((*buffer)));
}

void Importer::Shader::Load(const char* fileName)
{
	std::string path = LIBRARY_SHADER_PATH;
	path += fileName;

	std::string name = fileName;
	int extensionPos = name.find_last_of(".");
	name = name.substr(0, extensionPos);

	if (name.compare("vs") == 0)
		CreateShader(GL_VERTEX_SHADER, path.c_str());
	else if (name.compare("fs") == 0)
		CreateShader(GL_FRAGMENT_SHADER, path.c_str());
}

ResourceShader* Importer::Shader::CreateShader(unsigned int type, const char* shaderSource)
{
	ResourceShader* shader = new ResourceShader;

	strcpy(const_cast<char*>(shader->mShaderName), shaderSource);
	shader->mType = type;
	shader->mId = glCreateShader(type);
	glShaderSource(shader->mId, 1, &shaderSource, 0);
	glCompileShader(shader->mId);

	int success = GL_FALSE;
	glGetShaderiv(shader->mId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		int len = 0;
		glGetShaderiv(shader->mId, GL_INFO_LOG_LENGTH, &len);
		if (len > 0)
		{
			int written = 0;
			char* info = new char[len];
			glGetShaderInfoLog(shader->mId, len, &written, info);
			LOG("Log Info: %s", info);
			delete[] info;
			info = nullptr;
		}
	}

	return shader;
}

void ResourceProgram::CreateProgram(ResourceShader* vertex, ResourceShader* fragment)
{

}
