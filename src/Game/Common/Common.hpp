/*
* Liam Ashdown
* Copyright (C) 2019
*
* Thisprogram isfree software: you can redistribute it and/or modify
* it under the termsof the GNU General Public License aspublished by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Thisprogram isdistributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY, without even the implied warranty of
* MERCHANTABILITY or FITNESsFOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with thisprogram.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <PCH/Precompiled.hpp>
#include "GameFlags.hpp"
#include "Core/Core.hpp"

namespace Common
{
    /// Get Weapon Count by Ship
    /// @p_ShipId : Ship Id
    uint32 GetWeaponCountByShip(uint16 p_ShipId)
    {
        uint32 l_MaxWeaponCount = 0;

        switch (p_ShipId)
        {
        case PlayerShips::Phoenix:
            l_MaxWeaponCount = 1;
            break;
        case PlayerShips::Yamato:
            l_MaxWeaponCount = 2;
            break;
        case PlayerShips::Leonov:
            l_MaxWeaponCount = 6;
            break;
        case PlayerShips::Defcom:
            l_MaxWeaponCount = 3;
            break;
        case PlayerShips::Liberator:
            l_MaxWeaponCount = 4;
            break;
        case PlayerShips::Piranha:
            l_MaxWeaponCount = 5;
            break;
        case PlayerShips::Nostromo:
            l_MaxWeaponCount = 6;
            break;
        case PlayerShips::Vengeance:
            l_MaxWeaponCount = 10;
            break;
        case PlayerShips::Bigboy:
            l_MaxWeaponCount = 15;
            break;
        case PlayerShips::Goliath:
            l_MaxWeaponCount = 15;
            break;
        case PlayerShips::GGoliath:
            l_MaxWeaponCount = 15;
            break;
        case PlayerShips::CBSGoliath:
            l_MaxWeaponCount = 15;
            break;
        }

        return l_MaxWeaponCount;
    }
}   ///< namespace Common