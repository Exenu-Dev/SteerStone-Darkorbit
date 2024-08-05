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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "Common.hpp"

#include "Diagnostic/DiaIntervalTimer.hpp"
#include "Socket.hpp"

namespace SteerStone { namespace Chat { namespace Channel {
    class Room;
    class Base;
}   ///< namespace Channel
}   ///< namespace Game
}   ///< namespace Steerstone

namespace SteerStone { namespace Chat { namespace Entity {

    class Server::ClientPacket;

    struct Clan
    {
        Clan()
            : Id(0), Name(""), Tag("") {}

        uint32 RoomId()
        {
            // Prevent clashing
            return Id + 9000;
        }

        uint32 Id;
        std::string Name;
        std::string Tag;
    };

    /// Main entry for session in world
    class Player
    {
    public:
        friend class Server::ChatSocket;
        friend class Chat::Channel::Base;

    public:
        /// Constructor
        /// @p_ChatSocket : Socket
        Player(Server::ChatSocket* p_ChatSocket);
        /// Deconstructor
        ~Player();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        //              GENERAL
        ///////////////////////////////////////////
    public:
        /// Load Player Info from database
        bool LoadFromDB();
        /// Load Clan Info from database
        void LoadClan();
        /// Load Rooms
        void LoadRooms();

        /// Check if player is in room
        ///@ p_RoomId: Room Id to search for
        bool IsInRoom(uint16 const p_RoomId);

        /// Add Room
        /// @p_Room : Room to add
        void AddRoom(Chat::Channel::Room* p_Room);

        /// Leave Room
        /// @p_RoomId : Room Id to leave
        void LeaveRoom(uint16 p_RoomId);

        /// Close Room
        /// @p_RoomId : Room Id to close
        void CloseRoom(uint16 p_RoomId);

        /// Ignore Player
        /// @p_PlayerId : Player to ignore
        void IgnorePlayer(Entity::Player const* p_PlayerId);

        /// Unignore Player
        /// @p_PlayerId : Player to unignore
        void UnIgnorePlayer(Entity::Player const* p_PlayerId);

        ///////////////////////////////////////////
        //              PACKETS
        ///////////////////////////////////////////
    public:
        /// Send Packet
        /// @p_PacketBuffer : Packet Buffer
        void SendPacket(Server::PacketBuffer const* p_PacketBuffer) const;
        /// Send Login
        void SendLogin();
        /// Send Ping
        void SendPing();
        /// Whisper the player
        /// @p_Player : Player to whisper
        /// @p_Message : Message to send
        void Whisper(Entity::Player const* p_Player, std::string const p_Message) const;
        /// Send Rooms
        void SendRooms() const;
        /// Send Message to self
        /// This uses the system message packet
        /// @p_Message : Message to be sent to the player
        void SendMessageToSelf(std::string const p_Message) const;

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        /// Getters Function
        uint64 GetId()             const     { return m_Id;                                              }
        std::string GetUsername()  const     { return m_Username;                                        }
        bool IsAdmin()             const     { return m_AccessLevel == AccessLevel::ACCESS_LEVEL_ADMIN;  }
        bool IsModeratorAbove()    const     { return m_AccessLevel >= AccessLevel::ACCESS_LEVEL_MODERATOR; }
        AccessLevel GetAccessLevel() const   { return m_AccessLevel;                                     }
        bool InClan()              const     { return m_Clan.Id != 0;                                    }
        std::string GetClanName()  const     { return m_Clan.Name;                                       }
        std::string ClanTag()      const     { return m_Clan.Tag;                                        }
        Company GetCompany()       const     { return m_CompanyId;                                       }
        std::string GetUsernameForDisplay() const;
        bool IsIgnored(uint32 p_Id) const     { return std::find(m_IgnoredPlayers.begin(), m_IgnoredPlayers.end(), p_Id) != m_IgnoredPlayers.end(); }
        Chat::Channel::Room* GetRoom(uint16 p_RoomId) const;
        Chat::Channel::Room* CurrentRoom() const { return m_CurrentRoom; }
        std::map<uint32, Chat::Channel::Room*> GetRooms() const { return m_Rooms; }

        std::shared_ptr<Server::ChatSocket> ToSocket() { return m_Socket; }

        /// Timers
        Core::Diagnostic::IntervalTimer IntervalPing;

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        /// Stored Player Info
        uint32 m_Id;
        std::string m_Username;
        Company m_CompanyId;
        Rank m_RankId;
        Clan m_Clan;
        Chat::Channel::Room* m_CurrentRoom;
        std::vector<uint32> m_IgnoredPlayers;
        std::map<uint32, Chat::Channel::Room*> m_Rooms;
        AccessLevel m_AccessLevel;
        std::shared_ptr<Server::ChatSocket> m_Socket;                    ///< Socket
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone