#pragma once

#include "fmod_common.h"
#include "fmod.hpp"
#include "fmod_errors.h" // Only if you want error checking

namespace Engine
{
	class SoundSystem
	{

	public:
		SoundSystem();
		~SoundSystem() { StopSystem(); }

	public:
		void PlaySound(const std::string path);

		void StopSystem()
		{
			if (m_system)
			{
				m_system->close();
				m_system->release();
				m_system = nullptr;


				if (m_sound)
				{
					try
					{
						m_sound = nullptr;
					}
					catch (...){}
				}
			}
		}

		void Update()
		{
			if (m_system)
				m_system->update();
		}

	public:
		// Pointer to the FMOD instance
		FMOD::System* m_system;
		FMOD::Sound* m_sound;
		FMOD::Channel* m_channel = nullptr;
	};
}