#pragma once
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

#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"

namespace SteerStone { namespace Core { namespace Utils {

    /// Check if position is in radius
    /// @p_PositionX        : X Axis
    /// @p_PositionY        : Y Axis
    /// @p_OtherPositionX   : x Axis
    /// @p_OtherPositiony   : Y Axis
    inline bool IsInCircleRadius(float const p_PositionX, float const p_PositionY, float const p_OtherPositionX, float const p_OtherPositionY, float const p_Radius)
    {
        float l_DistanceSquared = std::sqrt((p_PositionX - p_OtherPositionX)
            * (p_OtherPositionX - p_PositionX)
            + (p_PositionY - p_OtherPositionY)
            * (p_PositionY - p_OtherPositionY));

        if (l_DistanceSquared <= p_Radius || std::isnan(l_DistanceSquared))
            return true;

        return false;
    }
    /// Check if position is in radius
    /// @p_PositionX        : X Axis
    /// @p_PositionY        : Y Axis
    /// @p_OtherPositionX   : x Axis
    /// @p_OtherPositiony   : Y Axis
    inline bool IsInSquareRadius(float const p_PositionX, float const p_PositionY, float const p_OtherPositionX, float const p_OtherPositionY, float const p_Radius)
    {
        if (p_PositionX < p_OtherPositionX - p_Radius) 
            return false;
        if (p_PositionX > p_OtherPositionX + p_Radius) 
            return false;
        if (p_PositionY < p_OtherPositionY - p_Radius)
            return false;
        if (p_PositionY > p_OtherPositionY + p_Radius) 
            return false;
        
        return true;
    }

}   ///< namespace Utils
}   ///< namespace Core
}   ///< namespace SteerStone