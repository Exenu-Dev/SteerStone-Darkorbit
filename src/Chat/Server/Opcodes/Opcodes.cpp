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
* along with this program.  If not, see <http://www.gnu.org/licenses/>.f
*/

#include "Socket.hpp"

namespace SteerStone { namespace Chat { namespace Server {

    SINGLETON_P_I(OpCodes);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    OpCodes::OpCodes()
    {

    }
    /// Deconstructor
    OpCodes::~OpCodes()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    OpcodeHandler const OpCodes::m_EmptyHandler =
    {
        "NULL",
        PacketStatus::STATUS_UNHANDLED,
        PacketProcess::PROCESS_NOW,
        &ChatSocket::HandleNULL
    };
    
    /// InitializePackets
    /// Load our packets into storages to be accessed later
    void OpCodes::InitializePackets()
    {
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_LOG_IN,          "CLIENT_PACKET_LOG_IN",         PacketStatus::STATUS_HANDLED, PacketProcess::PROCESS_NOW, &ChatSocket::HandleLogin          );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_SEND_MESSAGE,    "CLIENT_PACKET_SEND_MESSAGE",   PacketStatus::STATUS_HANDLED, PacketProcess::PROCESS_NOW, &ChatSocket::HandleSendMessage    );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKKET_PONG,           "CLIENT_PACKKET_PONG",          PacketStatus::STATUS_HANDLED, PacketProcess::PROCESS_NOW, &ChatSocket::HandlePong           );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_CHANGE_ROOM,     "CLIENT_PACKET_CHANGE_ROOM",    PacketStatus::STATUS_HANDLED, PacketProcess::PROCESS_NOW, &ChatSocket::HandleChangeRoom     );

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////        

        StoreServerPacket(ServerOpCodes::SERVER_PACKET_USER_LOGIN,              "SERVER_PACKET_USER_LOGIN",             &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SEND_MESSAGE,            "SERVER_PACKET_SEND_MESSAGE",           &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SEND_SYSTEM_MESSAGE,     "SERVER_PACKET_SEND_SYSTEM_MESSAGE",    &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SEND_PING,               "SERVER_PACKET_SEND_PING",              &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SEND_BAN_MESSAGE,        "SERVER_PACKET_SEND_BAN_MESSAGE",       &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SEND_IP_BAN_MESSAGE,     "SERVER_PACKET_SEND_IP_BAN_MESSAGE",    &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SEND_BAN_USER,           "SERVER_PACKET_SEND_BAN_USER",          &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SEND_DEVELOPER_MESSAGE,  "SERVER_PACKET_SEND_DEVELOPER_MESSAGE", &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SEND_ADMIN_MESSAGE,      "SERVER_PACKET_SEND_ADMIN_MESSAGE",     &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_KICK_USER,               "SERVER_PACKET_KICK_USER",              &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SET_ROOMLIST,            "SERVER_PACKET_SET_ROOMLIST",           &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_ROOM_STATUS_MESSAGE,     "SERVER_PACKET_ROOM_STATUS_MESSAGE",    &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_PRIVATE_ROOM_CREATED,    "SERVER_PACKET_PRIVATE_ROOM_CREATED",   &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_DELETE_ROOM,             "SERVER_PACKET_DELETE_ROOM",            &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_NOT_ROOM_OWNER,          "SERVER_PACKET_NOT_ROOM_OWNER",         &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CANNOT_LEAVE_ROOM,       "SERVER_PACKET_CANNOT_LEAVE_ROOM",      &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_ROOM_NAME_TOO_SHORT,     "SERVER_PACKET_ROOM_NAME_TOO_SHORT",    &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_ROOM_NAME_TOO_LONG,      "SERVER_PACKET_ROOM_NAME_TOO_LONG",     &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_ROOM_INVITE_USER_NOT_FOUND,  "SERVER_PACKET_ROOM_INVITE_USER_NOT_FOUND",         &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_ROOM_CANNOT_INVITE_YOURSELF, "SERVER_PACKET_ROOM_CANNOT_INVITE_YOURSELF",        &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_USER_JOINED_INVITED_ROOM,    "SERVER_PACKET_USER_JOINED_INVITED_ROOM",           &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_NO_MORE_PRIVATE_ROOMS,       "SERVER_PACKET_NO_MORE_PRIVATE_ROOMS",              &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CREATE_ROOM_WRONG_ARGUMENTS, "SERVER_PACKET_CREATE_ROOM_WRONG_ARGUMENTS",        &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_USER_DOES_NOT_EXIST,         "SERVER_PACKET_USER_DOES_NOT_EXIST",        &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_YOU_WHISPER,                 "SERVER_PACKET_YOU_WHISPER",        &ChatSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_USER_WHISPERS_YOU,           "SERVER_PACKET_USER_WHISPERS_YOU",        &ChatSocket::HandleServer);

        LOG_INFO("OpCodes", "Loaded %0 Client Packets", m_ClientOpCodes.size());
        LOG_INFO("OpCodes", "Loaded %0 Server Packets", m_ServerOpCodes.size());
    } 
    
    /// Store Client packet into storage
    /// @p_Opcode : Opcode Id
    /// @p_Name : Name of Opcode
    /// @p_Status : Status of Opcode
    /// @p_Process : When to process the packet
    /// @p_Handler : Handler Function which we will be accessing too
    void OpCodes::StoreClientPacket(uint64 const p_OpCode, char const* p_Name, PacketStatus const p_Status, PacketProcess p_Process, void(ChatSocket::* p_Handler)(ClientPacket* p_Packet))
    {
        OpcodeHandler& l_Ref = m_ClientOpCodes[p_OpCode];
        l_Ref.Name = p_Name;
        l_Ref.Status = p_Status;
        l_Ref.Handler = p_Handler;
    }
    /// Store Server packet into storage
    /// @p_Opcode : Opcode Id
    /// @p_Name : Name of Opcode
    /// @p_Handler : Handler Function which we will be accessing too
    void OpCodes::StoreServerPacket(uint64 const p_OpCode, char const* p_Name, void(ChatSocket::* p_Handler)(ClientPacket* p_Packet))
    {
        OpcodeHandler& l_Ref = m_ServerOpCodes[p_OpCode];
        l_Ref.Name = p_Name;
        l_Ref.Handler = p_Handler;
    }

    /// Get Client Packet
    /// @p_Id : Id of client packet we are searching for
    OpcodeHandler const& OpCodes::GetClientPacket(const uint64& Id)
    {
        auto const l_Itr = m_ClientOpCodes.find(Id);
        if (l_Itr != m_ClientOpCodes.end())
            return l_Itr->second;
        return m_EmptyHandler;
    }
    /// Get Server Packet
    /// @p_Id : Id of server packet we are searching for
    OpcodeHandler const& OpCodes::GetServerPacket(const uint64& Id)
    {
        auto const l_Itr = m_ServerOpCodes.find(Id);
        if (l_Itr != m_ServerOpCodes.end())
            return l_Itr->second;
        return m_EmptyHandler;
    }

    std::string OpCodes::GetServerPacketHeader(uint64 const p_OpCode)
    {
        switch (p_OpCode)
        {
            case ServerOpCodes::SERVER_PACKET_USER_LOGIN:
            {
				return "bv";
			}
            break;
            case ServerOpCodes::SERVER_PACKET_SEND_PING:
            {
                return "el";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_SEND_BAN_MESSAGE:
            {
                return "db";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_SEND_IP_BAN_MESSAGE:
            {
                return "ey";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_SEND_BAN_USER:
            {
                return "at";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_SEND_MESSAGE:
            {
                return "a";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_SEND_DEVELOPER_MESSAGE:
            {
                return "dq";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_SEND_ADMIN_MESSAGE:
            {
                return "j";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_KICK_USER:
            {
				return "cd";
			}
            break;
            case ServerOpCodes::SERVER_PACKET_SET_ROOMLIST:
            {
                return "by";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_ROOM_STATUS_MESSAGE:
            {
                return "c";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_PRIVATE_ROOM_CREATED:
            {
				return "cj";
			}
            break;
            case ServerOpCodes::SERVER_PACKET_DELETE_ROOM:
            {
				return "ck";
			}
            break;
            case ServerOpCodes::SERVER_PACKET_NOT_ROOM_OWNER:
            {
                return "ec";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_CANNOT_LEAVE_ROOM:
            {
                return "dz";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_ROOM_NAME_TOO_SHORT:
            {
                return "ch";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_ROOM_NAME_TOO_LONG:
            {
                return "ed";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_ROOM_INVITE_USER_NOT_FOUND:
            {
                return "cp";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_ROOM_INVITE_NOT_YOUR_ROOM:
            {
                return "co";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_ROOM_CANNOT_INVITE_YOURSELF:
            {
                return "cn";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_USER_JOINED_INVITED_ROOM:
            {
                return "ea";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_USER_LEFT_INVITED_ROOM:
            {
                return "eb";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_NO_MORE_PRIVATE_ROOMS:
            {
                return "cf";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_CREATE_ROOM_WRONG_ARGUMENTS:
            {
                return "cg";
            }
            case ServerOpCodes::SERVER_PACKET_USER_DOES_NOT_EXIST:
            {
                return "ct";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_YOU_WHISPER:
            {
                return "cw";
            }
            break;
            case ServerOpCodes::SERVER_PACKET_USER_WHISPERS_YOU:
            {
                return "cv";
            }
            break;
            default:
                LOG_WARNING("ServerPacket", "Unknown Server Opcode: %0", p_OpCode);
            break;
        }

        return "";
    }

}   ///< namespace Packet
}   ///< namespace Chat
}   ///< namespace SteerStone