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
#include "Network/Server/Listener.hpp"
#include "Diagnostic/DiaStopWatch.hpp"
#include "Opcodes/Opcodes.hpp"
#include "ServerPacket.hpp"

/// Declarations
namespace SteerStone { namespace Game { namespace Entity {
    
    class Player;

}   ///< Entity
}   ///< Game
}   ///< SteerStone

namespace SteerStone { namespace Game { namespace Server {

    enum class Authenticated
    {
        Authenticed,            ///< Authenticated from server
        NotAuthenticated,       ///< Is not authenticated yet from server
    };

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

            /// Handle Initial part of logging into game server
            /// @p_Packet        : Client Packet
            void HandleLoginPacket(ClientPacket* p_Packet);
            /// Handle Client Packet Handler
            /// @p_OpCodeHandler : Handler of Client Packet
            /// @p_Packet        : Client Packet
            void ExecutePacket(OpcodeHandler const* p_OpCodeHandler, ClientPacket* p_Packet);
            /// Send packet to client
            /// @p_PacketBuffer : Packet Buffer
            void SendPacket(const PacketBuffer* p_PacketBuffer);
            /// For Non-Implemented packets
            void HandleNULL(ClientPacket* p_Packet);
            /// For Server packets (Do not require handlers)
            void HandleServer(ClientPacket* p_Packet) {}
            /// For Client Packets which gets handled in ProcessIncomingData
            void HandleClient(ClientPacket* p_Packet) {}

            /// Login Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleLogin(ClientPacket* p_Packet);
            /// Login Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleLogout(ClientPacket* p_Packet);
            /// Login Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleCancelLogout(ClientPacket* p_Packet);
            /// Ping Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandlePingPacket(ClientPacket* p_Packet);

            /// Ship Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleChangeLaser(ClientPacket* p_Packet);
            /// Ship Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleChangeRocket(ClientPacket* p_Packet);
            /// Ship Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleChangeMisc(ClientPacket* p_Packet);
            /// Ship Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleSellOre(ClientPacket* p_Packet);
            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleOrePrices(ClientPacket* p_Packet);

            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleObjectMove(ClientPacket* p_Packet);
            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleInitializeOpponent(ClientPacket* p_Packet);
            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandlePortalJump(ClientPacket* p_Packet);
            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleSelectTarget(ClientPacket* p_Packet);
            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleLaserShoot(ClientPacket* p_Packet);
            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleAbortLaser(ClientPacket* p_Packet);
            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleShootRocket(ClientPacket* p_Packet);
            /// Map Handler
           /// @p_ClientPacket : Packet recieved from client
            void HandleSellResources(ClientPacket* p_Packet);
            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleLootCargo(ClientPacket* p_Packet);
            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleUpdateSettings(ClientPacket* p_Packet);

            /// Map Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleLootOre(ClientPacket* p_Packet);

            /// Debug Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleDebugInitializePlayer(ClientPacket* p_Packet);

            /// Quest Handler
            /// @p_ClientPacket : Packet recieved from client
            void HandleQuestHUD(ClientPacket* p_Packet);

            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleCheckOnline(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleGetOnlineCount(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleCanEquip(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleUpdateInventory(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleUpdateShip(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleUpdateDrones(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleBroughtAmmo(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleUpdateBoosters(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleUpdateClans(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleUpdateQuests(ClientPacket* p_Packet);
            /// Web Handler
            /// @p_ClientPacket : Packet recieved from Web
            void HandleCloak(ClientPacket* p_Packet);

            /// Pointer to Player
            Entity::Player* ToPlayer();
            /// Delete Player
            void DestroyPlayer();


        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            /// Handle incoming data
            virtual Core::Network::ProcessState ProcessIncomingData() override;

            /// Check for Web Packet
            /// @p_Buffer : Buffer of packet
            bool CheckForWebPacket(std::string p_Buffer);

        private:
            Authenticated m_AuthenticateState;
            Entity::Player* m_Player;                       ///< Player of socket
            Core::Diagnostic::StopWatch m_PingStopWatch;    ///< Stop Watch for Ping
    };

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace Steerstone