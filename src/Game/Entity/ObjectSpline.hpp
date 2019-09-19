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
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    class Object;

    class Spline
    {
        DISALLOW_COPY_AND_ASSIGN(Spline);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        /// @p_Object : Object
        Spline(Object* p_Object);
        /// Deconstructor
        ~Spline();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Set Speed
        /// @p_Speed : Speed
        void SetSpeed(uint32 const p_Speed);
        /// Return Speed
        uint32 GetSpeed() const;

        /// Set Object Position
        /// @p_PositionX : X Axis
        /// @p_PositionY : Y Axis
        void SetPosition(float const p_PositionX, float const p_PositionY, float const p_PlannedPositionX = 0.0f, float const p_PlannedPositionY = 0.0f);
        /// Return Position X
        float GetPositionX() const;
        /// Return Position Y
        float GetPositionY() const;
        /// Return Planned Position X
        float GetPlannedPositionX() const;
        /// Return Planned Position Y
        float GetPlannedPositionY() const;

        /// Update Position
        void UpdatePosition();

        /// Return Time in milliseconds to reach destination
        int32 CalculateDestinationTime();

        /// Get Destination time in milliseconds
        uint32 GetDestinationTime() const;
     
        /// Move Object
        /// @p_PlannedPositionX : Planned X Axis
        /// @p_PlannedPositionY : Planned Y Axis
        /// @p_CurrentPositionX : Current X Axis
        /// @p_CurrentPositionY : Current Y Axis
        void Move(float const p_PlannedPositionX, float const p_PlannedPositionY, float const p_CurrentPositionX, float const p_CurrentPositionY);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        float m_PositionX;
        float m_PositionY;
        float m_PlannedPositionX;
        float m_PlannedPositionY;
        uint32 m_Speed;
        uint32 m_TimeForDestination;
        uint32 m_LastCalled;
        std::tuple<uint32, uint32> m_GridIndex;

        Object* m_Object;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone