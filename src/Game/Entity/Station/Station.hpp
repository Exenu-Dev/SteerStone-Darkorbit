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

#define STATION_RADIUS 500ul

namespace SteerStone { namespace Game { namespace Entity {

    /// Portal
    class Station : public Unit
    {
        DISALLOW_COPY_AND_ASSIGN(Station);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        friend class Map::Base;

    public:
        /// Constructor
        Station();
        /// Deconstructor
        ~Station();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Is Object near Station
        /// @p_Object : Object being checked
        bool IsInStationRadius(Entity::Object* p_Object);

        /// Getters Functions
        uint32 GetId()         const { return m_Id;          }
        Company GetCompany()   const { return m_CompanyId;   }
        StationType GetType()  const { return m_Type;        }
        bool IsPeace()         const { return m_Peace;       }

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        uint32 m_Id;
        Company m_CompanyId;
        StationType m_Type;
        bool m_Peace;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone