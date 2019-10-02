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

#include "MapPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets {

    /// SERVER_PACKET_SPAWN_SHIP
    PacketBuffer const* SpawnShip::Write()
    {
        m_Buffer.AppendUInt32(UserId);
        m_Buffer.AppendUInt16(ShipId);
        m_Buffer.AppendUInt16(WeaponState);
        m_Buffer.AppendChar(Clan.c_str());
        m_Buffer.AppendChar(Name.c_str());
        m_Buffer.AppendFloat(PositionX);
        m_Buffer.AppendFloat(PositionY);
        m_Buffer.AppendUInt16(CompanyId);
        m_Buffer.AppendUInt32(ClanId);
        m_Buffer.AppendUInt16(Rank);
        m_Buffer.AppendBool(ShowRedSquareOnMiniMap);
        m_Buffer.AppendUInt16(ClanDiplomat);
        m_Buffer.AppendUInt16(GalaxyGatesAchieved);
        m_Buffer.AppendBool(UseBigFont);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_OBJECT_MOVE
    PacketBuffer const* ObjectMove::Write()
    {
        m_Buffer.AppendUInt32(Id);
        m_Buffer.AppendUInt32(PositionX);
        m_Buffer.AppendUInt32(PositionY);
        m_Buffer.AppendUInt32(Time);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_DESPAWN_SHIP
    PacketBuffer const* DespawnShip::Write()
    {
        m_Buffer.AppendUInt32(Id);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_CREATE_PORTAL
    PacketBuffer const* CreatePortal::Write()
    {
        m_Buffer.AppendUInt32(Id);
        m_Buffer.AppendUInt16(Type);
        m_Buffer.AppendChar("1"); ///< Unknown, not used by client
        m_Buffer.AppendUInt32(PositionX);
        m_Buffer.AppendUInt32(PositionY);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_CREATE_STATION
    PacketBuffer const* CreateStation::Write()
    {
        m_Buffer.AppendUInt32(Id);
        m_Buffer.AppendUInt32(Type);
        m_Buffer.AppendChar(Name.c_str());
        m_Buffer.AppendUInt16(CompanyId);
        m_Buffer.AppendBool(Peace);
        m_Buffer.AppendUInt32(PositionX);
        m_Buffer.AppendUInt32(PositionY);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_EVENT
    PacketBuffer const* Event::Write()
    {
        m_Buffer.AppendUInt32(PositionX);
        m_Buffer.AppendUInt32(PositionY);
        m_Buffer.AppendBool(InDemolitionZone);
        m_Buffer.AppendBool(PlayRepairAnimation);
        m_Buffer.AppendBool(InTradeZone);
        m_Buffer.AppendBool(InRadiationZone);
        m_Buffer.AppendBool(InJumpZone);
        m_Buffer.AppendBool(Repair);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_JUMP_PORTAL
    PacketBuffer const* JumpPortal::Write()
    {
        m_Buffer.AppendUInt32(MapId);
        m_Buffer.AppendUInt32(PortalId);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_JUMP_PORTAL
    PacketBuffer const* DisplayStarSystem::Write()
    {
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_LASER_SHOOT
    PacketBuffer const* LaserShoot::Write()
    {
        m_Buffer.AppendUInt32(FromId);
        m_Buffer.AppendUInt32(ToId);
        m_Buffer.AppendUInt32(LaserId);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_ATTACK_OUT_OF_RANGE
    PacketBuffer const* AttackOutOfRange::Write()
    {
        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_ATTACK_IN_RANGE
    PacketBuffer const* AttackInRange::Write()
    {
        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_CANCEL_LASTER_SHOOT
    PacketBuffer const* CancelLaserShoot::Write()
    {
        m_Buffer.AppendUInt32(FromId);
        m_Buffer.AppendUInt32(ToId);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_RECIEVED_DAMAGE
    PacketBuffer const* RecievedDamage::Write()
    {
        m_Buffer.AppendUInt32(HitPoints);
        m_Buffer.AppendUInt32(Shield);
        m_Buffer.AppendUInt32(0); ///< Not Used
        m_Buffer.AppendUInt32(Damage);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_KILL
    PacketBuffer const* Kill::Write()
    {
        m_Buffer.AppendUInt32(Id);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_MISS_SELF
    PacketBuffer const* MissSelf::Write()
    {
        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_MISS_TARGET
    PacketBuffer const* MissTarget::Write()
    {
        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_MAKE_DAMAGE
    PacketBuffer const* MakeDamage::Write()
    {
        if (UpdateAmmo)
            m_Buffer.AppendChar("L");
        else
            m_Buffer.AppendChar("NaN");

        m_Buffer.AppendUInt32(HitPoints);
        m_Buffer.AppendUInt32(Shield);
        m_Buffer.AppendUInt32(Damage);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_ESCAPED_THE_ATTACK
    PacketBuffer const* EscapedTheAttack::Write()
    {
        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

    /// SERVER_PACKET_MISC_INFO
    PacketBuffer const* MiscInfo::Write()
    {
        if (Type == "d") ///< Drones
        {
            m_Buffer.AppendChar(Type.c_str());
            m_Buffer.AppendUInt32(Id);
            m_Buffer.AppendChar(Misc.c_str());
        }

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< Packets
}   ///< Server
}   ///< Game
}   ///< SteerStone