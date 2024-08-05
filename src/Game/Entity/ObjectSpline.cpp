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

#include "Player.hpp"
#include "Diagnostic/DiaServerWatch.hpp"
#include "Utility/UtilMaths.hpp"
#include "Utility/UtilRandom.hpp"
#include "Logger/Base.hpp"
#include "Logger/LogDefines.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    /// @p_Object : Object
    Spline ::Spline(Object* p_Object)
        : m_Object(p_Object)
    {
        m_PositionX          = 0.0f;
        m_PositionY          = 0.0f;
        m_PlannedPositionX   = 0.0f;
        m_PlannedPositionY   = 0.0f;
        m_Orientation        = 0.0f;
        m_TimeForDestination = 0;
        m_LastCalled         = 0;
        m_Moving             = false;
        m_GridIndex          = std::make_tuple(0, 0);
        m_Speed              = 0;
    }
    /// Deconstructor
    Spline::~Spline()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////
    //                GENERAL
    ///////////////////////////////////////////

    /// Update Position
    void Spline::UpdatePosition()
    {
        uint32 l_TimeDistance = sServerTimeManager->GetServerTime() - m_LastCalled;

        if (l_TimeDistance > m_TimeForDestination)
        {
            m_PositionX = m_PlannedPositionX;
            m_PositionY = m_PlannedPositionY;
            return;
        }

        int32 l_DeltaX = m_PlannedPositionX - m_PositionX;
        int32 l_DeltaY = m_PlannedPositionY - m_PositionY;

        /// Get time in milliseconds
        float l_Delta = (static_cast<float>(l_TimeDistance) / 1000) * m_Speed;

        /// Get Intersect point
        double l_Degree = std::atan2(l_DeltaY, l_DeltaX);

        int32 l_PositionX = std::cos(l_Degree) * l_Delta + m_PositionX;
        int32 l_PositionY = std::sin(l_Degree) * l_Delta + m_PositionY;

        if (m_PositionX <= m_PlannedPositionX && l_PositionX > m_PlannedPositionX ||
            m_PositionX >= m_PlannedPositionX && l_PositionX < m_PlannedPositionX)
            l_PositionX = m_PlannedPositionX;

        if (m_PositionY <= m_PlannedPositionY && l_PositionY > m_PlannedPositionY ||
            m_PositionY >= m_PlannedPositionY && l_PositionY < m_PlannedPositionY)
            l_PositionY = m_PlannedPositionY;

        m_Orientation = std::round(static_cast<float>(l_Degree + 4 % 8));
        m_PositionX = l_PositionX;
        m_PositionY = l_PositionY;
    }

    /// Return Time in milliseconds to reach destination
    [[nodiscard]] int32 Spline::CalculateDestinationTime()
    {
        return static_cast<int32>((Core::Utils::DistanceSquared(m_PlannedPositionX, m_PlannedPositionY, m_PositionX, m_PositionY) / m_Speed) * 1000);
    }

    /// Move Object
    /// @p_PlannedPositionX : Planned X Axis
    /// @p_PlannedPositionY : Planned Y Axis
    void Spline::Move(float const p_PlannedPositionX, float const p_PlannedPositionY)
    {
        LOG_ASSERT(m_Object->GetMap(), "Object", "Object %0 attempted to move, but object is not in a map!", m_Object->GetGUID());

        UpdatePosition();

        if (p_PlannedPositionX || p_PlannedPositionY)
        {
            m_PlannedPositionX   = p_PlannedPositionX;
            m_PlannedPositionY   = p_PlannedPositionY;
        }

        m_TimeForDestination = CalculateDestinationTime();
        m_LastCalled         = sServerTimeManager->GetServerTime();
        m_Moving             = true;

        m_Object->GetMap()->Move(m_Object);
    }

    bool Spline::IsMoving() const
    {
        float l_Distance = Core::Utils::DistanceSquared(m_PositionX, m_PositionY, m_PlannedPositionX, m_PlannedPositionY);
        return l_Distance > 150.0f;
    }

    void Spline::SetSpeed(uint32 const p_Speed)
    {
        m_Speed = p_Speed;

        if (m_Object->IsPlayer())
            m_Object->ToPlayer()->SendUpdateSpeed();

    }

    float Spline::GetDistance(Spline* const p_Spline)
    {
        return Core::Utils::DistanceSquared(m_PositionX, m_PositionY, p_Spline->GetPositionX(), p_Spline->GetPositionY());
    }

    std::pair<float, float> Spline::PositionInCircleRadius(float const p_Radius /*=360*/)
    {
        auto l_Angle = Core::Utils::UInt32Random(0, 360);

        float l_X = m_PositionX + p_Radius * std::cos(l_Angle);
        float l_Y = m_PositionY + p_Radius * std::sin(l_Angle);

        return std::make_pair(l_X, l_Y);
	}

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone