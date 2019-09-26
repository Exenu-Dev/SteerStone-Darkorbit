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
#include "Object.hpp"

namespace SteerStone { namespace Game { namespace Entity {

    /// Death State of unit
    enum DeathState
    {
        ALIVE           = 0,
        JUST_DIED       = 1,
        DEAD            = 3,
        JUST_RESPAWNED  = 4,
    };

    class Unit : public Object
    {
        DISALLOW_COPY_AND_ASSIGN(Unit);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    public:
        /// Constructor
        Unit();
        /// Deconstructor
        virtual ~Unit();

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////
        //                GENERAL
        ///////////////////////////////////////////

        /// Attack
        /// @p_Victim : Victim we are attacking
        void Attack(Object* p_Victim);

        /// Update Attack
        /// @p_Diff : Execution Time
        void AttackerStateUpdate(uint32 const p_Diff);

        /// Update
        /// @p_Diff : Execution Time
        void Update(uint32 const p_Diff);

        ///////////////////////////////////////////
        //                TARGET
        ///////////////////////////////////////////

        void SetTarget(Object const* p_Target)  { m_Target = p_Target; m_TargetGUID = p_Target->GetGUID();  }
        bool HasTarget()                        { return m_Target != nullptr;                               }
        Object const* GetTarget()               { return m_Target;                                          }
        uint64 GetTargetGUID() const            { return m_TargetGUID;                                      }
        void ClearTarget()                      { m_Target = nullptr; m_TargetGUID = 0;                     }

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        int32 GetShield()          const { return m_Shield;         }
        uint32 GetMaxShield()      const { return m_MaxShield;      }
        int32 GetHitPoints()       const { return m_HitPoints;      }
        uint32 GetMinDamage()      const { return m_MinDamage;      }    
        uint32 GetMaxDamage()      const { return m_MaxDamage;      }    
        uint32 GetHitMaxPoints()   const { return m_MaxHitPoints;   }
        Company GetCompany()       const { return m_Company;        }
        uint16 GetRank()           const { return m_Rank;           }
        uint16 GetGatesAchieved()  const { return m_GatesAchieved;  }
        uint32 GetClanId()         const { return m_ClanId;         }
        std::string GetClanName()  const { return m_ClanName;       }
        uint16 GetWeaponState()    const { return m_WeaponState;    }
        uint16 GetDeathState()     const { return m_DeathState;     }

        void SetLaserType(uint16 const p_LaserType)   { m_LaserType = p_LaserType;   }
        void SetRocketType(uint16 const p_RocketType) { m_RocketType = p_RocketType; }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    protected:
        int32 m_HitPoints;
        uint32 m_MaxHitPoints;
        int32 m_Shield;
        uint32 m_MaxShield;
        uint32 m_MinDamage;
        uint32 m_MaxDamage;
        Company m_Company;
        uint16 m_Rank;
        uint16 m_GatesAchieved;
        uint32 m_ClanId;
        std::string m_ClanName;
        uint16 m_WeaponState;
        uint16 m_LaserType;
        uint16 m_RocketType;
        uint16 m_DeathState;
        bool m_Attacking;

        Object const* m_Target;
        uint64 m_TargetGUID;

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone