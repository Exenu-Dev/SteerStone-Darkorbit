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

#include "Grid.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "Database/DatabaseTypes.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_GridX : Grid X
    /// @p_GridY : Grid Y
    Grid::Grid(uint32 const p_GridX, uint32 const p_GridY)
        : m_GridX(p_GridX), m_GridY(p_GridY)
    {
        /// Grids are active on creation
        m_State = State::Active;

        m_IntervalCheckPlayer.SetInterval(sWorldManager->GetIntConfig(World::IntConfigs::INT_CONFIG_CHECK_FOR_PLAYER));
    }
    /// Deconstructor
    Grid::~Grid()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Add Object to Grid
    /// @p_Object : Object being added
    void Grid::Add(Entity::Object* p_Object)
    {
        std::lock_guard<std::mutex> l_Guard(m_Mutex);

        m_Objects[p_Object->GetObjectGUID()->GetCounter()] = p_Object;

        /// If a player is joining then set grid to active
        if (p_Object->GetObjectType() == Entity::ObjectType::OBJECT_TYPE_PLAYER)
            if (m_State == State::Idle)
                m_State = State::Active;

        LOG_INFO("Grid", "Added GUID: %0 to Grid: X %1 Y %2", p_Object->GetObjectGUID()->GetCounter(), m_GridX, m_GridY);
    }
    /// Remove Object from Grid
    /// @p_Object : Object being removed
    void Grid::Remove(Entity::Object* p_Object)
    {
        std::lock_guard<std::mutex> l_Guard(m_Mutex);

        auto l_Itr = m_Objects.find(p_Object->GetObjectGUID()->GetCounter());
        if (l_Itr != m_Objects.end())
        {
            LOG_INFO("Grid", "Removed GUID: %0 from Grid: X %1 Y %2", p_Object->GetObjectGUID()->GetCounter(), m_GridX, m_GridY);
            m_Objects.erase(l_Itr);
        }
        else
            LOG_WARNING("Grid", "Attempted to remove Object %0 from grid, but object does not exist!", p_Object->GetName());
    }

    /// Get State of Grid
    State Grid::GetState() const
    {
        return m_State;
    }

    /// Check if a player is inside the grid
    /// @p_Diff : Execution Time
    void Grid::CheckForPlayer(uint32 const p_Diff)
    {
        m_IntervalCheckPlayer.Update(p_Diff);
        if (!m_IntervalCheckPlayer.Passed())
            return;

        for (auto l_Itr : m_Objects)
        {
            /// We a player, keep the grid active
            if (l_Itr.second->GetObjectType() == Entity::ObjectType::OBJECT_TYPE_PLAYER)
            {
                #ifdef STEERSTONE_CORE_DEBUG
                    LOG_INFO("Grid", "Player found in Grid %0 %1 - Keeping grid active!", m_GridX, m_GridY);
                #endif

                return;
            }
        }

        /// We have not found a player, turn the grid to idle
        m_State = State::Idle;

        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("Grid", "Player not found in Grid %0 %1 - Grid is now idle!", m_GridX, m_GridY);
        #endif
    }

    /// Update Grid
    /// @p_Diff : Execution Time
    bool Grid::Update(uint32 const p_Diff)
    {
        std::lock_guard<std::mutex> l_Guard(m_Mutex);

        CheckForPlayer(p_Diff);

        for (auto l_Itr : m_Objects)
        {
            if (Entity::Player * l_Player = l_Itr.second->ToPlayer())
            {
                Server::PlayerMapFilter l_Filter(l_Player);
                l_Player->Update(p_Diff, l_Filter);
            }
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

   
}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone