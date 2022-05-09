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
#include "Socket.hpp"

namespace SteerStone { namespace Game { namespace Server { namespace Packets { namespace Attack {

    /// SERVER_PACKET_LASER_SHOOT packet builder
    class LaserShoot final : public ServerPacket
    {
    public:
        /// Constructor 
        LaserShoot() : ServerPacket(ServerOpCodes::SERVER_PACKET_LASER_SHOOT)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 FromId;
        uint32 ToId;
        uint32 LaserId;
    };

    /// SERVER_PACKET_ATTACK_OUT_OF_RANGE packet builder
    class AttackOutOfRange final : public ServerPacket
    {
    public:
        /// Constructor 
        AttackOutOfRange() : ServerPacket(ServerOpCodes::SERVER_PACKET_ATTACK_OUT_OF_RANGE)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    /// SERVER_PACKET_ATTACK_IN_RANGE packet builder
    class AttackInRange final : public ServerPacket
    {
    public:
        /// Constructor 
        AttackInRange() : ServerPacket(ServerOpCodes::SERVER_PACKET_ATTACK_IN_RANGE)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    /// SERVER_PACKET_CANCEL_LASTER_SHOOT packet builder
    class CancelLaserShoot final : public ServerPacket
    {
    public:
        /// Constructor 
        CancelLaserShoot() : ServerPacket(ServerOpCodes::SERVER_PACKET_CANCEL_LASTER_SHOOT)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 FromId;
        uint32 ToId;
    };

    /// SERVER_PACKET_RECIEVED_DAMAGE packet builder
    class RecievedDamage final : public ServerPacket
    {
    public:
        /// Constructor 
        RecievedDamage() : ServerPacket(ServerOpCodes::SERVER_PACKET_RECIEVED_DAMAGE)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 HitPoints;
        uint32 Shield;
        uint32 Damage;
    };


    /// SERVER_PACKET_MAKE_DAMAGE packet builder
    class MakeDamage final : public ServerPacket
    {
    public:
        /// Constructor 
        MakeDamage() : ServerPacket(ServerOpCodes::SERVER_PACKET_MAKE_DAMAGE)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        bool UpdateAmmo;
        uint32 HitPoints;
        uint32 Shield;
        uint32 Damage;
    };

    /// SERVER_PACKET_KILL packet builder
    class Kill final : public ServerPacket
    {
    public:
        /// Constructor 
        Kill() : ServerPacket(ServerOpCodes::SERVER_PACKET_KILL)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 Id;
    };

    /// SERVER_PACKET_MISS_SELF packet builder
    class MissSelf final : public ServerPacket
    {
    public:
        /// Constructor 
        MissSelf() : ServerPacket(ServerOpCodes::SERVER_PACKET_MISS_SELF)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    /// SERVER_PACKET_MISS_TARGET packet builder
    class MissTarget final : public ServerPacket
    {
    public:
        /// Constructor 
        MissTarget() : ServerPacket(ServerOpCodes::SERVER_PACKET_MISS_TARGET)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();
    };

    /// SERVER_PACKET_ESCAPED_THE_ATTACK packet builder
    class EscapedTheAttack final : public ServerPacket
    {
    public:
        /// Constructor 
        EscapedTheAttack() : ServerPacket(ServerOpCodes::SERVER_PACKET_ESCAPED_THE_ATTACK)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 Id;
    };

    /// SERVER_PACKET_TARGET_HEALTH packet builder
    class TargetHealth final : public ServerPacket
    {
    public:
        /// Constructor 
        TargetHealth() : ServerPacket(ServerOpCodes::SERVER_PACKET_TARGET_HEALTH)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        int32 Shield;
        uint32 MaxShield;
        int32 HitPoints;
        uint32 MaxHitPoints;
    };

    /// SERVER_PACKET_ROCKET_ATTACK packet builder
    class RocketShoot final : public ServerPacket
    {
    public:
        /// Constructor 
        RocketShoot() : ServerPacket(ServerOpCodes::SERVER_PACKET_ROCKET_ATTACK)
        {
            FromId      = 0;
            ToId        = 0;
            Hit         = false;
            RocketId    = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint32 FromId;
        uint32 ToId;
        bool Hit;
        uint32 RocketId;
    };

    /// SERVER_PACKET_NO_AMMUNITION packet builder
    class NoAmmunition final : public ServerPacket
    {
    public:
        /// Constructor 
        NoAmmunition() : ServerPacket(ServerOpCodes::SERVER_PACKET_NO_AMMUNITION)
        {
            TypeId = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint16 AttackType;
        uint16 TypeId;
    };

    /// SERVER_PACKET_CHANGE_ROCK_AMMO packet builder
    class ChangeRocketAmmo final : public ServerPacket
    {
    public:
        /// Constructor 
        ChangeRocketAmmo() : ServerPacket(ServerOpCodes::SERVER_PACKET_CHANGE_ROCK_AMMO)
        {
            TypeId = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Write the packet
        PacketBuffer const* Write();

        uint16 AttackType;
        uint16 TypeId;
    };

}   ///< namespace Attack
}   ///< namespace Packets
}   ///< namespace Server
}   ///< namespace Game
}   ///< namespace SteerStone