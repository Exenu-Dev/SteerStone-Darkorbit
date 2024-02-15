/*
* Liam Ashdown
* Copyright (C) 2022
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
#include <mutex>
#include "Core/Core.hpp"
#include "GridDefines.hpp"
#include "Diagnostic/DiaIntervalTimer.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    class Object;
    class Player;
    class Portal;
    class BonusBox;
    class Mob;
    class Unit;
    class Ore;

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Server {

    class PacketBuffer;

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Map {

    enum class State
    {
        Idle,       ///< Grid is in Idle mode, there's no players
        Active,     ///< Grid is Active, there are players
        InActive    ///< Grid is turned off regardless if there's players
    };

    enum class GridType
    {
        GRID_TYPE_NORMAL,
        GRID_TYPE_RADIATION,
    };

    class Entity::Object;
    class Entity::Player;
    class Entity::Portal;
    class Entity::BonusBox;
    class Server::PacketBuffer;
    class Base;

    typedef std::unordered_map<uint32, Entity::Object*> ObjectMap;
    typedef std::unordered_set<Entity::Player*> PlayerMap;

    /// Grid
    class Grid
    {
        DISALLOW_COPY_AND_ASSIGN(Grid);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////
    public:
        /// Constructor
        /// @p_Map   : Map who owns the grid
        /// @p_Type  : Type of grid
        /// @p_GridX : Grid X
        /// @p_GridY : Grid Y
        Grid(Base* p_Map, GridType p_Type, uint32 const p_GridX, uint32 const p_GridY);
        /// Deconstructor
        ~Grid();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        //               GENERAL
        ///////////////////////////////////////////
    public:
        /// Check if Player is near any interactive events
        /// @p_Objects : Portal and stations
        /// @p_Diff : Execution Time
        void UpdateInteractiveEvents(std::vector<Entity::Object*>& p_Objects, uint32 const p_Diff);
        /// Update Surrounding objects for players
        void UpdateSurroundingObjects();
        /// Check if a Player is inside the grid
        /// @p_Diff : Execution Time
        void CheckForPlayer(uint32 const p_Diff);
        /// Check if any objects are scheduled for deletion
        void CheckForScheduleForDeletion();
        /// Update Grid
        /// @p_Diff : Execution Time
        bool Update(uint32 const p_Diff);

        /// Check if near portal
        /// @p_Object : Object being checked
        Entity::Portal* CanJumpPortal(Entity::Object* p_Object);

        ///////////////////////////////////////////
        //               OBJECTS
        ///////////////////////////////////////////
    public:
        /// Add Object to Grid
        /// @p_Object       : Object being added
        /// @p_SendMovement : Send Movement packet when joining
        void Add(Entity::Object* p_Object, bool p_SendMovement = false);
        /// Remove Object from Grid
        /// @p_Object : Object being removed
        /// @p_SendPacket : Send Despawn Packet
        /// @p_Remove : Remove from ObjectMap
        void Remove(Entity::Object* p_Object, bool p_SendPacket = false, bool p_Remove = true);
        /// Unload objects from map
        void Unload();
        /// Build surrounding objects near player
        /// @p_Player       : Player
        void BuildSurroundingObjects(Entity::Player* p_Player);
        /// Build Player Spawn Packet
        /// @p_ObjectBuilt : Object being built
        /// @p_Player      : Player
        void BuildObjectSpawnAndSend(Entity::Object* p_ObjectBuilt, Entity::Player* p_Player);
        /// Build Object Despawn Packet
        /// @p_Object : Object being built
        void BuildObjectDespawnAndSend(Entity::Object* p_Object);

        ///////////////////////////////////////////
        //               FINDER
        ///////////////////////////////////////////
    public:
        /// Find Player
        /// @p_Id : Account Id of Player
        Entity::Object* FindPlayer(uint32 const p_Id);
        /// Find Nearest Player to Object
        /// @p_Object : Object which we are checking against
        /// @p_Distance : Find Object in a specific range
        Entity::Player* FindNearestPlayer(Entity::Object* p_Object, float p_Distance = 0.0f);
        /// Find Object
        /// @p_Counter : Counter of Object
        Entity::Object* FindObject(uint32 const p_Counter);

        ///////////////////////////////////////////
        //               PACKETS
        ///////////////////////////////////////////
    public:
        /// Send Packet to everyone in grid
        /// @p_Packet : Packet being sent
        void SendPacketToEveryone(Server::PacketBuffer const* p_Packet);
        /// Send Packet to everyone if object is in surrounding
        /// @p_Packet : Packet being sent
        /// @p_Object : Object being checked
        /// @p_SendToSelf : Send Packet to self
        void SendPacketIfInSurrounding(Server::PacketBuffer const* p_Packet, Entity::Object* p_Object, bool p_SendToSelf = false);
        /// Move Object
        /// @p_Object : Object being moved
        void Move(Entity::Object* p_Object);

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        State GetState()                const { return m_State;     }
        GridType GetGridType()          const { return m_Type;      }
        ObjectMap const GetObjects()    const { return m_Objects;   }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        ObjectMap m_Objects;                                         ///< Objects in Grid
        PlayerMap m_Players;                                         ///< Players in Grid
        Base* m_Map;                                                 ///< Map
        State m_State;                                               ///< Grid State
        GridType m_Type;                                                 ///< Grid Type
        uint32 m_GridX;                                              ///< Grid Index X
        uint32 m_GridY;                                              ///< Grid Index Y
        Core::Diagnostic::IntervalTimer m_IntervalCheckPlayer;       ///< Interval Timer
        Core::Diagnostic::IntervalTimer m_IntervalInteractiveEvents; ///< Interval Timer
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone