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

#pragma once
#include <PCH/Precompiled.hpp>

#include "Core/Core.hpp"
#include "Socket.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Main entry for session in world
    class Player
    {
        DISALLOW_COPY_AND_ASSIGN(Player);

        friend class Server::GameSocket;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        public:
            /// Constructor
            /// @p_GameSocket : Socket
            Player(Server::GameSocket* p_GameSocket);
            /// Deconstructor
            ~Player();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

            /// Load player details from database
            bool LoadFromDB();

            /// Send Client In-game settings
            void SendClientSettings();

            /// Send Packet
            /// @p_PacketBuffer : Packet Buffer
            void SendPacket(Server::PacketBuffer const* p_PacketBuffer);

            /// Stored Player Info Getters
            uint32 GetId()             const     { return m_Id; }
            std::string GetSessionId() const     { return m_SessionId; }
            std::string GetUsername()  const     { return m_Username; }
            uint32 GetUridium()        const     { return m_Uridium; }
            uint32 GetCredits()        const     { return m_Credits; }
            uint32 GetJackPot()        const     { return m_Jackpot; }
            uint32 GetLevel()          const     { return m_Level; }
            uint32 GetExperience()     const     { return m_Experience; }
            uint32 GetHonor()          const     { return m_Honor; }
            uint32 GetClanId()         const     { return m_ClanId; }
            bool IsPremium()           const     { return m_Premium; }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        private:
            /// Stored Player Info
            uint32 m_Id;
            std::string m_SessionId;
            std::string m_Username;
            uint32 m_Uridium;
            uint32 m_Credits;
            uint32 m_Jackpot;
            uint32 m_Level;
            uint32 m_Experience;
            uint32 m_Honor;
            uint32 m_ClanId;
            bool m_Premium;

            /// Player Settings
            bool m_DisplayBoost;
            bool m_DisplayDamage;
            bool m_DisplayAllLas;
            bool m_DisplayExploration;
            bool m_DisplayName;
            bool m_DisplayFirmIcon;
            bool m_DisplayAlphaBG;
            bool m_IgnoreRes;
            bool m_IgnoreBox;
            bool m_ConvertGates;
            bool m_ConvertOppo;
            bool m_SoundOff;
            bool m_BackgroundMusicOff;
            bool m_DisplayStatus;
            bool m_DisplayBubble;
            uint32 m_SelectedLaser;
            uint32 m_SelectedRocket;
            bool m_DisplayDigits;
            bool m_DisplayChat;
            bool m_DisplayDrones;
            bool m_ShowStarSystem;
            bool m_IgnoreCargo;
            bool m_IgnoreHostileCargo;
            bool m_AutoChangeAmmo;
            bool m_EnableBuyFast;


            std::shared_ptr<Server::GameSocket> m_Socket;   ///< Socket
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone