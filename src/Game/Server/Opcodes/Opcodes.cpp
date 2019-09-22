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
* along with this program.  If not, see <http://www.gnu.org/licenses/>.f
*/

#include "Socket.hpp"
#include <iomanip>

namespace SteerStone { namespace Game { namespace Server {

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
        PacketProcess::PROCESS_PLAYER_THREAD,
        &GameSocket::HandleNULL
    };
    
    /// InitializePackets
    /// Load our packets into storages to be accessed later
    void OpCodes::InitializePackets()
    {
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_LOGIN,             "CLIENT_PACKET_LOGIN",              PacketStatus::STATUS_AUTHENTICATION, PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleLogin               );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_CHANGE_LASER,      "CLIENT_PACKET_CHANGE_LASER",       PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_WORLD_THREAD,  &GameSocket::HandleChangeLaser         );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_CHANGE_ROCKET,     "CLIENT_PACKET_CHANGE_ROCKET",      PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_WORLD_THREAD,  &GameSocket::HandleChangeRocket        );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_PING,              "CLIENT_PACKET_PING",               PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleClient              );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_OBJECT_MOVE,       "CLIENT_PACKET_OBJECT_MOVE",        PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleObjectMove          );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_INITIALIZE_PLAYER, "CLIENT_PACKET_INITIALIZE_PLAYER",  PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_MAP_THREAD,    &GameSocket::HandleInitializePlayer    );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_PORTAL_JUMP,       "CLIENT_PACKET_PORTAL_JUMP",        PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_WORLD_THREAD,    &GameSocket::HandlePortalJump          );
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_TRADE_ORE,         "CLIENT_PACKET_TRADE_ORE",          PacketStatus::STATUS_LOGGED_IN,      PacketProcess::PROCESS_PLAYER_THREAD, &GameSocket::HandleNULL                );

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        StoreServerPacket(ServerOpCodes::SERVER_PACKET_PLAYER_INFO,         "SERVER_PACKET_PLAYER_INFO",        &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_INITIALIZE_SHIP,     "SERVER_PACKET_INITIALIZE_SHIP",    &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_MAP_UPDATE,          "SERVER_PACKET_MAP_UPDATE",         &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_MINI_MAP_UPDATE,     "SERVER_PACKET_MINI_MAP_UPDATE",    &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_UPDATE_ROCKET_MINE,  "SERVER_PACKET_MISC_AMMO_UPDATE",   &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_UPDATE_BATTERY,      "SERVER_PACKET_UPDATE_BATTERY",     &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_LOGGED_IN,           "SERVER_PACKET_LOGGED_IN",          &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SPAWN_SHIP,          "SERVER_PACKET_SPAWN_SHIP",         &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_DESPAWN_SHIP,        "SERVER_PACKET_DESPAWN_SHIP",       &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_OBJECT_MOVE,         "SERVER_PACKET_OBJECT_MOVE",        &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CREATE_PORTAL,       "SERVER_PACKET_CREATE_PORTAL",      &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_CREATE_STATION,      "SERVER_PACKET_CREATE_STATION",     &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_SEND_MESSAGE,        "SERVER_PACKET_SEND_MESSAGE",       &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_EVENT,               "SERVER_PACKET_EVENT",              &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_JUMP_PORTAL,         "SERVER_PACKET_SEND_MESSAGE",       &GameSocket::HandleServer);
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_DISPLAY_STAR_SYSTEM, "SERVER_PACKET_DISPLAY_STAR_SYSTEM",&GameSocket::HandleServer);

        /// Debug Headless Packets
        StoreServerPacket(ServerOpCodes::SERVER_PACKET_HEADLESS_MOVE,       "SERVER_PACKET_HEADLESS_MOVE",      &GameSocket::HandleServer);

        LOG_INFO("OpCodes", "Loaded %0 Client Packets", m_ClientOpCodes.size());
        LOG_INFO("OpCodes", "Loaded %0 Server Packets", m_ServerOpCodes.size());
    } 
    
    /// Store Client packet into storage
    /// @p_Opcode : Opcode Id
    /// @p_Name : Name of Opcode
    /// @p_Status : Status of Opcode
    /// @p_Process : When to process the packet
    /// @p_Handler : Handler Function which we will be accessing too
    void OpCodes::StoreClientPacket(uint8 const p_OpCode, char const* p_Name, PacketStatus const p_Status, PacketProcess p_Process, void(GameSocket::* p_Handler)(ClientPacket* p_Packet))
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
    void OpCodes::StoreServerPacket(uint8 const p_OpCode, char const* p_Name, void(GameSocket::* p_Handler)(ClientPacket* p_Packet))
    {
        OpcodeHandler& l_Ref = m_ServerOpCodes[p_OpCode];
        l_Ref.Name = p_Name;
        l_Ref.Handler = p_Handler;
    }

    /// Get Client Packet
    /// @p_Id : Id of client packet we are searching for
    OpcodeHandler const* OpCodes::GetClientPacket(ClientOpCodes p_Id)
    {
        return &m_ClientOpCodes[p_Id];
    }
    /// Get Server Packet
    /// @p_Id : Id of server packet we are searching for
    OpcodeHandler const* OpCodes::GetServerPacket(ServerOpCodes p_Id)
    {
        return &m_ServerOpCodes[p_Id];
    }

    /// Get Client Opcode Name
    std::string OpCodes::GetClientOpCodeName(ClientOpCodes p_Opcode)
    {
        std::ostringstream l_StringStream;
        l_StringStream << '[';

        if (static_cast<uint32>(p_Opcode) < ClientOpCodes::CLIENT_MAX_OPCODE)
        {
            if (OpcodeHandler const* l_Handler = GetClientPacket(p_Opcode))
                l_StringStream << l_Handler->Name;
            else
                l_StringStream << "UNKNOWN OPCODE";
        }
        else
            l_StringStream << "INVALID OPCODE";

        l_StringStream << " (" << p_Opcode << ")]";
        return l_StringStream.str();
    }
    /// Get Server Opcode Name
    std::string OpCodes::GetServerOpCodeName(ServerOpCodes p_Opcode)
    {
        std::ostringstream l_StringStream;
        l_StringStream << '[';

        if (static_cast<uint32>(p_Opcode) < ClientOpCodes::CLIENT_MAX_OPCODE)
        {
            if (OpcodeHandler const* l_Handler = GetServerPacket(p_Opcode))
                l_StringStream << l_Handler->Name;
            else
                l_StringStream << "UNKNOWN OPCODE";
        }
        else
            l_StringStream << "INVALID OPCODE";

        l_StringStream << " (" << p_Opcode << ")]";
        return l_StringStream.str();
    }

}   ///< namespace Packet
}   ///< namespace Game
}   ///< namespace SteerStone