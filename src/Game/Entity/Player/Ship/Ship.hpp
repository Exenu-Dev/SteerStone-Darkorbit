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

#include "Socket.hpp"
#include "GameFlags.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets {

    class InitializeShip;
    class MapUpdate;
    class MiniMapUpdate;

}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Entity {

    class Player;
    class Server::Packets::InitializeShip;

    /// Ship Data for player class
    class Ship
    {
        friend class Player;

        public:
            /// Constructor
            /// @p_Player : Player
            Ship(Player* p_Player);
            /// Deconstructor
            ~Ship();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

            /// Load player details from database
            bool LoadFromDB();

            /// Send ship details
            /// @p_Packet : Packet being formed
            void FormulateInitializeShip(Server::Packets::InitializeShip& p_Packet);
            /// Send Map update
            void SendMapUpdate();
            /// Send Rocket and Mine Update
            void SendRocketMineUpdate();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            PlayerShips m_ShipId;
            uint32 m_Speed;
            int32 m_Shield;
            uint32 m_MaxShield;
            int32 m_HitPoints;
            uint32 m_MaxHitPoints;
            uint32 m_CargoSpace;
            uint32 m_MaxCargoSpace;
            float m_PositionX;
            float m_PositionY;
            uint32 m_MapId;
            uint32 m_MaxAmmo;
            uint32 m_MaxRockets;
            uint16 m_WeaponState;
            bool m_UseSystemFont;

            uint32 m_RocketR310;
            uint32 m_RocketPLT2026;
            uint32 m_RocketPLT2021;
            uint32 m_Mines;
            uint32 m_SmartBombs;
            uint32 m_InstantShields;
            Player* m_Player;   ///< Player

    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone