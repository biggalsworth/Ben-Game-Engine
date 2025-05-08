#pragma once
#include "filesystem"
#include "Renderer/Subtexture.h"
namespace Engine
{
	class Scene;

	class Animator
	{

	public:
		int index = 0;
		float interval;
		float elapsed = 0.0f;
		bool playing = false;
		std::vector<std::filesystem::path> images;
		std::vector<Ref<SubTexture2D>> spriteSheet;

	public:
		Animator(std::vector<std::filesystem::path> _images, std::vector<Ref<SubTexture2D>> sprites, float _interval) : images(_images), spriteSheet(sprites), interval(_interval) {};
		~Animator();


	public:
		void Play();
		void Update(float ts, uint32_t ent, Scene* scene);
		void Stop();

	};
}