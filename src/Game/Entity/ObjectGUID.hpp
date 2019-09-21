/*
* Liam Ashdown
* Copyright (C) 2019
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <PCH/Precompiled.hpp>
#include "Logger/LogDefines.hpp"
#include "Logger/Base.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    enum class GUIDType
    {
        Player      = 0x01,
        NPC         = 0x02,
        Portal      = 0x05,
        Station     = 0x06,
    };

    /// Generate Unique Counter
    class ObjectGuidGenerator
    {
        DISALLOW_COPY_AND_ASSIGN(ObjectGuidGenerator);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
        
    public:
        /// Constructor
        ObjectGuidGenerator() { }
        /// Deconstructor
        ~ObjectGuidGenerator() {}

        /// Max UInt32
        inline uint32 GetMaxCounter()
        {
            return uint32(0xFFFFFFFF);
        }

        /// Generate Unique Counter
        uint32 Generate()
        {
            static uint32 l_NextGUID = 0;
            if (l_NextGUID >= GetMaxCounter() - 1)
                LOG_ASSERT(false, "ObjectGuidGenerator", "Reach maxed 32 interger! Overflow!");

            return l_NextGUID++;
        }
    };

    class ObjectGUID
    {
    public:
        /// Constructor
        ObjectGUID()
        {
            m_GUID = 0;
        }

        /// Constructor
        /// @p_Type     : Type of Object
        /// @p_Entry    : Entry of Object
        /// @p_Counter  : Unique Counter
        ObjectGUID(GUIDType p_Type, uint32 p_Entry = 0, uint32 p_Counter = ObjectGuidGenerator().Generate())
        {
            m_GUID = uint64(p_Counter ? uint64(p_Counter) | (uint64(p_Entry) << 32) | (uint64(p_Type) << 52) : 0);
        }
        /// Deconstructor
        ~ObjectGUID()
        {

        }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Get Entry
        uint32 GetEntry() const 
        { 
            return HasEntry() ? uint32((m_GUID >> 32) & uint64(0x00000000000FFFFF)) : 0; 
        }
        /// Get Type
        GUIDType GetType() const
        {
            return GUIDType((uint64(m_GUID) >> 52) & 0x00000FFF);
        }
        /// Get Counter
        uint32 GetCounter()  const
        {
            return uint32(m_GUID & uint64(0x00000000FFFFFFFF));
        }
        /// Get GUID
        uint64 GetGUID() const
        {
            return m_GUID;
        }

        /// Is Player
        bool IsPlayer() const { return !IsEmpty() && GetType() == GUIDType::Player; }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        /// Is Object not a player
        bool HasEntry() const 
        { 
            return HasEntry(GetType());
        }

        /// Is GUID empty
        bool IsEmpty()              const 
        { 
            return m_GUID == 0; 
        }

        static bool HasEntry(GUIDType p_Type)
        {
            switch (p_Type)
            {
            case GUIDType::NPC:
            case GUIDType::Portal:
                return true;
            default:
                return false;
            }
        }

    private:
        uint64 m_GUID;  ///< GUID
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone