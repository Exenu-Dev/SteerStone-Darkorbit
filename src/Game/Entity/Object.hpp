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
#include "ObjectSpline.hpp"
#include "ObjectGUID.hpp"
#include "Map.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    enum class Type
    {
        OBJECT_TYPE_NON,
        OBJECT_TYPE_PLAYER,
        OBJECT_TYPE_NPC,
        OBJECT_TYPE_PORTAL,
    };

    class Player;

    class Object
    {
        DISALLOW_COPY_AND_ASSIGN(Object);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        Object();
        /// Deconstructor
        virtual ~Object();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Set Object Type
        /// @p_Type : Type of Object
        void SetType(Type const p_Type);
        /// Return Object Type
        Type GetType() const;

        /// Set Object Name
        /// @p_Name : Name of Object
        void SetName(std::string const p_Name);
        /// Return Object Name
        std::string GetName() const;

        /// Set Object GUID
        /// @p_ObjectGUID : Object GUID
        void SetGUID(ObjectGUID& p_ObjectGUID);
        /// Return Object GUID
        const ObjectGUID* GetObjectGUID();
        /// Returns Object GUID
        uint64 GetGUID() const;

        /// Set Grid Index
        /// @p_GridIndex : Grid Index
        void SetGridIndex(std::tuple<uint32, uint32> const p_GridIndex);
        /// Return Grid Index
        std::tuple<uint32, uint32> GetGridIndex() const;

        /// Set Map for Object
        void SetMap(Map::Base* p_Map);
        /// Get Map
        Map::Base* GetMap() const;

        /// Returns Spline
        Spline* GetSpline();

        /// To Player Class
        Player* ToPlayer();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        Type m_Type;
        ObjectGUID m_ObjectGUID;
        std::string m_Name;

        Spline m_Spline;
        Map::Base* m_Map;
        std::tuple<uint32, uint32> m_GridIndex;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone