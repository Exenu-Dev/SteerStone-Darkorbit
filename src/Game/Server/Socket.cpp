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

#include <boost/algorithm/string.hpp>

#include "Config/Config.hpp"
#include "Socket.hpp"
#include "World.hpp"
#include "Player.hpp"

namespace SteerStone { namespace Game { namespace Server {

    /// Constructor 
    /// @p_Service : Boost Service
    /// @p_CloseHandler : Close Handler Custom function
    GameSocket::GameSocket(boost::asio::io_service& p_Service, std::function<void(Socket*)> p_CloseHandler)
        : Socket(p_Service, std::move(p_CloseHandler))
    {
        m_AuthenticateState = Authenticated::NotAuthenticated;
        m_Player = nullptr;

        m_PingStopWatch.Start();
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Handle incoming data
    Core::Network::ProcessState GameSocket::ProcessIncomingData()
    {
        std::vector<uint8> l_BufferVec;
        l_BufferVec.resize(ReadLengthRemaining());

        /// Read the entire buffer
        if (Read((char*)& l_BufferVec[0], ReadLengthRemaining()))
        {
            /// Remove junk data
            l_BufferVec.resize(l_BufferVec.size() + 1);
            l_BufferVec[l_BufferVec.size() - 1] = 0;

            for (auto l_Itr : Core::Utils::SplitAll((char*)& l_BufferVec[0], "\n", false))
            {
                /// Note; Since we handle packet headers as 1 byte, the client sends LOGIN as the packet header, so we need to handle
                /// it differently
                if (m_AuthenticateState == Authenticated::NotAuthenticated)
                {
                    std::string l_Buffer = (char*)& l_BufferVec[0];

                    if (!CheckForWebPacket(l_Buffer))
                    {
                        l_Buffer.resize(5);

                        if (l_Buffer == "LOGIN")
                        {
                            HandleLoginPacket(new ClientPacket((char*)&l_BufferVec[0]));

                            /// The "!" is the 'fake' packet header, see HandleLoginPacket function which explains why I am doing this
                            m_Player->QueuePacket(new ClientPacket("!|" + (std::string)(char*)&l_BufferVec[0]));

                            m_AuthenticateState = Authenticated::Authenticed;

                            return Core::Network::ProcessState::Successful;
                        }
                    }
                    else
                        return Core::Network::ProcessState::Successful;

                    return Core::Network::ProcessState::Error;
                }

                ClientOpCodes l_Opcode = static_cast<ClientOpCodes>(l_BufferVec[0]);

                switch (l_Opcode)
                {
                case ClientOpCodes::CLIENT_PACKET_PING:
                    HandlePingPacket(new ClientPacket((char*)& l_BufferVec[0]));
                    break;
                default:
                {
                    OpcodeHandler const* l_OpCodeHandler = sOpCode->GetClientPacket(l_Opcode);

                    if (!l_OpCodeHandler)
                    {
                        LOG_ERROR("GameSocket", "No defined handler for opcode %0 sent by %1", sOpCode->GetClientOpCodeName(l_Opcode), GetRemoteAddress());
                        return Core::Network::ProcessState::Skip;
                    }

                #ifdef STEERSTONE_CORE_DEBUG
                    else
                        LOG_INFO("GameSocket", "Received packet %0 from %1", sOpCode->GetClientOpCodeName(l_Opcode), GetRemoteAddress());
                #endif

                    switch (l_OpCodeHandler->Status)
                    {
                    case PacketStatus::STATUS_AUTHENTICATION:
                    {
                        if (!m_Player || m_Player->IsLoggedIn())
                        {
                            LOG_WARNING("GameSocket", "Recieved opcode %0 from player %1 but player is already in world!", sOpCode->GetClientOpCodeName(l_Opcode), m_Player->GetName());
                            return Core::Network::ProcessState::Skip;
                        }
                    }
                    break;
                    case PacketStatus::STATUS_LOGGED_IN:
                    {
                        if (!m_Player || !m_Player->IsLoggedIn())
                        {
                            LOG_WARNING("GameSocket", "Recieved opcode %0 from player %1 but player is not in world!", sOpCode->GetClientOpCodeName(l_Opcode), m_Player->GetName());
                            return Core::Network::ProcessState::Skip;
                        }
                    }
                    break;
                    case PacketStatus::STATUS_UNHANDLED:
                    {
                        ;///< TODO; What should we put here
                    }
                    break;
                    }

                    /// Note; Player thread is the network thread
                    if (l_OpCodeHandler->Process == PacketProcess::PROCESS_PLAYER_THREAD)
                        ExecutePacket(l_OpCodeHandler, new ClientPacket((char*)& l_BufferVec[0]));
                    else
                        m_Player->QueuePacket(new ClientPacket((char*)& l_BufferVec[0]));
                }
                break;
                }

                return Core::Network::ProcessState::Successful;
            }
        }

        return Core::Network::ProcessState::Successful;
    }

    /// Check for Web Packet
    /// @p_Buffer : Buffer of packet
    bool GameSocket::CheckForWebPacket(std::string p_Buffer)
    {
        /// Check If Web interaction is enabled
        static const bool l_WebRequestEnabled = sConfigManager->GetBool("WebEnabled", false);

        if (!l_WebRequestEnabled)
            return false;

        /// Secret Key
        static const std::string l_SecretKey = sConfigManager->GetString("WebSecretKey").c_str();

        bool l_Result = boost::algorithm::contains(p_Buffer, l_SecretKey);

        /// It is a web packet, lets handle it
        if (l_Result)
        {
            /// First we need to strip the secret key
            std::string l_Buffer = SteerStone::Core::Utils::String::ReplaceAll(p_Buffer, l_SecretKey + '|', "");
            int l_Header = l_Buffer[0];

            ClientOpCodes l_Opcode = static_cast<ClientOpCodes>(*(char*)&l_Header);

            OpcodeHandler const* l_OpCodeHandler = sOpCode->GetClientPacket(l_Opcode);

            if (!l_OpCodeHandler->Name)
                return true;


            ExecutePacket(l_OpCodeHandler, new ClientPacket(l_Buffer));

            return true;
        }

        return false;
    }

    /// Handle Initial part of logging into game server
    /// @p_Packet        : Client Packet
    void GameSocket::HandleLoginPacket(ClientPacket* p_Packet)
    {
        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("GameSocket", "Received packet %0 from %1", sOpCode->GetClientOpCodeName(static_cast<ClientOpCodes>(p_Packet->GetHeader())), GetRemoteAddress());
        #endif

        /// This is a cheeky way of doing it
        /// The issue is, if we add the player to the map on player thread,
        /// there will be a race condition, so initialize the player and then pass the packet to the world thread
        /// the world thread gets updated before the map thread
        m_Player              = new Entity::Player(this);
        m_Player->m_Id        = p_Packet->ReadInt32();
        m_Player->m_SessionId = p_Packet->ReadString();

        if (!m_Player->LoadFromDB())
        {
            LOG_WARNING("Player", "Failed to load player details from database. User Id: %0", m_Player->GetId());
            CloseSocket(); ///< Close socket, nothing we can do without having player stored info
        }
        else
            sWorldManager->AddPlayer(m_Player);
    }
    /// Ping Handler
    /// @p_ClientPacket : Packet recieved from client
    void GameSocket::HandlePingPacket(ClientPacket* p_Packet)
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

        #ifdef STEERSTONE_CORE_DEBUG
            LOG_INFO("GameSocket", "Sent packet %0 to %1", sOpCode->GetServerOpCodeName(static_cast<ServerOpCodes>(p_PacketBuffer->GetHeader())), GetRemoteAddress());
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