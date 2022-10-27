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

#include "ChatManager.hpp"
#include "Database/DatabaseTypes.hpp"
#include "Packets/Server/ChatPacket.hpp"

namespace SteerStone { namespace Chat { namespace Channel {

    SINGLETON_P_I(Base);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    Base::Base()
    {
    }
    /// Deconstructor
    Base::~Base()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Static StopChat
    volatile bool Base::s_StopChat = false;

    /// Add Player
    ///@ p_Player : Player
    void Base::AddPlayer(Entity::Player* p_Player)
    {
        m_Players.insert(p_Player);
    }

    /// Update the chat
    ///@ p_Diff : Time Diff
    void Base::Update(uint32 const p_Diff)
    {
        for (auto l_Itr : m_Players)
        {
            if (!l_Itr->ToSocket() || l_Itr->ToSocket()->IsClosed())
            {
                m_Players.erase(l_Itr);
                continue;
            }

            l_Itr->IntervalPing.Update(p_Diff);
            if (l_Itr->IntervalPing.Passed())
            {
                Server::Packets::Ping l_Packet;
                l_Itr->SendPacket(Server::Packets::Ping().Write());
                l_Itr->IntervalPing.Reset();
            }
        }
    }
    /// Stop World Updating
    bool Base::StopWorld() const
    {
        return s_StopChat;
    }

    /// Send Message
    ///@ p_Player : Player who is sending the message
    ///@ p_Message : Message to send
    ///@ p_RoomId: Room Id to send to
    void Base::SendMessageToRoom(Entity::Player const* p_Player, std::string const p_Message, uint16 const p_RoomId)
    {
        for (auto l_Itr : m_Players)
        {
            if (l_Itr->IsInRoom(p_RoomId))
            {
                if (p_Player->IsAdmin())
                {
                    Server::Packets::SendAdminMessage l_Packet;
                    l_Packet.RoomId = p_RoomId;
                    l_Packet.Username = p_Player->GetUsername();
                    l_Packet.Message = p_Message;
                    l_Itr->SendPacket(l_Packet.Write());
                }
                else
                {
                    Server::Packets::SendMessageToRoom l_Packet;
                    l_Packet.RoomId = p_RoomId;
                    l_Packet.Username = p_Player->GetUsername();
                    l_Packet.Message = p_Message;
                    l_Itr->SendPacket(l_Packet.Write());
                }
            }
        }
    }

    /// Add Process Command
    ///@ p_Id : Player Id
    ///@ p_Command : Command to process
    void Base::AddProcessCommand(const uint32 p_Id, const std::string p_Command)
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("INSERT INTO process_commands (type, user_id, processed, created_at, updated_at) VALUES (?, ?, ?, NOW(), NOW())");
        l_PreparedStatement->SetString(0, p_Command);
        l_PreparedStatement->SetUint32(1, p_Id);
        l_PreparedStatement->SetBool(2, false);
        l_PreparedStatement->ExecuteStatement(true);

    }

    /// Send System Message
    /// @p_Message : System Message
    void Base::SendSystemMessage(const std::string p_Message)
    {
        Server::Packets::SystemMessage l_SystemMessagePacket;
        l_SystemMessagePacket.Message = p_Message;

        for (auto l_Itr : m_Players)
        {
            l_Itr->SendPacket(l_SystemMessagePacket.Write());
        }
    }

    /// Get the total players
    uint32 const Base::TotalPlayers()
    {
        return m_Players.size();
    }
}
}
}