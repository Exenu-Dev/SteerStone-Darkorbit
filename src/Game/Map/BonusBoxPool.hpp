/*
* Liam Ashdown
* Copyright (C) 2021
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
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Bonus Box Pool
    class BonusBoxPool : public PoolUpdater
    {
    public:
        /// Constructor
        BonusBoxPool(); 
        /// Deconstructor
        ~BonusBoxPool();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Update the pool
        /// @p_Diff : Execution Time
        virtual void Update(uint32 const p_Diff) override;

        /// Add to pool
        /// @p_Object : Object being added
        virtual void Add(Entity::Object* p_Object) override;
        /// Remove from Pool
        /// @p_Object : Object being removed
        virtual void Remove(Entity::Object* p_Object) override;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        std::unordered_map<Grid*, std::unordered_map<uint32, Entity::BonusBox*>> m_Pool;
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone