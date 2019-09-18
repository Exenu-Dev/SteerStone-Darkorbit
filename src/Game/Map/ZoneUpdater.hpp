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
#include <mutex>

namespace SteerStone { namespace Game { namespace Map {

    class Zone;

    /// Zone Updater Task
    class ZoneUpdaterTask
    {
    public:
        /// Constructor
        /// @p_Zone : Zone
        ZoneUpdaterTask(Zone* p_Zone);
        /// Deconstructor
        virtual ~ZoneUpdaterTask();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        virtual void Call() = 0;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        Zone* m_Zone;   ///< Zone
    };

    /// Zone Update Request
    class ZoneUpdateRequest : public ZoneUpdaterTask
    {
        DISALLOW_COPY_AND_ASSIGN(ZoneUpdateRequest);

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        /// @p_Zone : Zone
        /// @p_Diff : Execution Time
        ZoneUpdateRequest(Zone* p_Zone, uint32 const p_Diff);
        /// Deconstructor
        ~ZoneUpdateRequest();

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        /// Call Zone Update
        void Call() override;

        //////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////

    private:
        Zone* m_Zone;   ///< Zone
        uint32 m_Diff;  ///< Execution Time
    };

}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone