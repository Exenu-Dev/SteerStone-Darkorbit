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

#ifndef _OPCODES_OPCODES_HPP_
#define _OPCODES_OPCODES_HPP_
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "Singleton/Singleton.hpp"
#include "Packets/ClientPacket.hpp"
#endif /* _OPCODES_OPCODES_HPP_ */


enum class PacketStatus
{
    STATUS_UNHANDLED,
    STATUS_HANDLED
};

enum class PacketProcess
{
    PROCESS_NOW
};

enum ClientOpCodes
{
    CLIENT_PACKET_LOG_IN        = 0xD7,
    CLIENT_PACKET_SEND_MESSAGE  = 0x86,
    CLIENT_PACKKET_PONG         = 0xD1,
    CLIENT_PACKET_CHANGE_ROOM   = 0xDC,
};

enum ServerOpCodes
{
    SERVER_PACKET_USER_LOGIN		        = 0x01,
    SERVER_PACKET_SEND_MESSAGE              = 0x61,
    SERVER_PACKET_SEND_SYSTEM_MESSAGE       = 0x64,
    SERVER_PACKET_SEND_ADMIN_MESSAGE        = 0x6A,
    SERVER_PACKET_SEND_PING                 = 0x65,
    SERVER_PACKET_SEND_BAN_MESSAGE          = 0x66,
    SERVER_PACKET_SEND_IP_BAN_MESSAGE       = 0x67,
    SERVER_PACKET_SEND_BAN_USER             = 0x68,
    SERVER_PACKET_SEND_DEVELOPER_MESSAGE    = 0x69,
    SERVER_PACKET_KICK_USER                 = 0x6B,
    SERVER_PACKET_SET_ROOMLIST		        = 0x6C,
    SERVER_PACKET_ROOM_STATUS_MESSAGE       = 0x6D,
    SERVER_PACKET_PRIVATE_ROOM_CREATED      = 0x6E,
    SERVER_PACKET_DELETE_ROOM			    = 0x6F,
    SERVER_PACKET_NOT_ROOM_OWNER			= 0x70,
    SERVER_PACKET_CANNOT_LEAVE_ROOM			= 0x71,
    SERVER_PACKET_ROOM_NAME_TOO_SHORT	    = 0x72,
    SERVER_PACKET_ROOM_NAME_TOO_LONG 	    = 0x73,
    SERVER_PACKET_ROOM_INVITE_USER_NOT_FOUND = 0x74,
    SERVER_PACKET_ROOM_INVITE_NOT_YOUR_ROOM = 0x75,
    SERVER_PACKET_ROOM_CANNOT_INVITE_YOURSELF = 0x76,
    SERVER_PACKET_USER_JOINED_INVITED_ROOM  = 0x77,
    SERVER_PACKET_USER_LEFT_INVITED_ROOM	= 0x78, 
    SERVER_PACKET_NO_MORE_PRIVATE_ROOMS		= 0x79,
    SERVER_PACKET_CREATE_ROOM_WRONG_ARGUMENTS = 0x7A,
    SERVER_PACKET_USER_DOES_NOT_EXIST		= 0x7B,
    SERVER_PACKET_YOU_WHISPER			    = 0x7C,
    SERVER_PACKET_USER_WHISPERS_YOU		    = 0x7D,
};

namespace SteerStone { namespace Chat { namespace Server {

    class ChatSocket;

    /// Opcode Handler
    struct OpcodeHandler
    {
        char const* Name;
        PacketStatus Status;
        PacketProcess Process;
        void (ChatSocket::*Handler)(ClientPacket* p_Packet);
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
            OpcodeHandler const& GetClientPacket(const uint64& Id);
            /// Get Server Packet
            /// @p_Id : Id of server packet we are searching for
            OpcodeHandler const& GetServerPacket(const uint64& Id);

            std::string GetServerPacketHeader(uint64 const p_OpCode);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            /// Store Client packet into storage
            /// @p_Opcode : Opcode Id
            /// @p_Name : Name of Opcode
            /// @p_Status : Status of Opcode
            /// @p_Process : When to process the packet
            /// @p_Handler : Handler Function which we will be accessing too
            void StoreClientPacket(uint64 const p_OpCode, char const* p_Name, PacketStatus const p_Status, PacketProcess p_Process, void (ChatSocket::*handler)(ClientPacket* p_Packet));
            /// Store Server packet into storage
            /// @p_Opcode : Opcode Id
            /// @p_Name : Name of Opcode
            /// @p_Handler : Handler Function which we will be accessing too
            void StoreServerPacket(uint64 const p_OpCode, char const* p_Name, void (ChatSocket::* p_Handler)(ClientPacket* p_Packet));

        private:
            static OpcodeHandler const m_EmptyHandler;      ///< Empty handler if client packet has not been given a handler yet
            OpCodeMap m_ClientOpCodes;                      ///< Holds client packets
            OpCodeMap m_ServerOpCodes;                      ///< Holds server packets
    };

}   ///< namespace Packet
}   ///< namespace Chat
}   ///< namespace SteerStone

#define sOpCode SteerStone::Chat::Server::OpCodes::GetSingleton()