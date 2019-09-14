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

#include "LoginPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets {

    /// SERVER_PACKET_LOGIN
    PacketBuffer const* Login::Write()
    {
        m_Buffer.AppendChar("SET");
        m_Buffer.AppendBool(DisplayBoost);
        m_Buffer.AppendBool(DisplayDamage);
        m_Buffer.AppendBool(DisplayAllLas);
        m_Buffer.AppendBool(DisplayExploration);
        m_Buffer.AppendBool(DisplayName);
        m_Buffer.AppendBool(DisplayFirmIcon);
        m_Buffer.AppendBool(DisplayAlphaBG);
        m_Buffer.AppendBool(IgnoreRes);
        m_Buffer.AppendBool(IgnoreBox);
        m_Buffer.AppendBool(ConvertGates);
        m_Buffer.AppendBool(ConvertOppo);
        m_Buffer.AppendBool(SoundOff);
        m_Buffer.AppendBool(BackgroundMusicOff);
        m_Buffer.AppendBool(DisplayStatus);
        m_Buffer.AppendBool(DisplayBubble);
        m_Buffer.AppendUInt32(SelectedLaser);
        m_Buffer.AppendUInt32(SelectedRocket);
        m_Buffer.AppendBool(DisplayDigits);
        m_Buffer.AppendBool(DisplayChat);
        m_Buffer.AppendBool(DisplayDrones);
        m_Buffer.AppendBool(ShowStarSystem);
        m_Buffer.AppendBool(IgnoreCargo);
        m_Buffer.AppendBool(IgnoreHostileCargo);
        m_Buffer.AppendBool(AutoChangeAmmo);
        m_Buffer.AppendBool(EnableBuyFast);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    PacketBuffer const* InitializeShip::Write()
    {
        m_Buffer.AppendUInt32(Id);
        m_Buffer.AppendChar(Username.c_str());
        m_Buffer.AppendUInt16(ShipId);
        m_Buffer.AppendUInt32(Speed);
        m_Buffer.AppendInt32(Shield);
        m_Buffer.AppendUInt32(MaxShield);
        m_Buffer.AppendInt32(HitPoints);
        m_Buffer.AppendUInt32(m_MaxHitPoints);
        m_Buffer.AppendUInt32(CargoSpace);
        m_Buffer.AppendUInt32(MaxCargoSpace);
        m_Buffer.AppendFloat(PositionX);
        m_Buffer.AppendFloat(PositionY);
        m_Buffer.AppendUInt32(MapId);
        m_Buffer.AppendUInt16(CompanyId);
        m_Buffer.AppendUInt32(ClanId);
        m_Buffer.AppendUInt32(MaxBattery);
        m_Buffer.AppendUInt32(MaxRockets);
        m_Buffer.AppendUInt16(WeaponState);
        m_Buffer.AppendBool(IsPremium);
        m_Buffer.AppendUInt32(Experience);
        m_Buffer.AppendUInt32(Honour);
        m_Buffer.AppendUInt32(Level);
        m_Buffer.AppendUInt32(Credits);
        m_Buffer.AppendUInt32(Uridium);
        m_Buffer.AppendUInt32(JackPot);
        m_Buffer.AppendUInt16(Rank);
        m_Buffer.AppendChar(ClanTag.c_str());
        m_Buffer.AppendUInt16(GatesAchieved);
        m_Buffer.AppendBool(UseSystemFont);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone