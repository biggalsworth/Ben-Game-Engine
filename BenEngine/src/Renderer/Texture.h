#pragma once
#include <glm/glm.hpp>

namespace Engine
{

	class Texture
	{
	public:
		//Texture()
		//{
		//	glm::vec2 textCoords[4] = {{min.x, min.y}, {max.x, min.y}, {max.x, max.y}, {min.x, max.y}};
		//}

		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual std::string GetPath() const = 0;
		virtual void SetTextCoords(glm::vec2& coords) {};

		glm::vec2 min = { 0,0 };
		glm::vec2 max = { 1,1 };

		glm::vec2 textCoords[4];


	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const std::string& path);

	};

	class TextureLibrary
	{
	private:
		std::map<std::string, Ref<Texture2D>> loadedTextures;

	public:
		Ref<Texture2D> CreateTexture(std::string _path);

		Ref<Texture2D> GetTexture(const std::string _path);

	};

}

