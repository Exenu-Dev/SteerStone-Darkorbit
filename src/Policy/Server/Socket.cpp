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
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Socket.hpp"

namespace SteerStone { namespace Policy { namespace Server {

    /// Constructor 
    /// @p_Service : Boost Service
    /// @p_CloseHandler : Close Handler Custom function
    PolicySocket::PolicySocket(boost::asio::io_service& p_Service, std::function<void(Socket*)> p_CloseHandler)
        : Socket(p_Service, std::move(p_CloseHandler))
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Handle incoming data
    Core::Network::ProcessState PolicySocket::ProcessIncomingData()
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
            ClientPacket l_ClientPacket((char*)& l_BufferVec[0]);
            OpcodeHandler const l_OpcodeHandler = sOpCode->GetClientPacket(l_ClientPacket.GetHeader());
            ExecutePacket(&l_OpcodeHandler, &l_ClientPacket);

            return Core::Network::ProcessState::Successful;
        }

        return Core::Network::ProcessState::Error;
    }

    /// Handle Client Packet Handler
    /// @p_OpCodeHandler : Handler of Client Packet
    /// @p_Packet        : Client Packet
    void PolicySocket::ExecutePacket(const OpcodeHandler* p_OpCodeHandler, ClientPacket* p_Packet)
    {
        if (!p_OpCodeHandler || !p_Packet)
        {
            LOG_WARNING("PolicySocket", "Couldn't execute packet. Handler or ClientPacket is null!");
            return;
        }

        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("PolicySocket", "Received Packet <%0> from %1", p_OpCodeHandler->Name, GetRemoteAddress());
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
    void PolicySocket::SendPacket(const PacketBuffer* p_PacketBuffer)
    {
        if (IsClosed())
            return;

        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("PolicySocket", "Sending Packet <%0> to %1", sOpCode->GetServerPacket(p_PacketBuffer->GetContents()[0]).Name, GetRemoteAddress());
        #endif

        Write((const char*)p_PacketBuffer->GetContents(), p_PacketBuffer->GetSize(), true);
    }

    /// For Non-Implemented packets
    void PolicySocket::HandleNULL(ClientPacket* p_Packet)
    {
        LOG_WARNING("PolicySocket", "Received unhandled packet. Header: %0", p_Packet->GetHeader());
    }

}   ///< namespace Server
}   ///< namespace Policy
}   ///< namespace Steerstone