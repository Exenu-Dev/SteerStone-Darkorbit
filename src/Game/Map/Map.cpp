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

#include "Map.hpp"
#include "Player.hpp"
#include "Database/DatabaseTypes.hpp"

namespace SteerStone { namespace Game { namespace Map {

    /// Constructor
    /// @p_Id : Map Id
    Base::Base(uint32 const p_Id)
        : m_Id(p_Id)
    { 
        /// Load Grids
        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
                m_Grids[l_X][l_Y] = new Grid(l_X, l_Y);
    }
    /// Deconstructor
    Base::~Base()
    {
    }

    /// Get Map Id
    uint32 Base::GetId() const
    {
        return m_Id;
    }

    /// Calculate Grid By Object Position
    /// @p_Object : Object
    std::tuple<uint32, uint32> Base::CalculateGridByPosition(Entity::Object* p_Object)
    {
        /// We need to calculate which grid the player will be in
        float l_PositionX = p_Object->GetPositionX();
        float l_PositionY = p_Object->GetPositionY();
        uint32 l_PositionOffset = (l_PositionX + l_PositionY) / GRID_SEARCH_RADIUS;

        /// This is a quicker way of determining we are inside the 64 grid range
        if (l_PositionOffset > MAX_GRIDS)
        {
            LOG_WARNING("Map", "Attempted to add Player %0 to grid but grid is larger than max grid! kicking player!", p_Object->ToPlayer()->GetName());
            p_Object->ToPlayer()->KickPlayer();
            return std::make_tuple(0, 0); ///< To make compiler happy
        }

        /// Now grab the Grid X and Grid Y index
        float l_GridX = l_PositionX / GRID_SEARCH_RADIUS;
        if (l_GridX < 1)
            l_GridX = 0;
        else
            l_GridX = std::ceil(((l_GridX / MAX_GRIDS) * 10)) + 1;

        /// The width is bigger than the height
        /// Map Size is 21000x14100, so we need to cut the search radius in half to get the index we are looking for
        float l_GridY = l_PositionY / GRID_SEARCH_RADIUS_HALFED;
        if (l_GridY < 1)
            l_GridY = 0;
        else
            l_GridY = std::floor((l_GridY / MAX_GRIDS * 10)) - 1;

        /// Is our grid out of bounds?
        if (l_GridX > GRID_CELLS - 1 || l_GridY > GRID_CELLS - 1) ///< Grid starts from 0
            LOG_ASSERT(false, "Map", "Grid X or Y is larger than Grid Cells! Grid X: %0, Grid Y: %1", l_GridX, l_GridY);

        return std::make_tuple(static_cast<uint32>(l_GridX), static_cast<uint32>(l_GridY));
    }

    /// Add Object to map
    /// @p_Object : Object being added to map
    void Base::Add(Entity::Object* p_Object)
    {
        std::tuple<uint32, uint32> l_GridIndex = CalculateGridByPosition(p_Object);

        m_Grids[std::get<0>(l_GridIndex)][std::get<1>(l_GridIndex)]->Add(p_Object);
    }
    /// Remove Object from map
    /// @p_Object : Object being removed from map
    void Base::Remove(Entity::Object* p_Object)
    {
        std::tuple<uint32, uint32> l_GridIndex = CalculateGridByPosition(p_Object);

        m_Grids[std::get<0>(l_GridIndex)][std::get<1>(l_GridIndex)]->Remove(p_Object);
    }

    /// Update Maps
    /// @p_Diff : Execution Time
    bool Base::Update(uint32 const p_Diff)
    {
        for (uint32 l_X = 0; l_X < GRID_CELLS; l_X++)
        {
            for (uint32 l_Y = 0; l_Y < GRID_CELLS; l_Y++)
            {
                Grid* l_Grid = m_Grids[l_X][l_Y];

                /// If there's no players inside the grid, then no point in updating
                if (l_Grid->GetState() == State::Idle)
                    continue;

                l_Grid->Update(p_Diff);
            }
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

   
}   ///< namespace Map
}   ///< namespace Game
}   ///< namespace Steerstone