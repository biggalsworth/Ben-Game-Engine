#pragma once
#include "Texture.h"

namespace Engine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string path);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; };
		virtual uint32_t GetHeight() const override { return m_Height; };

		virtual void Bind(uint32_t slot = 0) const override;

		virtual std::string GetPath() const override;

		//virtual void SetTextCoords(glm::vec2& coords) override { *textCoords = coords; };

	private:
		std::string m_path;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_RendererID;
	};
}