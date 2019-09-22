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

#include "Player.hpp"
#include "Station.hpp"
#include "Portal.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Object::Object()
        : m_Spline(this)
    {
        m_Type    = Type::OBJECT_TYPE_NON;
        m_Map     = nullptr;
    }
    /// Deconstructor
    Object::~Object()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Set Type
    /// @p_Type : Type of Object
    void Object::SetType(Type const p_Type)
    {
        m_Type = p_Type;
    }
    /// Return Type
    Type Object::GetType() const
    {
        return m_Type;
    }

    /// Set Object Name
    /// @p_Name : Name of Object
    void Object::SetName(std::string const p_Name)
    {
        m_Name = p_Name;
    }
    /// Return Object Name
    std::string Object::GetName() const
    {
        return m_Name;
    }

    /// Set Object GUID
    /// @p_ObjectGUID : Object GUID
    void Object::SetGUID(ObjectGUID& p_ObjectGUID)
    {
        m_ObjectGUID = p_ObjectGUID;
    }
    /// Return Object GUID
    const ObjectGUID* Object::GetObjectGUID()
    {
        return &m_ObjectGUID;
    }
    /// Returns Object GUID
    uint64 Object::GetGUID() const
    {
        return m_ObjectGUID.GetGUID();
    }

    /// Set Grid Index
    /// @p_GridIndex : Grid Index
    void Object::SetGridIndex(std::tuple<uint32, uint32> const p_GridIndex)
    {
        m_GridIndex = p_GridIndex;
    }
    /// Return Grid Index
    std::tuple<uint32, uint32> Object::GetGridIndex() const
    {
        return m_GridIndex;
    }

    /// Set Map for Object
    void Object::SetMap(Map::Base* p_Map)
    {
        //LOG_ASSERT(p_Map, "Object", "Attempted to assign Object Map to nullptr! for Object %0", GetGUID());

        m_Map = p_Map;
    }
    /// Get Map
    Map::Base* Object::GetMap() const
    {
        return m_Map;
    }
    /// Get Grid
    Map::Grid* Object::GetGrid() const
    {
        return m_Map->GetGrid(this);
    }
    /// Get Spline
    Spline* Object::GetSpline()
    {
        return &m_Spline;
    }

    /// To Player Class
    Player* Object::ToPlayer()
    {
         if (GetType() == Type::OBJECT_TYPE_PLAYER) 
             return reinterpret_cast<Player*>(this);

         return nullptr;
    }
    /// To Portal Class
    Portal* Object::ToPortal()
    {
        if (GetType() == Type::OBJECT_TYPE_PORTAL)
            return reinterpret_cast<Portal*>(this);

        return nullptr;
    }
    /// To Station Class
    Station* Object::ToStation()
    {
        if (GetType() == Type::OBJECT_TYPE_STATION)
            return reinterpret_cast<Station*>(this);

        return nullptr;
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone