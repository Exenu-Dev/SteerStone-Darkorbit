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
#include "PoolUpdater.hpp"
#include "Core/Core.hpp"

namespace SteerStone { namespace Game { namespace Map {

    enum PoolType
    {
        POOL_TYPE_MOB       = 0,
        POOL_TYPE_BONUS_BOX = 1
    };

    class Base;
    enum Type;

    /// Pool Manager
    class PoolManager
    {
        DISALLOW_COPY_AND_ASSIGN(PoolManager);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        /// @p_Map : Reference of map
        PoolManager(Base* p_Map);
        /// Deconstructor
        ~PoolManager();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    
    public:
        /// Initialize Pool for map
        void Initialize();
    private:
        /// Load Mobs into pool
        void InitializeMobs();

    public:
        /// Update Pool
        /// @p_Diff : Execution Time
        void Update(uint32 const p_Diff);

        /// Add Bonus box to map
        /// @p_Unit     : Unit taking away the resources
        /// @p_Type     : Type of Bonus Box
        /// @p_OwnerId  : Owner of cargo box
        void AddBonuxBox(Entity::Unit* p_Unit, BonusBoxType p_Type, uint32 const p_OwnerId);
        /// Remove Bonus Box
        /// @p_Object : Object Bonus Box
        void RemoveBonusBox(Entity::Object* p_Object);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    
    private:
        std::map<uint16, PoolUpdater*> m_Pool;
        Base* m_Map;
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone