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

#include "Portal.hpp"
#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Portal::Portal()
    {
        m_Id            = 0;
        m_CompanyId     = Company::NOMAD;
        m_Type          = PortalType::PORTAL_TYPE_NORMAL;
        m_ToMapId       = 0;
        m_ToPositionX   = 0.0f;
        m_ToPositionY   = 0.0f;
        m_Level         = 0;

        SetType(Type::OBJECT_TYPE_PORTAL);
    }
    /// Deconstructor
    Portal::~Portal()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Is Object near Portal
    /// @p_Object : Object being checked
    bool Portal::IsInPortalRadius(Entity::Object* p_Object)
    {
        return Core::Utils::IsInCircleRadius(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(), p_Object->GetSpline()->GetPositionX(), p_Object->GetSpline()->GetPositionY(), PORTAL_RADIUS);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone