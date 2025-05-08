#pragma once
#include "Core.h"

namespace Engine 
{

    //these classes define different types of universally unique identifies (UUIDs)
    //the base UUID is for 64 bit - better used when a large space of Unique IDs are needed
    //the UUID32 is for 32 bit - for systems that prioritise memory efficiency or where 32-bit IDs are adequate


    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID& other);

        operator uint64_t () { return m_UUID; }
        operator const uint64_t() const { return m_UUID; }
    private:
        uint64_t m_UUID;
    };

    class UUID32
    {
    public:
        UUID32();
        UUID32(uint32_t uuid);
        UUID32(const UUID32& other);

        operator uint32_t () { return m_UUID; }
        operator const uint32_t() const { return m_UUID; }
    private:
        uint32_t m_UUID;
    };
}

//this code provides specialisations for the std::hash template to make either UUID format
//compatible with standard contatiners such as std::unordered_map

namespace std {

    template <>
    struct hash<Engine::UUID>
    {
        std::size_t operator()(const Engine::UUID& uuid) const
        {
            // uuid is already a randomly generated number, and is suitable as a hash key as-is.
            // this may change in future, in which case return hash<uint64_t>()(uuid); might be more appropriate
            return uuid;
        }
    };

    template <>
    struct hash<Engine::UUID32>
    {
        std::size_t operator()(const Engine::UUID32& uuid) const
        {
            return hash<uint32_t>()((uint32_t)uuid);
        }
    };
}
