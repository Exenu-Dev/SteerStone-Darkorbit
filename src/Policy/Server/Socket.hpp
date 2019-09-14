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
#include "ServerPacket.hpp"
#include "Opcodes/Opcodes.hpp"

namespace SteerStone { namespace Policy { namespace Server { 

    class PolicySocket : public Core::Network::Socket
    {
        DISALLOW_COPY_AND_ASSIGN(PolicySocket);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Constructor 
            /// @p_Service : Boost Service
            /// @p_CloseHandler : Close Handler Custom function
            PolicySocket(boost::asio::io_service& p_Service, std::function<void(Socket*)> p_CloseHandler);
            /// Deconstructor
            ~PolicySocket() {}

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////

            /// Send packet to client
            /// @p_PacketBuffer : Packet Buffer
            void SendPacket(const PacketBuffer* p_PacketBuffer);

            /// For Non-Implemented packets
            void HandleNULL(ClientPacket* p_Packet);
            /// For Server packets (Do not require handlers)
            void HandleServer(ClientPacket* p_Packet) {}
            /// Policy Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandlePolicy(ClientPacket* p_Packet);


        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            /// Handle incoming data
            virtual bool ProcessIncomingData() override;

            /// Handle Client Packet Handler
            /// @p_OpCodeHandler : Handler of Client Packet
            /// @p_Packet        : Client Packet
            void ExecutePacket(const OpcodeHandler* p_OpCodeHandler, ClientPacket* p_Packet);
    };

}   ///< namespace Server
}   ///< namespace Policy
}   ///< namespace Steerstone