#include "Engine_PCH.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Animator.h"

namespace Engine
{

	void Animator::Play()
	{
		playing = true;
		elapsed = interval;
		index = 0;

	}

	void Animator::Update(float ts, uint32_t ent, Scene* scene)
	{
		if (playing == false)
			return;

		elapsed += ts;
		if (elapsed >= interval)
		{
			elapsed = 0;
			

			Entity entity { (entt::entity)ent, scene };
			if (entity.HasComponent<SpriteRendererComponent>())
			{
				if(spriteSheet.empty() == true)
				{
					if (index >= images.size())
						index = 0;

					entity.GetComponent<SpriteRendererComponent>().texture = Project::GetTextureLibrary()->CreateTexture(images[index].string());
				}
				else
				{
					if (index >= spriteSheet.size())
						index = 0;
					//*entity.GetComponent<SpriteRendererComponent>().texture->textCoords = *spriteSheet[index]->GetTexCoords();
					//entity.GetComponent<SpriteRendererComponent>().texture = spriteSheet[index]->GetTexture();

					auto& sprite = entity.GetComponent<SpriteRendererComponent>();
					//sprite.texture = spriteSheet[index]->GetTexture();
					//*sprite.texture->textCoords = *spriteSheet[index]->GetTexCoords();

					sprite.texture = spriteSheet[index]->GetTexture();
					memcpy(sprite.texture->textCoords, spriteSheet[index]->GetTexCoords(), sizeof(glm::vec2) * 4);


					LOG_INFO("Sprite Tex Path: {}", spriteSheet[index]->GetTexture()->GetPath());
					LOG_INFO("Coords: ({}, {}), ({}, {})",
						spriteSheet[index]->GetTexCoords()[0].x, spriteSheet[index]->GetTexCoords()[0].y,
						spriteSheet[index]->GetTexCoords()[2].x, spriteSheet[index]->GetTexCoords()[2].y);


				}	
			}
			index++;
		}
	}

	void Animator::Stop()
	{
		playing = false;
	}




}
