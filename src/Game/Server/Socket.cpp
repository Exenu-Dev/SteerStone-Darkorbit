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

#include "Socket.hpp"
#include "Player.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Constructor 
    /// @p_Service : Boost Service
    /// @p_CloseHandler : Close Handler Custom function
    GameSocket::GameSocket(boost::asio::io_service& p_Service, std::function<void(Socket*)> p_CloseHandler)
        : Socket(p_Service, std::move(p_CloseHandler))
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Handle incoming data
    bool GameSocket::ProcessIncomingData()
    {
        std::vector<uint8> l_BufferVec;
        l_BufferVec.resize(ReadLengthRemaining());

        /// Read the entire buffer
        if (Read((char*)& l_BufferVec[0], ReadLengthRemaining()))
        {
            /// Remove junk data
            l_BufferVec.resize(l_BufferVec.size() + 1);
            l_BufferVec[l_BufferVec.size() - 1] = 0;

            /// Form packet and execute handler
            ClientPacket* l_ClientPacket        = new ClientPacket((char*)& l_BufferVec[0]);
            OpcodeHandler const l_OpcodeHandler = sOpCode->GetClientPacket(l_ClientPacket->GetHeader());
            ExecutePacket(&l_OpcodeHandler, l_ClientPacket);

            return true;
        }

        LOG_WARNING("GameSocket", "Failed to read incoming buffer!");

        return false;
    }

    /// Handle Client Packet Handler
    /// @p_OpCodeHandler : Handler of Client Packet
    /// @p_Packet        : Client Packet
    void GameSocket::ExecutePacket(const OpcodeHandler* p_OpCodeHandler, ClientPacket* p_Packet)
    {
        if (!p_OpCodeHandler || !p_Packet)
        {
            LOG_WARNING("GameSocket", "Couldn't execute packet. Handler or ClientPacket is null!");
            return;
        }

        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("GameSocket", "Received Packet <%0> from %1", p_OpCodeHandler->Name, GetRemoteAddress());
        #endif

        switch (p_OpCodeHandler->Process)
        {
            case PacketProcess::PROCESS_NOW:
            {
                (this->*p_OpCodeHandler->Handler)(p_Packet);
            }
            break;
            default:
                break;
        }

    }

    /// Send packet to client
    /// @p_PacketBuffer : Packet Buffer
    void GameSocket::SendPacket(const PacketBuffer* p_PacketBuffer)
    {
        if (IsClosed())
            return;

        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("GameSocket", "Sending Packet <%0> to %1", sOpCode->GetServerPacket(p_PacketBuffer->GetContents()[0]).Name, GetRemoteAddress());
        #endif

        Write((const char*)p_PacketBuffer->GetContents(), p_PacketBuffer->GetSize());
    }
    /// For Non-Implemented packets
    void GameSocket::HandleNULL(ClientPacket* p_Packet)
    {
        LOG_WARNING("GameSocket", "Received unhandled packet. Header: %0", p_Packet->GetHeader());
    }
    /// Pointer to Player
    Entity::Player* GameSocket::ToPlayer()
    {
        return m_Player;
    }
    /// Delete Player
    void GameSocket::DestroyPlayer()
    {
        m_Player = nullptr;
    }

}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace Steerstone