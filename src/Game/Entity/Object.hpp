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
        OBJECT_TYPE_NON     = 0,
        OBJECT_TYPE_PLAYER  = 1,
        OBJECT_TYPE_NPC     = 2,
        OBJECT_TYPE_PORTAL  = 3,
        OBJECT_TYPE_STATION = 4
    };

    class Unit;
    class Player;
    class Portal;
    class Station;
    class Mob;

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
        ObjectGUID const& GetObjectGUID();
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
        /// Get Grid
        Map::Grid* GetGrid() const;
        /// Get Spline
        Spline* GetSpline();

        /// To Unit Class
        Unit* ToUnit();
        /// To Player Class
        Player* ToPlayer();
        /// To Portal Class
        Portal* ToPortal();
        /// To Station Class
        Station* ToStation();
        /// To Mob Class
        Mob* ToMob();

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