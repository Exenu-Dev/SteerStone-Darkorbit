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
        m_Player = nullptr;

        m_PingStopWatch.Start();
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

            ClientOpCodes l_Opcode = static_cast<ClientOpCodes>(l_BufferVec[0]);

            switch (l_Opcode)
            {
                case ClientOpCodes::CLIENT_PACKET_LOGIN:
                {
                    if (m_Player)
                    {
                        LOG_WARNING("GameSocket", "Tried to process packet CLIENT_PACKET_LOGIN but player is already logged in!");
                        return false;
                    }

                    HandleLoginPacket(new ClientPacket((char*)& l_BufferVec[0]));
                }
                break;
                case ClientOpCodes::CLIENT_PACKET_PING:
                {

                    HandleLoginPacket(new ClientPacket((char*)& l_BufferVec[0]));
                }
                break;
                default:
                {
                    if (l_Opcode > ClientOpCodes::CLIENT_MAX_OPCODE)
                    {
                        LOG_WARNING("GameSocket", "Recieved opcode %0 which is larger than max opcode! closing socket!", l_Opcode);
                        return false;
                    }

                    OpcodeHandler const* l_OpCodeHandler = sOpCode->GetClientPacket(l_Opcode);
                    if (!l_OpCodeHandler)
                    {
                        LOG_ERROR("GameSocket", "No defined handler for opcode %0 sent by %1", sOpCode->GetClientOpCodeName(l_Opcode), GetRemoteAddress());
                        return false;
                    }

                    #ifdef STEERSTONE_CORE_DEBUG
                        else
                            LOG_INFO("GameSocket", "Received packet %0 from %1", sOpCode->GetClientOpCodeName(l_Opcode), GetRemoteAddress());
                    #endif

                    switch (l_OpCodeHandler->Status)
                    {
                        case PacketStatus::STATUS_AUTHENTICATION:
                        {
                            if (m_Player || m_Player->IsLoggedIn())
                            {
                                LOG_WARNING("GameSocket", "Recieved opcode %0 from player %1 but player is already in world!", sOpCode->GetClientOpCodeName(l_Opcode), m_Player->GetName());
                                return true;
                            }
                        }
                        break;
                        case PacketStatus::STATUS_LOGGED_IN:
                        {
                            if (!m_Player || !m_Player->IsLoggedIn())
                            {
                                LOG_WARNING("GameSocket", "Recieved opcode %0 from player %1 but player is not in world!", sOpCode->GetClientOpCodeName(l_Opcode), m_Player->GetName());
                                return true;
                            }
                        }
                        break;
                        case PacketStatus::STATUS_UNHANDLED:
                        {
                            ;///< TODO; What should we put here
                        }
                        break;
                    }

                    if (l_OpCodeHandler->Process == PacketProcess::PROCESS_PLAYER_THREAD)
                        ExecutePacket(l_OpCodeHandler, new ClientPacket((char*)& l_BufferVec[0]));
                    else
                        m_Player->QueuePacket(new ClientPacket((char*)& l_BufferVec[0]));
                }
                break;
            }

            return true;
        }

        LOG_WARNING("GameSocket", "Failed to read incoming buffer!");

        return false;
    }

    /// Handle Initial part of logging into game server
    /// @p_Packet        : Client Packet
    void GameSocket::HandleLoginPacket(ClientPacket* p_Packet)
    {
        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("GameSocket", "Received packet %0 from %1", sOpCode->GetClientOpCodeName(static_cast<ClientOpCodes>(p_Packet->GetHeader())), GetRemoteAddress());
        #endif

        (this->*sOpCode->GetClientPacket(ClientOpCodes::CLIENT_PACKET_LOGIN)->Handler)(p_Packet);

        delete p_Packet;
    }
    /// Ping Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandlePing(ClientPacket* p_Packet)
    {
        /// TODO; Code Ping
        m_PingStopWatch.Reset();
    }
    /// Handle Client Packet Handler
    /// @p_OpCodeHandler : Handler of Client Packet
    /// @p_Packet        : Client Packet
    void GameSocket::ExecutePacket(OpcodeHandler const* p_OpCodeHandler, ClientPacket* p_Packet)
    {
        (this->*p_OpCodeHandler->Handler)(p_Packet);
    }
    /// Send packet to client
    /// @p_PacketBuffer : Packet Buffer
    void GameSocket::SendPacket(const PacketBuffer* p_PacketBuffer)
    {
        if (IsClosed())
            return;

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