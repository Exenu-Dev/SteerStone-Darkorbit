/*
* Liam Ashdown
* Copyright (C) 2021
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

namespace SteerStone { namespace Policy { namespace Server {

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
        &PolicySocket::HandleNULL
    };
    
    /// InitializePackets
    /// Load our packets into storages to be accessed later
    void OpCodes::InitializePackets()
    {
        StoreClientPacket(ClientOpCodes::CLIENT_PACKET_POLICY, "CLIENT_PACKET_POLICY", PacketStatus::STATUS_AUTHENTICATION, PacketProcess::PROCESS_NOW, &PolicySocket::HandlePolicy);
        
        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////        

        StoreServerPacket(ServerOpCodes::SERVER_PACKET_POLICY, "SERVER_PACKET_POLICY", &PolicySocket::HandleServer);

        LOG_INFO("OpCodes", "Loaded %0 Client Packets", m_ClientOpCodes.size());
        LOG_INFO("OpCodes", "Loaded %0 Server Packets", m_ServerOpCodes.size());
    } 
    
    /// Store Client packet into storage
    /// @p_Opcode : Opcode Id
    /// @p_Name : Name of Opcode
    /// @p_Status : Status of Opcode
    /// @p_Process : When to process the packet
    /// @p_Handler : Handler Function which we will be accessing too
    void OpCodes::StoreClientPacket(uint64 const p_OpCode, char const* p_Name, PacketStatus const p_Status, PacketProcess p_Process, void(PolicySocket::* p_Handler)(ClientPacket* p_Packet))
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
    void OpCodes::StoreServerPacket(uint64 const p_OpCode, char const* p_Name, void(PolicySocket::* p_Handler)(ClientPacket* p_Packet))
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

}   ///< namespace Packet
}   ///< namespace Policy
}   ///< namespace SteerStone