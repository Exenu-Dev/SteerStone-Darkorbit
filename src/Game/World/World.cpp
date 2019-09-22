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

#include "World.hpp"
#include "Player.hpp"
#include "ZoneManager.hpp"
#include "Config/Config.hpp"

namespace SteerStone { namespace Game { namespace World {

    SINGLETON_P_I(Base);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    Base::Base()
    {
    }
    /// Deconstructor
    Base::~Base()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Static StopWorld
    volatile bool Base::s_StopWorld = false;

    /// Load the World
    void Base::Load()
    {
        LoadConfigs();

        sZoneManager->LoadMaps();
    }

    /// Update World
    /// @p_Diff : Execution Diff
    void Base::Update(uint32 const p_Diff)
    {
        Entity::Player* l_Player = nullptr;
        while (m_PlayerQueue.Next(l_Player))
            ProcessPlayer(l_Player);

        UpdatePlayers(p_Diff);

        sZoneManager->Update(p_Diff);
    }
    /// Update Players
    /// @p_Diff : Execution Diff
    void Base::UpdatePlayers(uint32 p_Diff)
    {
        for (auto& l_Itr = m_Players.begin(); l_Itr != m_Players.end();)
        {
            Entity::Player* l_Player = l_Itr->second;

            LOG_ASSERT(l_Player, "World", "Player is null while updating players!");

            Server::WorldUpdateFilter l_Filter(l_Player);

            if (!l_Player->Update(p_Diff, l_Filter))
            {
                l_Itr = m_Players.erase(l_Itr);
                RemovePlayer(l_Player);
            }
            else
                l_Itr++;
        }
    }
    /// Stop World Updating
    bool Base::StopWorld() const
    {
        return s_StopWorld;
    }

    /// Load Configs
    void Base::LoadConfigs()
    {
        m_IntConfigs[IntConfigs::INT_CONFIG_MAP_INTERVAL]                 = sConfigManager->GetInt("MapUpdateInterval", 0);
        m_IntConfigs[IntConfigs::INT_CONFIG_CHECK_FOR_PLAYER]             = sConfigManager->GetInt("CheckForPlayer", 60000);
        m_IntConfigs[IntConfigs::INT_CONFIG_CHECK_FOR_INTERACTIVE_EVENTS] = sConfigManager->GetInt("CheckForInteractiveEvents", 5000);
        m_IntConfigs[IntConfigs::INT_CONFIG_JUMP_DELAY]                   = sConfigManager->GetInt("JumpDelay", 2000);
    }

    /// Returns bool value
    bool Base::GetBoolConfig(BoolConfigs p_Index)
    {
        return p_Index < BOOL_CONFIG_MAX ? m_BoolConfigs[p_Index] : 0;
    }
    /// Returns integer value
    uint32 Base::GetIntConfig(IntConfigs p_Index)
    {
        return p_Index < INT_CONFIG_MAX ? m_IntConfigs[p_Index] : 0;
    }
    /// Returns float value
    float Base::GetFloatConfig(FloatConfigs p_Index)
    {
        return p_Index < FLOAT_CONFIG_MAX ? m_FloatConfigs[p_Index] : 0;
    }

    /// Add Player to queue
    /// @p_Player : Player being added
    void Base::AddPlayer(Entity::Player* p_Player)
    {
        m_PlayerQueue.Add(p_Player);
    }
    /// Remove Player from world
    /// @p_Player : Player being removed
    void Base::RemovePlayer(Entity::Player* p_Player)
    {
        LOG_ASSERT(p_Player, "World", "Attempted to delete player but player already is null!");

        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("World", "Removed Player Id %0 from World", p_Player->GetId());
        #endif

        delete p_Player;
        p_Player = nullptr;
    }
    /// Return Player in world
    /// @p_Id : Id of player
    Entity::Player* Base::FindPlayer(uint32 const p_Id)
    {
        auto l_Itr = m_Players.find(p_Id);
        if (l_Itr != m_Players.end())
            return l_Itr->second;

        return nullptr;
    }
    /// Process Player in queue
    /// @p_Player : being processed
    void Base::ProcessPlayer(Entity::Player* p_Player)
    {
        LOG_ASSERT(p_Player, "World", "Attempted to add a nullptr player!");

        #ifdef STEERSTONE_CORE_DEBUG
                LOG_INFO("World", "Added Player Id %0 to World", p_Player->GetId());
        #endif

        auto l_Itr = m_Players.find(p_Player->GetId());
        if (l_Itr != m_Players.end())
            LOG_ASSERT(!p_Player, "World", "Attempted to add player but player already exists!");

        m_Players[p_Player->GetId()] = p_Player;
    }
}   ///< namespace World
}   ///< namespace Game
}   ///< namespace Steerstone