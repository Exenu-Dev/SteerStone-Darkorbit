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

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone