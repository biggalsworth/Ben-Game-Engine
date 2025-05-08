#include "Engine_PCH.h"
#include "SoundSystem.h"

namespace Engine
{
    SoundSystem::SoundSystem()
        : m_system(nullptr), m_sound(nullptr), m_channel(nullptr)
    {
        // Initialize FMOD System
        FMOD_RESULT result = FMOD::System_Create(&m_system);
        if (result != FMOD_OK || !m_system)
        {
            LOG_ERROR("Failed to create FMOD system");
            return;
        }

        int driverCount = 0;
        m_system->getNumDrivers(&driverCount);

        if (driverCount == 0)
        {
            LOG_ERROR("No audio drivers found");
            m_system->release();
            m_system = nullptr;
            return;
        }

        // Initialize FMOD
        result = m_system->init(512, FMOD_INIT_NORMAL, nullptr);
        if (result != FMOD_OK)
        {
            LOG_ERROR("FMOD initialization failed");
            m_system->release();
            m_system = nullptr;
            return;
        }
    }

    void SoundSystem::PlaySound(const std::string path)
    {
        // Load a sound
        m_system->createSound(path.c_str(), FMOD_3D, nullptr, &m_sound);

        m_sound->setMode(FMOD_3D_INVERSETAPEREDROLLOFF);

        // Play the sound
        m_system->playSound(m_sound, nullptr, false, &m_channel);
                        
        m_channel->setMode(FMOD_3D_INVERSETAPEREDROLLOFF);

        // Main loop to keep the program running
        //loop  the sound in the background, preventing blocking while the startup sound plays
        std::thread playSound([&]() 
            {
            bool isPlaying = true;
            while (isPlaying) 
            {
                m_system->update();
                m_channel->isPlaying(&isPlaying); // Correctly updates `isPlaying`
            }

            m_sound->release(); // Frees sound only after loop exits
            });

        playSound.detach();

        
    }

    //inline void SoundSystem::PlaySound(const std::string path)
    //{
    //    // Load a sound
    //    m_system->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_sound);
    //
    //    // Play the sound
    //    m_system->playSound(m_sound, nullptr, false, &m_channel);
    //
    //    // Main loop to keep the program running
    //    bool isPlaying = true;
    //    while (isPlaying) {
    //        m_system->update();
    //        m_channel->isPlaying(&isPlaying);
    //    }
    //
    //
    //}

}
