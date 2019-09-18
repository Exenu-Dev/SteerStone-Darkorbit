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

#include "ZoneUpdater.hpp"
#include "Zone.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_Zone : Zone
    ZoneUpdaterTask::ZoneUpdaterTask(Zone* p_Zone)
        : m_Zone(p_Zone)
    { 
    }
    /// Deconstructor
    ZoneUpdaterTask::~ZoneUpdaterTask()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    /// @p_Zone : Zone
    /// @p_Diff : Execution Time
    ZoneUpdateRequest::ZoneUpdateRequest(Zone* p_Zone, uint32 const p_Diff)
        : ZoneUpdaterTask(p_Zone), m_Zone(p_Zone), m_Diff(p_Diff)
    {
    }
    /// Deconstructor
    ZoneUpdateRequest::~ZoneUpdateRequest()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Call Zone Update
    void ZoneUpdateRequest::Call()
    {
        /// Update all of our maps
        m_Zone->UpdateMaps(m_Diff);
    }

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone