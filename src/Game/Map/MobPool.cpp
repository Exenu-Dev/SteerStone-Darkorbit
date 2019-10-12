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

#include "PoolUpdater.hpp"
#include "World.hpp"
#include "MobPool.hpp"

namespace SteerStone { namespace Game { namespace Map {
    
    /// Constructor
    MobPool::MobPool()
    {
    }
    /// Deconstructor
    MobPool::~MobPool()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Update the pool
    /// @p_Diff : Execution Time
    void MobPool::Update(uint32 const p_Diff)
    {
    }

    /// Add to pool
    /// @p_Object : Object being added
    void MobPool::Add(Entity::Object* p_Object)
    {
        m_Pool[p_Object->GetGrid()][p_Object->GetObjectGUID().GetCounter()] = p_Object->ToMob();
    }
    /// Remove from Pool
   /// @p_Object : Object being removed
    void MobPool::Remove(Entity::Object* p_Object)
    {
        auto l_Itr = m_Pool[p_Object->GetGrid()].find(p_Object->GetObjectGUID().GetCounter());
        if (l_Itr != m_Pool[p_Object->GetGrid()].end())
            m_Pool[p_Object->GetGrid()].erase(l_Itr);
    }

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone