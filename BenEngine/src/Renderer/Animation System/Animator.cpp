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
			if (index >= images.size())
				index = 0;

			Entity entity { (entt::entity)ent, scene };
			if (entity.HasComponent<SpriteRendererComponent>())
			{
				if(spriteSheet.empty() == true)
					entity.GetComponent<SpriteRendererComponent>().texture = Project::GetTextureLibrary()->CreateTexture(images[index].string());
				else
				{
					*entity.GetComponent<SpriteRendererComponent>().texture->textCoords = *spriteSheet[index]->GetTexCoords();
					entity.GetComponent<SpriteRendererComponent>().texture = spriteSheet[index]->GetTexture();
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
