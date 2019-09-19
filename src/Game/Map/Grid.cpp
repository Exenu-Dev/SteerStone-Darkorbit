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

#include "Packets/Server/MapPackets.hpp"
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

        m_Objects[p_Object->GetGUID()] = p_Object;

        /// If a player is joining then set grid to active
        if (p_Object->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
        {
            m_Players.insert(p_Object);

            if (m_State == State::Idle)
                m_State = State::Active;

            BuildPlayerSpawnAndSend(p_Object);
        }

        LOG_INFO("Grid", "Added GUID: %0 to Grid: X %1 Y %2", p_Object->GetGUID(), m_GridX, m_GridY);
    }
    /// Remove Object from Grid
    /// @p_Object : Object being removed
    void Grid::Remove(Entity::Object* p_Object)
    {
        m_Mutex.lock();
        m_Objects.erase(p_Object->GetGUID());
        m_Mutex.unlock();

        if (p_Object->GetType() == Entity::Type::OBJECT_TYPE_PLAYER)
        {
            m_Mutex.lock();
            m_Players.erase(p_Object);
            m_Mutex.unlock();

            BuildPlayerDespawnAndSend(p_Object);
        }

        LOG_INFO("Grid", "Removed GUID: %0 from Grid: X %1 Y %2", p_Object->GetGUID(), m_GridX, m_GridY);
    }

    /// Move Object
    /// @p_Object : Object being moved
    /// @p_PositionX : New X Axis
    /// @p_PositionY : New Y Axis
    void Grid::Move(Entity::Object* p_Object)
    {
        Server::Packets::ObjectMove l_Packet;
        l_Packet.Id        = p_Object->ToPlayer()->GetId();
        l_Packet.PositionX = p_Object->GetSpline()->GetPlannedPositionX();
        l_Packet.PositionY = p_Object->GetSpline()->GetPlannedPositionY();
        l_Packet.Time      = p_Object->GetSpline()->GetDestinationTime();
        SendPacketEveryone(l_Packet.Write());
    }

    /// Find Player
    /// @p_Id : Id of Player
    Entity::Object* Grid::FindPlayer(uint32 const p_Id)
    {
        m_Mutex.lock();
        std::unordered_set<Entity::Object*> l_Copy(m_Players);
        m_Mutex.unlock();

        for (auto l_Itr : l_Copy)
        {
            if (l_Itr->ToPlayer()->GetId() == p_Id)
                return l_Itr;
        }

        return nullptr;
    }

    /// Send Packet to everyone
    /// @p_Packet : Packing being sent
    /// @p_Object : Send packet to self
    void Grid::SendPacketEveryone(Server::PacketBuffer const* p_Packet, Entity::Object* p_Object)
    {
        m_Mutex.lock();
        std::unordered_set<Entity::Object*> l_Copy(m_Players);
        m_Mutex.unlock();

        for (auto l_Itr : l_Copy)
        {
            if (p_Object)
                if (p_Object->GetGUID() == l_Itr->GetGUID())
                    continue;

            l_Itr->ToPlayer()->SendPacket(p_Packet);
        }
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

        if (!m_Players.empty())
        {
            #ifdef STEERSTONE_CORE_DEBUG
                LOG_INFO("Grid", "Player found in Grid %0 %1 - Keeping grid active!", m_GridX, m_GridY);
            #endif
        }
        else
        {
            /// We have not found a player, turn the grid to idle
            m_State = State::Idle;

            #ifdef STEERSTONE_CORE_DEBUG
                LOG_INFO("Grid", "Player not found in Grid %0 %1 - Grid is now idle!", m_GridX, m_GridY);
            #endif
        }
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