#pragma once
#include <Renderer/Texture.h>
#include <Audio/SoundSystem.h>
#include <filesystem>
#include <GraphicsAPI/stb_image.h>

#include <glad/glad.h>
#include "FileWatch.hpp"


namespace Engine
{
	static class Project
	{
	public:
		Project();
		~Project();

		inline static Ref<TextureLibrary> GetTextureLibrary() { return s_TextureLibrary; }

		inline static std::vector<std::filesystem::path>* FindFileInSubfolders(const std::filesystem::path& directory, const std::string& filename)
		{
			std::string name;
			std::string file;
			file = filename;

			//convert the file to lower case
			std::transform(file.begin(), file.end(), file.begin(), ::tolower);

			std::vector<std::filesystem::path>* files = new std::vector<std::filesystem::path>();
			for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) 
			{
				if (entry.is_directory())
					continue;

				//convert the file to lower case
				name = entry.path().filename().string();
				std::transform(name.begin(), name.end(), name.begin(), ::tolower);

				//if (name.find(filename) != std::string::npos && entry.path().extension() == ".png")
				if (name.find(file) != std::string::npos)
				{
					files->push_back(entry.path());
				}
			}
			return files; // File not found
		}

		static GLuint LoadTextureFromFile(const char* filename)
		{
			// Generate texture ID and load texture data
			GLuint textureID;
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);

			int width, height, nrComponents;
			unsigned char* data = stbi_load(filename, &width, &height, &nrComponents, 0);
			if (data) {
				GLenum format;
				if (nrComponents == 1)
					format = GL_RED;
				else if (nrComponents == 3)
					format = GL_RGB;
				else if (nrComponents == 4)
					format = GL_RGBA;

				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				stbi_image_free(data);
			}
			else {
				std::cerr << "Failed to load texture: " << filename << std::endl;
				stbi_image_free(data);
				return 0;
			}

			// Set the texture wrapping/filtering options (on the currently bound texture object)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			return textureID;
		}

	public:
		static Ref<SoundSystem> m_Sound;

		static std::filesystem::path currCodeProj;


	private:
		inline static Ref<TextureLibrary> s_TextureLibrary;
		static std::unique_ptr<filewatch::FileWatch<std::wstring>> watch;
	};
}

