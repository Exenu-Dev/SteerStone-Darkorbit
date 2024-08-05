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
#include "Utility/UtilRandom.hpp"

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

        LoadRooms();
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
    /// Load the standard default rooms
    void Base::LoadRooms()
    {
        m_Rooms[RoomId::ROOM_ID_GLOBAL]         = new Room(RoomId::ROOM_ID_GLOBAL, "Global", 1, Company::NOMAD, RoomType::ROOM_TYPE_NORMAL);
        m_Rooms[RoomId::ROOM_ID_MMO]            = new Room(RoomId::ROOM_ID_MMO, "MMO", 2, Company::MMO, RoomType::ROOM_TYPE_NORMAL);
        m_Rooms[RoomId::ROOM_ID_EIC]            = new Room(RoomId::ROOM_ID_EIC, "EIC", 2, Company::EARTH, RoomType::ROOM_TYPE_NORMAL);
        m_Rooms[RoomId::ROOM_ID_VRU]            = new Room(RoomId::ROOM_ID_VRU, "VRU", 2, Company::VRU, RoomType::ROOM_TYPE_NORMAL);
        m_Rooms[RoomId::ROOM_ID_CLAN_SEARCH]    = new Room(RoomId::ROOM_ID_CLAN_SEARCH, "Clan Search", 3, Company::NOMAD, RoomType::ROOM_TYPE_CLAN);
    }

    /// Get Standard Rooms Based on Company
    /// @p_Company : Company
    std::map<uint32, Room*> Base::GetStandardRoomsBasedOnCompany(const Company p_Company)
    {
        std::map<uint32, Room*> l_Rooms;

        switch (p_Company)
        {
            case Company::EARTH:
            {
                l_Rooms[RoomId::ROOM_ID_EIC] = GetRoomById(RoomId::ROOM_ID_EIC);
			}
			break;
			case Company::MMO:
            {
                l_Rooms[RoomId::ROOM_ID_MMO] = GetRoomById(RoomId::ROOM_ID_MMO);
			}
			break;
			case Company::VRU:
            {
                l_Rooms[RoomId::ROOM_ID_VRU] = GetRoomById(RoomId::ROOM_ID_VRU);
			}
			break;
            default:
                LOG_WARNING("Chat", "Company not found.");
        }

        l_Rooms[RoomId::ROOM_ID_GLOBAL] = GetRoomById(RoomId::ROOM_ID_GLOBAL);

        return l_Rooms;
    }

    /// Get Room by Id
    /// @p_Id : Room Id
    Room* Base::GetRoomById(const uint16 p_Id)
    {
        auto l_Itr = m_Rooms.find(p_Id);
        if (l_Itr != m_Rooms.end())
			return l_Itr->second;

        return nullptr;
    }

    /// Check to see if room exists by name
    /// @p_Name : Room Name
    bool Base::RoomExistsByName(const std::string p_Name)
    {
        for (auto l_Itr : m_Rooms)
        {
			if (l_Itr.second->GetName() == p_Name)
				return true;
		}

        return false;
    }

    /// Add Room
    /// @p_Room : Room to add
    void Base::AddRoom(Room* p_Room)
    {
        m_Rooms[p_Room->GetId()] = p_Room;
    }
    /// Remove Room
    /// @p_Id : Room Id
    void Base::RemoveRoom(const uint16 p_Id)
    {
        auto l_Itr = m_Rooms.find(p_Id);
        if (l_Itr != m_Rooms.end())
        {
			Room* l_Room = l_Itr->second;

            for (auto l_JItr : l_Room->GetRoomPlayers())
                l_JItr.second->LeaveRoom(l_Room->GetId());

            l_Room->SetScheduledForDeletion(true);
		}
    }
    /// Update Rooms
    /// This is to find out if the room is empty, if so, then remove the room
    /// @p_Diff : Time Diff
    void Base::UpdateRooms(uint32 const p_Diff)
    {
        for (auto l_Itr = m_Rooms.begin(); l_Itr != m_Rooms.end();)
        {
            if (l_Itr->second->IsPrivateRoom())
            {
                if (l_Itr->second->GetRoomPlayers().empty())
                {
                    delete l_Itr->second;
                    l_Itr = m_Rooms.erase(l_Itr);
                    continue;
                }
            }

            if (l_Itr->second->IsScheduledForDeletion())
            {
                for (auto l_JItr : l_Itr->second->GetRoomPlayers())
					l_JItr.second->m_Rooms.erase(l_JItr.second->m_Rooms.find(l_Itr->second->GetId()));

                delete l_Itr->second;
				l_Itr = m_Rooms.erase(l_Itr);
				continue;
			}

            l_Itr->second->Update(p_Diff);
            ++l_Itr;
		}
    }
    /// Generate Room Id
    uint16 Base::GenerateRoomId()
    {
        uint16 l_RoomId = 0;

        while (true)
        {
			l_RoomId = Core::Utils::UInt32Random(1000, 9999);

			if (m_Rooms.find(l_RoomId) == m_Rooms.end())
				break;
		}

		return l_RoomId;
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
                delete *l_Itr;
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

        UpdateRooms(p_Diff);
    }
    /// Stop World Updating
    bool Base::StopWorld() const
    {
        return s_StopChat;
    }

    /// Process Incoming Command
    ///@ p_Input : Command to process
    ///@ p_Player : Player who is sending the command
    void Base::ProcessCommand(const std::string& p_Input, Entity::Player* p_Player)
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
    /// This just uses the Admin Message Packet
    /// Probably Wrong
    /// @p_Message : System Message
    void Base::SendSystemMessage(const std::string p_Message)
    {
        Server::Packets::DeveloperMessage l_SystemMessagePacket;
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