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
#include "Singleton/Singleton.hpp"
#include "Utility/UtiLockedQueue.hpp"
#include "Player.hpp"
#include "GameFlags.hpp"

#define WORLD_SLEEP_TIMER 60

namespace SteerStone { namespace Game { namespace Entity {

    class Player;

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace World {

    class Entity::Player;

    enum BoolConfigs
    {
        BOOL_CONFIG_MAX = 1
    };

    enum IntConfigs
    {
        INT_CONFIG_MAP_INTERVAL,
        INT_CONFIG_CHECK_FOR_PLAYER,
        INT_CONFIG_CHECK_FOR_INTERACTIVE_EVENTS,
        INT_CONFIG_JUMP_DELAY,
        INT_CONFIG_DELAY_REMOVAL,
        INT_CONFIG_LOG_OUT_TIMER,
        INT_CONFIG_PREMIUM_LOG_OUT_TIMER,
        INT_CONFIG_PLAYER_ATTACK_RANGE,
        INT_CONFIG_MOB_ATTACK_RANGE,
        INT_CONFIG_MIN_FOLLOW_DISTANCE,
        INT_CONFIG_MAX_FOLLOW_DISTANCE,
        INT_CONFIG_MAX_ROAMING_DISTANCE,
        INT_CONFIG_FIND_PLAYER_DISTANCE,
        INT_CONFIG_DISTANCE_AWAY_FROM_PLAYER,
        INT_CONFIG_SAVE_PLAYER_TO_DATABASE,
        INT_CONFIG_MAX,
    };

    enum FloatConfigs
    {
        FLOAT_CONFIG_MAX = 1
    };

    /// World
    class Base
    {
        SINGLETON_P_D(Base);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        //                 WORLD
        ///////////////////////////////////////////
    public:
        /// Update World
        /// @p_Diff : Execution Diff
        void Update(uint32 const p_Diff);
        /// Stop World Updating
        bool StopWorld() const;
        /// Load the World
        void Load();
        /// Get Player Count
        uint32 GetPlayerCount();

        ///////////////////////////////////////////
        //             CONFIGURATION
        ///////////////////////////////////////////
    public:
        /// Load Configs
        void LoadConfigs();
        /// Returns bool value
        bool GetBoolConfig(BoolConfigs p_Index);
        /// Returns integer value
        uint32 GetIntConfig(IntConfigs p_Index);
        /// Returns float value
        float GetFloatConfig(FloatConfigs p_Index);

        ///////////////////////////////////////////
        //             PLAYER HANDLE
        ///////////////////////////////////////////
    public:
        /// Add Player to queue
        /// @p_Player : Player being added
        void AddPlayer(Entity::Player* p_Player);
        /// Remove Player from world
        /// @p_Player : Player being removed
        void RemovePlayer(Entity::Player* p_Player);
        /// Return Player in world
        /// @p_Id : Id of player
        Entity::Player* FindPlayer(uint32 const p_Id);
        /// Process Player in queue
        /// @p_Player : Being processed
        void ProcessPlayer(Entity::Player* p_Player);
        /// Remove from world
        /// @p_Player : Player being removed from world
        void AddPlayerToRemoval(Entity::Player* p_Player);
        /// Process the removal of the player from world
        /// @p_Diff : Execution Time
        void ProcessRemovalPlayer(uint32 const p_Diff);
        /// Update Players
        /// @p_Diff : Execution Diff
        void ProcessPlayerPackets(uint32 p_Diff);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        std::map<uint32, Entity::Player*> m_Players;                        ///< Player in world storage
        Core::Utils::LockedQueue<Entity::Player*> m_PlayerQueue;            ///< Player in queue storage
        std::set<Entity::Player*> m_PlayerRemovalQueue;                     ///< Player in removal queue storage
        uint32 m_IntConfigs[INT_CONFIG_MAX];                                ///< Holds config settings for int
        bool m_BoolConfigs[BOOL_CONFIG_MAX];                                ///< Holds config settings for bool
        float m_FloatConfigs[FLOAT_CONFIG_MAX];                             ///< Holds config settings for float

        static volatile bool s_StopWorld;                                   ///< Stop World Updating
    };

}   ///< namespace World
}   ///< namespace Game
}   ///< namespace Steerstone

#define sWorldManager SteerStone::Game::World::Base::GetSingleton()