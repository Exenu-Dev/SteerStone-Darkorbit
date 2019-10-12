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
        OBJECT_TYPE_NON         = 0,
        OBJECT_TYPE_PLAYER      = 1,
        OBJECT_TYPE_MOB         = 2,
        OBJECT_TYPE_PORTAL      = 3,
        OBJECT_TYPE_STATION     = 4,
        OBJECT_TYPE_BONUS_BOX   = 5
    };

    class Unit;
    class Player;
    class Portal;
    class Station;
    class BonusBox;
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

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        Map::Base* GetMap()                       const { return m_Map;                     }
        Map::Grid* GetGrid()                      const { return m_Map->GetGrid(this);      }
        Spline* GetSpline()                             { return &m_Spline;                 }
        Spline const* GetSpline() const                 { return &m_Spline;                 }
        ObjectGUID const& GetObjectGUID()         const { return m_ObjectGUID;              }
        uint64 GetGUID()                          const { return m_ObjectGUID.GetGUID();    }
        Type GetType()                            const { return m_Type;                    }
        std::string GetName()                     const { return m_Name;                    }
        std::tuple<uint32, uint32> GetGridIndex() const { return m_GridIndex;               }
        bool NeedToBeUpdated()                    const { return m_NeedToBeUpdate;          }

        bool IsPlayer()             const   { return m_Type == Type::OBJECT_TYPE_PLAYER;    }
        bool IsMob()                const   { return m_Type == Type::OBJECT_TYPE_MOB;       }
        bool IsPortal()             const   { return m_Type == Type::OBJECT_TYPE_PORTAL;    }
        bool IsStation()            const   { return m_Type == Type::OBJECT_TYPE_STATION;   }
        bool IsBonusBox()           const   { return m_Type == Type::OBJECT_TYPE_BONUS_BOX; }

        Unit* ToUnit()                      {                                               return reinterpret_cast<Unit*>(this);                               }
        Unit const* ToUnit()        const   {                                               return reinterpret_cast<Unit const*>(this);                         }
        Player* ToPlayer()                  { if (GetType() == Type::OBJECT_TYPE_PLAYER)    return reinterpret_cast<Player*>(this);             return nullptr; }
        Player const* ToPlayer()    const   { if (GetType() == Type::OBJECT_TYPE_PLAYER)    return reinterpret_cast<Player const*>(this);       return nullptr; }
        Portal* ToPortal()                  { if (GetType() == Type::OBJECT_TYPE_PORTAL)    return reinterpret_cast<Portal*>(this);             return nullptr; }
        Portal const* ToPortal()    const   { if (GetType() == Type::OBJECT_TYPE_PORTAL)    return reinterpret_cast<Portal const*>(this);       return nullptr; }
        Station* ToStation()                { if (GetType() == Type::OBJECT_TYPE_STATION)   return reinterpret_cast<Station*>(this);            return nullptr; }
        Station const* ToStation()  const   { if (GetType() == Type::OBJECT_TYPE_STATION)   return reinterpret_cast<Station const*>(this);      return nullptr; }
        Mob* ToMob()                        { if (GetType() == Type::OBJECT_TYPE_MOB)       return reinterpret_cast<Mob*>(this);                return nullptr; }
        Mob const* ToMob()          const   { if (GetType() == Type::OBJECT_TYPE_MOB)       return reinterpret_cast<Mob const*>(this);          return nullptr; }
        BonusBox* ToBonusBox()              { if (GetType() == Type::OBJECT_TYPE_BONUS_BOX) return reinterpret_cast<BonusBox*>(this);           return nullptr; }
        BonusBox const* ToBonusBox()const   { if (GetType() == Type::OBJECT_TYPE_BONUS_BOX) return reinterpret_cast<BonusBox const*>(this);     return nullptr; }

        void SetGridIndex(std::tuple<uint32, uint32> const p_GridIndex) { m_GridIndex = p_GridIndex;   }
        void SetName(std::string const p_Name)                          { m_Name = p_Name;             }
        void SetType(Type const p_Type)                                 { m_Type = p_Type;             }
        void SetGUID(ObjectGUID const& p_ObjectGUID)                    { m_ObjectGUID = p_ObjectGUID; }
        void SetNeedToBeUpdated(bool const p_Update)                    { m_NeedToBeUpdate = p_Update; }
        void SetMap(Map::Base* p_Map)                                   { LOG_ASSERT(p_Map, "Object", "Attempted to assign Object Map to nullptr! for Object %0", GetGUID());  m_Map = p_Map; }
    
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    private:
        Type m_Type;
        ObjectGUID m_ObjectGUID;    
        std::string m_Name;
        bool m_NeedToBeUpdate;
        Spline m_Spline;
        Map::Base* m_Map;
        std::tuple<uint32, uint32> m_GridIndex;
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone