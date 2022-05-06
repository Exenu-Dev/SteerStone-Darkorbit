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

#include "AttackPackets.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets { namespace Attack {
    
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

    /// SERVER_PACKET_TARGET_HEALTH
    PacketBuffer const* TargetHealth::Write()
    {
        m_Buffer.AppendChar("0");   ///< Unused by client
        m_Buffer.AppendChar("0");   ///< Unused by client
        m_Buffer.AppendInt32(Shield);
        m_Buffer.AppendInt32(MaxShield);
        m_Buffer.AppendInt32(HitPoints);
        m_Buffer.AppendInt32(MaxHitPoints);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }
    /// SERVER_PACKET_ROCKET_ATTACK
    PacketBuffer const* RocketShoot::Write()
    {
        m_Buffer.AppendUInt32(FromId);
        m_Buffer.AppendUInt32(ToId);
        m_Buffer.AppendBool(true);
        m_Buffer.AppendUInt32(RocketId);

        m_Buffer.AppendEndSplitter();
        m_Buffer.AppendCarriage();

        return &m_Buffer;
    }

}   ///< namespace Attack
}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone