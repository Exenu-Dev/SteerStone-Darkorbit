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
#include "Unit.hpp"
#include "GameFlags.hpp"

#define PORTAL_RADIUS 500ul

namespace SteerStone { namespace Game { namespace Entity {

    /// Portal
    class Portal : public Object
    {
        DISALLOW_COPY_AND_ASSIGN(Portal);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        friend class Map::Base;

    public:
        /// Constructor
        Portal();
        /// Deconstructor
        ~Portal();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Is Object near Portal
        /// @p_Object : Object being checked
        bool IsInPortalRadius(Entity::Object* p_Object);

        /// Getters Functions
        uint32 GetId()         const { return m_Id;          }
        Company GetCompany()   const { return m_CompanyId;   }
        PortalType GetType()   const { return m_Type;        }
        uint32 GetToMapId()    const { return m_ToMapId;     }
        float GetToPositionX() const { return m_ToPositionX; }
        float GetToPositionY() const { return m_ToPositionY; }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        uint32 m_Id;
        Company m_CompanyId;
        PortalType m_Type;
        uint32 m_ToMapId;
        float m_ToPositionX;
        float m_ToPositionY;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone