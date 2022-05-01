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

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"
#include "Common.hpp"

#include "Diagnostic/DiaIntervalTimer.hpp"
#include "Socket.hpp"

namespace SteerStone { namespace Chat { namespace Entity {

    class Server::ClientPacket;

    /// Main entry for session in world
    class Player
    {
    public:
        friend class Server::ChatSocket;

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

        /// Check if player is in room
        ///@ p_RoomId: Room Id to search for
        bool IsInRoom(uint16 const p_RoomId);

        ///////////////////////////////////////////
        //              PACKETS
        ///////////////////////////////////////////
    public:
        /// Send Packet
        /// @p_PacketBuffer : Packet Buffer
        void SendPacket(Server::PacketBuffer const* p_PacketBuffer);
        /// Send Ping
        void SendPing();

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        /// Getters Function
        uint32 GetId()             const     { return m_Id;                     }
        std::string GetUsername()  const     { return m_Username;               }
        bool IsAdmin()             const     { return m_RankId == Rank::ADMIN;  }

        std::shared_ptr<Server::ChatSocket> ToSocket() { return m_Socket; }

        /// Timers
        Core::Diagnostic::IntervalTimer IntervalPing;

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        /// Stored Player Info
        uint32 m_Id;
        std::string m_Username;
        std::vector<uint16> m_RoomIds;
        Company m_CompanyId;
        Rank m_RankId;
        std::shared_ptr<Server::ChatSocket> m_Socket;                    ///< Socket
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone