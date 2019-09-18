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

namespace SteerStone { namespace Game { namespace Entity {

    /// Constructor
    Object::Object()
    {
        m_ObjectType    = ObjectType::OBJECT_TYPE_NON;

        m_PositionX     = 0.0f;
        m_PositionY     = 0.0f;
        m_Map           = nullptr;
    }
    /// Deconstructor
    Object::~Object()
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Set Object Type
    /// @p_ObjectType : Type of Object
    void Object::SetObjectType(ObjectType const p_ObjectType)
    {
        m_ObjectType = p_ObjectType;
    }
    /// Return Object Type
    ObjectType Object::GetObjectType() const
    {
        return m_ObjectType;
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
    void Object::SetObjectGUID(ObjectGUID& p_ObjectGUID)
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

    /// Set Map for Object
    void Object::SetMap(Map::Base* p_Map)
    {
        LOG_ASSERT(p_Map, "Object", "Attempted to assign Object Map to nullptr! for Object %0", GetGUID());

        m_Map = p_Map;
    }
    /// Get Map
    Map::Base* Object::GetMap() const
    {
        return m_Map;
    }

    /// Set Object Position
    /// @p_PositionX : X
    /// @p_PositionY : Y
    void Object::SetPosition(float const p_PositionX, float const p_PositionY)
    {
        m_PositionX = p_PositionX;
        m_PositionY = p_PositionY;
    }
    /// Return Position X
    float Object::GetPositionX() const
    {
        return m_PositionX;
    }
    /// Return Position Y
    float Object::GetPositionY() const
    {
        return m_PositionY;
    }

    /// To Player Class
    Player* Object::ToPlayer()
    {
         if (GetObjectType() == ObjectType::OBJECT_TYPE_PLAYER) 
             return reinterpret_cast<Player*>(this);

         return nullptr;
    }

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone