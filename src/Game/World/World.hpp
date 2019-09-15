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
#include "Singleton/Singleton.hpp"
#include "Utility/UtiLockedQueue.hpp"
#include "GameFlags.hpp"

#define WORLD_SLEEP_TIMER 50

namespace SteerStone { namespace Game { namespace Entity {

    class Player;

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace World {

    class Entity::Player;

    /// World
    class World
    {
        SINGLETON_P_D(World);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Update World
            /// @p_Diff : Execution Diff
            void Update(uint32 p_Diff);
            /// Update Players
            /// @p_Diff : Execution Diff
            void UpdatePlayers(uint32 p_Diff);
            /// Stop World Updating
            bool StopWorld() const;

            /// Add Player to queue
            /// @p_Player : Player being added
            void AddPlayer(Entity::Player* p_Player);
            /// Remove Player from world
            /// @p_Player : Player being removed
            void RemovePlayer(Entity::Player* p_Player);
            /// Process Player in queue
            /// @p_Player : being processed
            void ProcessPlayer(Entity::Player* p_Player);

        private:
            std::map<uint32, Entity::Player*> m_Players;                ///< Player in world
            Core::Utils::LockedQueue<Entity::Player*> m_PlayerQueue;    ///< Player in queue storage
            static volatile bool s_StopWorld;                   ///< Stop World Updating
    };

}   ///< namespace World
}   ///< namespace Game
}   ///< namespace Steerstone

#define sWorldManager SteerStone::Game::World::World::GetSingleton()