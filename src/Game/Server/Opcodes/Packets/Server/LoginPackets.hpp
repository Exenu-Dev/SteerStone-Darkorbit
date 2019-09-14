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
#include "Socket.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets {

    /// SERVER_PACKET_LOGIN packet builder
    class Login final : public ServerPacket
    {
        public:
            /// Constructor 
            Login() : ServerPacket(ServerOpCodes::SERVER_PACKET_LOGIN)
            {
            }

            //////////////////////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////////////////////

            /// Write the packet
            PacketBuffer const* Write();

            bool DisplayBoost;
            bool DisplayDamage;
            bool DisplayAllLas;
            bool DisplayExploration;
            bool DisplayName;
            bool DisplayFirmIcon;
            bool DisplayAlphaBG;
            bool IgnoreRes;
            bool IgnoreBox;
            bool ConvertGates;
            bool ConvertOppo;
            bool SoundOff;
            bool BackgroundMusicOff;
            bool DisplayStatus;
            bool DisplayBubble;
            uint32 SelectedLaser;
            uint32 SelectedRocket;
            bool DisplayDigits;
            bool DisplayChat;
            bool DisplayDrones;
            bool ShowStarSystem;
            bool IgnoreCargo;
            bool IgnoreHostileCargo;
            bool AutoChangeAmmo;
            bool EnableBuyFast;
    };

    /// SERVER_PACKET_LOGIN packet builder
    class InitializeShip final : public ServerPacket
    {
    public:
        /// Constructor 
        InitializeShip() : ServerPacket(ServerOpCodes::SERVER_PACKET_INITIALIZE_SHIP)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 Id;
        std::string Username;
        uint16 ShipId;
        uint32 Speed;
        int32 Shield;
        uint32 MaxShield;
        int32 HitPoints;
        uint32 m_MaxHitPoints;
        uint32 CargoSpace;
        uint32 MaxCargoSpace;
        float PositionX;
        float PositionY;
        uint32 MapId;
        uint16 CompanyId;
        uint32 ClanId;
        uint32 MaxBattery;
        uint32 MaxRockets;
        uint16 WeaponState;
        bool IsPremium;
        uint32 Experience;
        uint32 Honour;
        uint16 Level;
        uint32 Credits;
        uint32 Uridium;
        uint32 JackPot;
        uint16 Rank;
        std::string ClanTag;
        uint16 GatesAchieved;
        bool UseSystemFont;
    };

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone