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

        ///////////////////////////////////////////
        //                GENERAL
        ///////////////////////////////////////////
        /// Update Position
        /// @p_IgnoreTimeCheck : Ignore Time Check
        void UpdatePosition();

        /// Return Time in milliseconds to reach destination
        [[nodiscard]] int32 CalculateDestinationTime();

        /// Move Object
        /// @p_PlannedPositionX : Planned X Axis
        /// @p_PlannedPositionY : Planned Y Axis
        /// @p_IgnoreTimeCheck  : Ignore Time Check
        void Move(float const p_PlannedPositionX = 0.0f, float const p_PlannedPositionY = 0.0f);

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        uint32 GetSpeed()           const { return m_Speed;              }
        float GetPositionX()        const { return m_PositionX;          }
        float GetPositionY()        const { return m_PositionY;          }
        float GetPlannedPositionX() const { return m_PlannedPositionX;   }
        float GetPlannedPositionY() const { return m_PlannedPositionY;   }
        float GetOrientation()      const { return m_Orientation;        }
        bool IsMoving()             const { return m_Moving;             }
        uint32 GetDestinationTime() const { return m_TimeForDestination; }
        uint32 GetLastTimeCalled()  const { return m_LastCalled;         }

        void SetPosition(float const p_PositionX, float const p_PositionY, float const p_PlannedPositionX = 0.0f, float const p_PlannedPositionY = 0.0f) { m_PositionX = p_PositionX; m_PositionY = p_PositionY; if (p_PlannedPositionX) m_PlannedPositionX = p_PlannedPositionX;     if (p_PlannedPositionY) m_PlannedPositionY = p_PlannedPositionY; }
        void SetIsMoving(bool const p_Moving) { m_Moving = p_Moving; }
        void SetSpeed(uint32 const p_Speed)   { m_Speed = p_Speed;   }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        float m_PositionX;
        float m_PositionY;
        float m_PlannedPositionX;
        float m_PlannedPositionY;
        float m_Orientation;
        bool m_Moving;
        uint32 m_Speed;
        uint32 m_TimeForDestination;
        uint32 m_LastCalled;
        std::tuple<uint32, uint32> m_GridIndex;

        Object* m_Object;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone