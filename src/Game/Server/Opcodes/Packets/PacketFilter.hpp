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
#include "Core/Core.hpp"
#include "ClientPacket.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    class Player;

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace SteerStone

namespace SteerStone { namespace Game { namespace Server {

    /// Allows to determine if next packet is safe to be processed
    class PacketFilter
    {
        DISALLOW_COPY_AND_ASSIGN(PacketFilter);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Constructor
            /// @p_Player : Player
            explicit PacketFilter(Entity::Player* p_Player);
            /// Deconstructor
            virtual ~PacketFilter() {}

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

            /// Process Packet
            /// @p_Client Packet : Packet being processed
            virtual bool Process(ClientPacket* /*p_ClientPacket*/) 
            { 
                return true; 
            }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        protected:
            Entity::Player* const m_Player;
    };

    /// Thread Safe packets only to be processed
    class PlayerMapFilter : public PacketFilter
    {
        public:
            /// Constructor
            /// @p_Player : Player
            explicit PlayerMapFilter(Entity::Player* p_Player);
            /// Deconstructor
            ~PlayerMapFilter() { }

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////

            /// Process Packet in Map::Update
            /// @p_ClientPacket : Packet being processed
            virtual bool Process(ClientPacket* p_ClientPacket) override;
    };

    /// UnThread Safe packets only to be processed
    class WorldUpdateFilter : public PacketFilter
    {
        public:
            /// Constructor
            /// @p_Player : Player
            explicit WorldUpdateFilter(Entity::Player* p_Player);
            /// Deconstructor
            ~WorldUpdateFilter() { }

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////

            /// Process Packet in Player::Update
            /// @p_ClientPacket : Packet being processed
            virtual bool Process(ClientPacket* p_ClientPacket) override;
    };

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone