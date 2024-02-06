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

#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <iomanip>
#include <ctime>

#include "Utility/UtilMaths.hpp"

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
        m_CommandsHandler = new Commands::Handler();
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
    /// Return Player in chat
    /// @p_Id : Id of player
    Entity::Player* Base::FindPlayer(uint64 const p_Id)
    {
        for (auto l_Itr : m_Players)
        {
            if (l_Itr->GetId() == p_Id)
                return l_Itr;
        }

        return nullptr;
    }
    /// Find Player by Username
    /// @p_Username : Username of player
    Entity::Player* Base::FindPlayerByUsername(const std::string p_Username)
    {
        for (auto l_Itr : m_Players)
        {
			if (l_Itr->GetUsername() == p_Username)
				return l_Itr;
		}

		return nullptr;
	}

    /// Update the chat
    ///@ p_Diff : Time Diff
    void Base::Update(uint32 const p_Diff)
    {
        // Erase Remove Idiom: for removing players from the chat
        for (auto l_Itr = m_Players.begin(); l_Itr != m_Players.end();)
        {
            if (!(*l_Itr)->ToSocket() || (*l_Itr)->ToSocket()->IsClosed())
            {
                l_Itr = m_Players.erase(l_Itr);
            }
            else
            {
                (*l_Itr)->IntervalPing.Update(p_Diff);
                if ((*l_Itr)->IntervalPing.Passed())
                {
                    Server::Packets::Ping l_Packet;
                    (*l_Itr)->SendPacket(Server::Packets::Ping().Write());
                    (*l_Itr)->IntervalPing.Reset();
                }

				++l_Itr;
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

    /// Process Incoming Command
    ///@ p_Input : Command to process
    ///@ p_Player : Player who is sending the command
    void Base::ProcessCommand(const std::string& p_Input, Entity::Player const* p_Player)
    {
        m_CommandsHandler->HandleInput(p_Input , p_Player);
    }

    /// Add Process Command
    ///@ p_Id : Player Id
    ///@ p_Command : Command to process
    void Base::AddProcessCommand(const uint32 p_Id, const std::string p_Command, const nlohmann::json p_Json /*= nlohmann::json()*/)
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("INSERT INTO process_commands (type, user_id, processed, arguments, created_at, updated_at) VALUES (?, ?, ?, ?, NOW(), NOW())");
        l_PreparedStatement->SetString(0, p_Command);
        l_PreparedStatement->SetUint32(1, p_Id);
        l_PreparedStatement->SetBool(2, false);
        l_PreparedStatement->SetString(3, p_Json.dump());
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

    /// Ban Player
    /// @p_BannedUsername : Player who is banned
    /// @p_Player         : Player who banned the player
    /// @p_Reason         : Reason why player is banned
    /// @p_DaysHours      : How many days or hours the player is banned for
    void Base::BanPlayer(const std::string p_BannedUsername, Entity::Player const* p_Player, const std::string p_Reason, std::string p_DaysHours)
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id FROM users WHERE username = ?");
        l_PreparedStatement->SetString(0, p_BannedUsername);
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

            const uint64 p_BannedId = l_Result[0].GetUInt64();

            Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
            l_PreparedStatement->PrepareStatement("SELECT banned_user_id FROM chat_bans WHERE banned_user_id = ?");
            l_PreparedStatement->SetUint64(0, p_BannedId);
            l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

            if (!l_PreparedResultSet)
            {
                Entity::Player* l_BannedPlayer = sChatManager->FindPlayer(p_BannedId);

                /// Cannot ban an admin
                if (l_BannedPlayer->IsAdmin())
                {
                    p_Player->SendMessageToSelf("You cannot ban this player. The Player is an admin.");
                    // return;
                }

                char l_Type = p_DaysHours[p_DaysHours.size() - 1];

                if (l_Type != 'd' && l_Type != 'h')
                {
                    p_Player->SendMessageToSelf("Invalid type parameter.");
                    return;
                }
                
                p_DaysHours.pop_back();

                if (!Core::Utils::IsNumber(p_DaysHours))
                {
                    p_Player->SendMessageToSelf("Invalid days parameter.");
                    return;
                }

                uint32 l_DaysHours = l_Type == 'd' ? std::stoi(p_DaysHours) * 24 * 60 * 60 : std::stoi(p_DaysHours) * 24 * 60;
                std::time_t l_UTCTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::time_t l_ExpiresAt = l_UTCTime + l_DaysHours;

                char l_DateString[26];
                std::strftime(l_DateString, sizeof l_DateString, "%Y-%m-%d %H:%M:%S", localtime(&l_ExpiresAt));

                Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
                l_PreparedStatement->PrepareStatement("INSERT INTO chat_bans (banned_user_id, banned_by_user_id, reason, expires_at, updated_at, created_at) VALUES (?, ?, ?, ?, NOW(), NOW())");
                l_PreparedStatement->SetUint64(0, p_BannedId);
                l_PreparedStatement->SetUint64(1, p_Player->GetId());
                l_PreparedStatement->SetString(2, p_Reason);
                l_PreparedStatement->SetString(3, l_DateString);
                l_PreparedStatement->ExecuteStatement(true);

                if (l_BannedPlayer)
                {
                    Server::Packets::BanUser l_Packet;
                    l_BannedPlayer->SendPacket(l_Packet.Write());
                }

                p_Player->SendMessageToSelf("Player " + p_BannedUsername + " is banned.");
            }
            else
                p_Player->SendMessageToSelf("Player " + p_BannedUsername + " is already banned.");
        }
        else
            p_Player->SendMessageToSelf("Cannot find player " + p_BannedUsername);
    }

    /// Check if player is banned
    /// If the player is banned, then send packet to let know user is banned (if true)
    /// @p_Player       : Player who may be banned
    /// @p_SendPacket   : Send Packet to banned player to inform the they are banned.
    const bool Base::PlayerIsBanned(Entity::Player* p_Player, const bool p_SendPacket /* = true*/)
    {
        Core::Database::PreparedStatement* l_PreparedStatement = GameDatabase.GetPrepareStatement();
        l_PreparedStatement->PrepareStatement("SELECT id, reason, expires_at, created_at FROM chat_bans WHERE banned_user_id = ?");
        l_PreparedStatement->SetUint64(0, p_Player->GetId());
        std::unique_ptr<Core::Database::PreparedResultSet> l_PreparedResultSet = l_PreparedStatement->ExecuteStatement();

        if (l_PreparedResultSet)
        {
            if (p_SendPacket)
            {
                Core::Database::ResultSet* l_Result = l_PreparedResultSet->FetchResult();

                const std::string l_Reason = l_Result[1].GetString();
                const std::string l_ExpiryTime = l_Result[2].GetDateTimeToString();
                const std::string l_CreatedAt = l_Result[3].GetDateTimeToString();

                Server::Packets::BanMessage l_Packet;
                l_Packet.StartedAt = l_CreatedAt;
                l_Packet.EndsAt = l_ExpiryTime;
                p_Player->SendPacket(l_Packet.Write());
            }

            return true;
        }

        return false;
    }

    /// Get the total players
    uint32 const Base::TotalPlayers()
    {
        return m_Players.size();
    }
}
}
}