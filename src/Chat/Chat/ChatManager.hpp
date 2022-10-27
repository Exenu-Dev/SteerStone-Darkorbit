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
#include "Singleton/Singleton.hpp"

#include "Player.hpp"

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

        /// Add Process Command
        ///@ p_Id : Player Id
        ///@ p_Command : Command to process
        void AddProcessCommand(const uint32 p_Id, const std::string p_Command);

        /// Send System Message
        /// @p_Message : System Message
        void SendSystemMessage(const std::string p_Message);

        /// Get the total players
        uint32 const TotalPlayers();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        PlayerSet m_Players;
        static volatile bool s_StopChat;                                   ///< Stop Chat Updating
    };
}   ///< namespace Channel
}   ///< namespace Chat
}   ///< namespace Steerstone

#define sChatManager SteerStone::Chat::Channel::Base::GetSingleton()