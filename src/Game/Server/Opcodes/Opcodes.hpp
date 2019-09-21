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

#ifndef _OPCODES_OPCODES_HPP_
#define _OPCODES_OPCODES_HPP_
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "Singleton/Singleton.hpp"
#include "PacketFilter.hpp"
#endif /* _OPCODES_OPCODES_HPP_ */

enum class PacketStatus
{
    STATUS_UNHANDLED,           ///< Packet not handled
    STATUS_AUTHENTICATION,      ///< Player has not been created yet
    STATUS_LOGGED_IN            ///< Player is created and in world        
};

enum class PacketProcess
{  
    PROCESS_WORLD_THREAD,       ///< Process Packet in Player::Update           (Thread Unsafe)
    PROCESS_MAP_THREAD,         ///< Process Packet in Map::Update              (Thread Safe)
    PROCESS_PLAYER_THREAD       ///< Process Packet in Socket::ReadIncomingData (For non implemented packets)
};

enum ClientOpCodes : uint8
{
    CLIENT_PACKET_LOGIN                 = 0x4C,
    CLIENT_PACKET_CHANGE_LASER          = 0x75,
    CLIENT_PACKET_CHANGE_ROCKET         = 0x64,
    CLIENT_PACKET_PING                  = 0x50,
    CLIENT_PACKET_OBJECT_MOVE           = 0x31,
    CLIENT_PACKET_INITIALIZE_PLAYER     = 0x69,
    CLIENT_PACKET_PORTAL_JUMP           = 0x6A,
    CLIENT_PACKET_TRADE_ORE             = 0x62,
    CLIENT_MAX_OPCODE                   = 0x7A
};

enum ServerOpCodes : uint8
{
    SERVER_PACKET_PLAYER_INFO           = 0x41,
    SERVER_PACKET_INITIALIZE_SHIP       = 0x49,
    SERVER_PACKET_MAP_UPDATE            = 0x69,
    SERVER_PACKET_MINI_MAP_UPDATE       = 0x6D,
    SERVER_PACKET_UPDATE_ROCKET_MINE    = 0x33,
    SERVER_PACKET_UPDATE_BATTERY        = 0x42,
    SERVER_PACKET_LOGGED_IN             = 0x38,
    SERVER_PACKET_SPAWN_SHIP            = 0x43,
    SERVER_PACKET_DESPAWN_SHIP          = 0x52,
    SERVER_PACKET_OBJECT_MOVE           = 0x31,
    SERVER_PACKET_DISPLAY_SUCC_DEBUG    = 0x53,
    SERVER_PACKET_CREATE_PORTAL         = 0x70,
    SERVER_PACKET_CREATE_STATION        = 0x73,
    SERVER_PACKET_SEND_MESSAGE          = 0x41,
    SERVER_PACKET_EVENT                 = 0x44,
    SERVER_MAX_OPCODE                   = 0x7A,
};

namespace SteerStone { namespace Game { namespace Server {

    class GameSocket;

    /// Opcode Handler
    struct OpcodeHandler
    {
        char const* Name;
        PacketStatus Status;
        PacketProcess Process;
        void (GameSocket::*Handler)(ClientPacket* p_Packet);
    };

    using OpCodeMap =  std::map<uint64, OpcodeHandler>;

    /// Stores Opcodes handlers for server and client
    class OpCodes
    {
        SINGLETON_P_D(OpCodes);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Initialize Client and Server Handlers
            void InitializePackets();

            /// Get Client Packet
            /// @p_Id : Id of client packet we are searching for
            OpcodeHandler const* GetClientPacket(ClientOpCodes p_Id);
            /// Get Server Packet
            /// @p_Id : Id of server packet we are searching for
            OpcodeHandler const* GetServerPacket(ServerOpCodes p_Id);

            /// Get Client Opcode Name
            std::string GetClientOpCodeName(ClientOpCodes p_Opcode);
            /// Get Server Opcode Name
            std::string GetServerOpCodeName(ServerOpCodes p_Opcode);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            /// Store Client packet into storage
            /// @p_Opcode : Opcode Id
            /// @p_Name : Name of Opcode
            /// @p_Status : Status of Opcode
            /// @p_Process : When to process the packet
            /// @p_Handler : Handler Function which we will be accessing too
            void StoreClientPacket(uint8 const p_OpCode, char const* p_Name, PacketStatus const p_Status, PacketProcess p_Process, void (GameSocket::*handler)(ClientPacket* p_Packet));
            /// Store Server packet into storage
            /// @p_Opcode : Opcode Id
            /// @p_Name : Name of Opcode
            /// @p_Handler : Handler Function which we will be accessing too
            void StoreServerPacket(uint8 const p_OpCode, char const* p_Name, void (GameSocket::* p_Handler)(ClientPacket* p_Packet));

        private:
            static OpcodeHandler const m_EmptyHandler;      ///< Empty handler if client packet has not been given a handler yet
            OpCodeMap m_ClientOpCodes;                      ///< Holds client packets
            OpCodeMap m_ServerOpCodes;                      ///< Holds server packets
    };

}   ///< namespace Packet
}   ///< namespace Game
}   ///< namespace SteerStone

#define sOpCode SteerStone::Game::Server::OpCodes::GetSingleton()