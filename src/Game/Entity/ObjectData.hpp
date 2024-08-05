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
#include "Core/Core.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    enum class PriceType : uint8
    {
        PRICE_TYPE_CREDITS = 0,
        PRICE_TYPE_URIDIUM = 1
    };

    struct MobTemplate
    {
    public:
        MobTemplate()
        {
            Entry               = 0;
            Name.clear();
            Type                = 0;
            WeaponState         = 0;
            HitPoints           = 0;
            Shield              = 0;
            ShieldResistance    = 0;
            MinDamage           = 0;
            MaxDamage           = 0;
            Behaviour           = 0;
            RespawnTimer        = 0;
            MinMovementTime     = 0;
            MaxMovementTime     = 0;
            Speed               = 0;
            Experience          = 0;
            Honor               = 0;
            Credits             = 0;
            Uridium             = 0;
            Prometium           = 0;
            Endurium            = 0;
            Terbium             = 0;
            Prometid            = 0;
            Duranium            = 0;
            Promerium           = 0;
            Xenomit             = 0;
            Seprom              = 0;
            Palladium           = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint32 Entry;
        std::string Name;
        uint16 Type;
        uint16 ShipId;
        uint16 WeaponState;
        int32 HitPoints;
        int32 Shield;
        uint32 ShieldResistance;
        uint32 MinDamage;
        uint32 MaxDamage;
        uint16 Behaviour;
        uint32 RespawnTimer;
        uint32 MinMovementTime;
        uint32 MaxMovementTime;
        uint32 Speed;
        uint32 Experience;
        uint32 Honor;
        uint32 Credits;
        uint32 Uridium;
        uint32 Prometium;
        uint32 Endurium;
        uint32 Terbium;
        uint32 Prometid;
        uint32 Duranium;
        uint32 Promerium;
        uint32 Xenomit;
        uint32 Seprom;
        uint32 Palladium;
    };

    struct PortalTemplate
    {
    public:
        PortalTemplate()
        {
            PortalId        = 0;
            Name.clear();
            Company         = 0;
            Type            = 0;
            MapId           = 0;
            PositionX       = 0;
            PositionY       = 0;
            ToMapId         = 0;
            ToPositionX     = 0;
            ToPositionY     = 0;
            Level           = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint32 PortalId;
        std::string Name;
        uint16 Company;
        uint16 Type;
        uint32 MapId;
        float PositionX;
        float PositionY;
        uint32 ToMapId;
        float ToPositionX;
        float ToPositionY;
        uint16 Level;
    };
    
    struct StationTemplate
    {
    public:
        StationTemplate()
        {
            Id          = 0;
            Name.clear();
            Company     = 0;
            Type        = 0;
            MapId       = 0;
            Peace       = 0;
            PositionX   = 0;
            PositionY   = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint32 Id;
        std::string Name;
        uint16 Company;
        uint16 Type;
        uint32 MapId;
        bool Peace;
        float PositionX;
        float PositionY;
    };

    struct ItemTemplate
    {
        ItemTemplate()
        {
            Id                  = 0;
            Name.clear();
            Type                = 0;
            GFX                 = 0;
            Value               = 0;
            ValuePercentage     = 0;
            Credits             = 0;
            Uridium             = 0;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        uint32 GetPrice() const
        {
			if (Uridium > 0)
				return Uridium;

            return Credits;
		}

        uint32 GetValue() const
        {
			return Value;
		}

        PriceType GetPriceType() const
        {
            if (Uridium > 0)
                return PriceType::PRICE_TYPE_URIDIUM;

            return PriceType::PRICE_TYPE_CREDITS;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint32 Id;
        std::string Name;
        uint16 Type;
        uint16 GFX;
        uint32 Value;
        uint32 ValuePercentage;
        uint32 Credits;
        uint32 Uridium;
    };

    struct ShipTemplate
    {
        ShipTemplate()
        {
            Entry               = 0;
            Name                = "";
            Description         = "";
            HitPoints           = 0;
            NanoHull            = 0;
            Speed               = 0;
            Cargo               = 0;
            Lasers              = 0;
            Generators          = 0;
            Rockets             = 0;
            Ammo                = 0;
            Extras              = 0;
            Credits             = 0;
            Uridium             = 0;
            Disabled            = false;
        }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        uint32 Entry;
        std::string Name;
        std::string Description;
        uint32 HitPoints;
        uint32 NanoHull;
        uint32 Speed;
        uint32 Cargo;
        uint32 Lasers;
        uint32 Generators;
        uint32 Rockets;
        uint32 Ammo;
        uint32 Extras;
        uint32 Credits;
        uint32 Uridium;
        bool Disabled;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone