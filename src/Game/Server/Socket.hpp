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
#include "Network/Listener.hpp"
#include "Opcodes/Opcodes.hpp"
#include "ServerPacket.hpp"

/// Declarations
namespace SteerStone { namespace Game { namespace Entity {
    
    class Player;

}   ///< Entity
}   ///< Game
}   ///< SteerStone

namespace SteerStone { namespace Game { namespace Server {

    class Entity::Player;

    class GameSocket : public Core::Network::Socket
    {
        DISALLOW_COPY_AND_ASSIGN(GameSocket);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Constructor 
            /// @p_Service : Boost Service
            /// @p_CloseHandler : Close Handler Custom function
            GameSocket(boost::asio::io_service& p_Service, std::function<void(Socket*)> p_CloseHandler);
            /// Deconstructor
            ~GameSocket() {}

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////

            /// Send packet to client
            /// @p_PacketBuffer : Packet Buffer
            void SendPacket(const PacketBuffer* p_PacketBuffer);
            /// For Non-Implemented packets
            void HandleNULL(ClientPacket* p_Packet);
            /// For Server packets (Do not require handlers)
            void HandleServer(ClientPacket* p_Packet) {}
            /// Login Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleLogin(ClientPacket* p_Packet);

            /// Pointer to Player
            Entity::Player* ToPlayer();
            /// Delete Player
            void DestroyPlayer();


        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            /// Handle incoming data
            virtual bool ProcessIncomingData() override;

            /// Handle Client Packet Handler
            /// @p_OpCodeHandler : Handler of Client Packet
            /// @p_Packet        : Client Packet
            void ExecutePacket(const OpcodeHandler* p_OpCodeHandler, ClientPacket* p_Packet);

        private:
            Entity::Player* m_Player;       ///< Player of socket
    };

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace Steerstone