#include "Engine_PCH.h"
#include "Texture.h"
#include "OpenGLTexture.h"

namespace Engine
{
	Ref<Texture2D> TextureLibrary::CreateTexture(std::string _path)
	{
		if (loadedTextures.find(_path) == loadedTextures.end())
		{
			loadedTextures.emplace(_path, Texture2D::Create(_path));
			return loadedTextures[_path];
		}
		else 
		{
			return loadedTextures[_path];
		}
	};

	Ref<Texture2D> TextureLibrary::GetTexture(std::string _path)
	{
		if (loadedTextures[_path])
		{
			return loadedTextures[_path];
		}
	}


	Ref<Texture2D> Texture2D::Create(const std::string& filePath)
	{
		//TODO: check if the api is opengl or not
		return std::make_shared<OpenGLTexture2D>(filePath);
	}

}