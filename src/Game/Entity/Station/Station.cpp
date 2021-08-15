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

#include "Station.hpp"
#include "Utility/UtilMaths.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Station::Station()
    {
        m_Id            = 0;
        m_CompanyId     = Company::NOMAD;
        m_Type          = StationType::STATION_TYPE_UNKNOWN;

        SetType(Type::OBJECT_TYPE_STATION);
    }
    /// Deconstructor
    Station::~Station()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Is Object near Station
    /// @p_Object : Object being checked
    bool Station::IsInStationRadius(Entity::Object* p_Object)
    {
        return Core::Utils::IsInSquareRadius(GetSpline()->GetPositionX(), GetSpline()->GetPositionY(), p_Object->GetSpline()->GetPositionX(), p_Object->GetSpline()->GetPositionY(), STATION_RADIUS);
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone