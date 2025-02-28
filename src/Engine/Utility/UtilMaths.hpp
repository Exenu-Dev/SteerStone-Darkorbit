#pragma once
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

#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"

#include <cmath>

#define M_PI 3.14159265358979323846

namespace SteerStone { namespace Core { namespace Utils {

    /// Check if position is in radius
    /// @p_PositionX        : X Axis
    /// @p_PositionY        : Y Axis
    /// @p_OtherPositionX   : x Axis
    /// @p_OtherPositiony   : Y Axis
    inline bool IsInCircleRadius(float const& p_PositionX, float const& p_PositionY, float const& p_OtherPositionX, float const& p_OtherPositionY, float const p_Radius)
    {
        float l_DistX = (p_PositionX - p_OtherPositionX);
        l_DistX = l_DistX * l_DistX;

        float l_DistY = (p_PositionY - p_OtherPositionY);
        l_DistY = l_DistY * l_DistY;

        int32 l_Total = l_DistX + l_DistY;

        return l_Total < p_Radius * p_Radius;
    }
    /// Check if position is in radius
    /// @p_PositionX        : X Axis
    /// @p_PositionY        : Y Axis
    /// @p_OtherPositionX   : x Axis
    /// @p_OtherPositiony   : Y Axis
    inline bool IsInSquareRadius(float const& p_PositionX, float const& p_PositionY, float const& p_OtherPositionX, float const& p_OtherPositionY, float const p_Radius)
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
    /// Get Distance Squared
    /// @p_PositionX        : X Axis
    /// @p_PositionY        : Y Axis
    /// @p_OtherPositionX   : x Axis
    /// @p_OtherPositiony   : Y Axis
    inline float DistanceSquared(float const& p_PositionX, float const& p_PositionY, float const& p_OtherPositionX, float const& p_OtherPositionY)
    {
        return sqrt(pow(p_OtherPositionX - p_PositionX, 2) + pow(p_OtherPositionY - p_PositionY, 2));
    }

    inline float CalculateAngle(float const& p_PositionX, float const& p_PositionY, float const& p_OtherPositionX, float const& p_OtherPositionY)
	{
        return atan2(p_OtherPositionY - p_PositionY, p_OtherPositionX - p_PositionX);
	}

    /// Percentage calculation
    /// @p_Base : Value
    /// @p_Percentage : Percentage
    template <class T, class U>
    inline T CalculatePercentage(T p_Base, U p_Percentage)
    {
        return T(p_Base * static_cast<float>(p_Percentage) / 100.0f);
    }
    /// Percentage calculation
    /// @p_Base : Value
    /// @p_Percentage : Percentage
    template <class T, class U>
    inline T AddPercentage(T& p_Base, U p_Percentage)
    {
        return p_Base += CalculatePercentage(p_Base, p_Percentage);
    }
    /// Percentage calculation
    /// @p_Base : Value
    /// @p_Percentage : Percentage
    template <class T, class U>
    inline T ApplyPercentage(T& p_Base, U p_Percentage)
    {
        return p_Base = CalculatePercentage(p_Base, p_Percentage);
    }
    /// Check to see if string is a number
    /// @p_String : String to check against
    inline bool IsNumber(const std::string& p_String)
    {
        std::string::const_iterator l_Itr = p_String.begin();
        while (l_Itr != p_String.end() && std::isdigit(*l_Itr)) ++l_Itr;
        return !p_String.empty() && l_Itr == p_String.end();
    }

}   ///< namespace Utils
}   ///< namespace Core
}   ///< namespace SteerStone