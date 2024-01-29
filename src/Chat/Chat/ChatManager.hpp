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
#include "json.hpp"
#include "Singleton/Singleton.hpp"

#include "Commands/Handler.hpp"

#define CHAT_SLEEP_TIMER 60

namespace SteerStone { namespace Chat { namespace Channel {
    typedef std::set<Entity::Player*> PlayerSet;

    /// Chat
    class Base
    {
        SINGLETON_P_D(Base);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Add Player
        ///@ p_Player : Player
        void AddPlayer(Entity::Player* p_Player);
        /// Return Player in chat
        /// @p_Id : Id of player
        Entity::Player* FindPlayer(uint64 const p_Id);

        /// Update the Chat
        ///@ p_Diff : Time Diff
        void Update(uint32 const p_Diff);
        /// Stop World Updating
        bool StopWorld() const;

        /// Send Message
        ///@ p_Player : Player who is sending the message
        ///@ p_Message: Message to send
        ///@ p_RoomId: Room Id to send to
        void SendMessageToRoom(Entity::Player const* p_Player, std::string const p_Message, uint16 const p_RoomId);

        /// Process Incoming Command
        ///@ p_Input : Command to process
        ///@ p_Player : Player who is sending the command
        void ProcessCommand(const std::string& p_Input, Entity::Player const* p_Player);

        /// Add Process Command
        ///@ p_Id : Player Id
        ///@ p_Command : Command to process
        void AddProcessCommand(const uint32 p_Id, const std::string p_Command, const nlohmann::json p_Json = nlohmann::json());

        /// Send System Message
        /// @p_Message : System Message
        void SendSystemMessage(const std::string p_Message);

        /// Ban Player
        /// @p_BannedUsername : Player who is banned
        /// @p_Player         : Player who banned the player
        /// @p_Reason         : Reason why player is banned
        /// @p_DaysHours      : How many days or hours the player is banned for
        void BanPlayer(const std::string p_BannedUsername, Entity::Player const* p_Player, const std::string p_Reason, std::string p_DaysHours);

        /// Check if player is banned
        /// If the player is banned, then send packet to let know user is banned (if true)
        /// @p_Player : Player who may be banned
        /// @p_SendPacket : Send Packet to banned player to inform the they are banned.
        const bool PlayerIsBanned(Entity::Player* p_Player, const bool p_SendPacket = true);

        /// Get the total players
        uint32 const TotalPlayers();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        PlayerSet m_Players;
        static volatile bool s_StopChat;                                   ///< Stop Chat Updating
        Commands::Handler* m_CommandsHandler;                              ///< Commands Handler
    };
}   ///< namespace Channel
}   ///< namespace Chat
}   ///< namespace Steerstone

#define sChatManager SteerStone::Chat::Channel::Base::GetSingleton()