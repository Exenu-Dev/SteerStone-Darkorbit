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

#include "Diagnostic/DiaIntervalTimer.hpp"

#define ATTACK_UPDATE_INTERVAL 1000ul
#define MAX_RESOURCE_COUNTER 9ul

namespace SteerStone { namespace Game { namespace Entity {

    /// Death State of unit
    enum DeathState
    {
        ALIVE           = 0,
        JUST_DIED       = 1,
        DEAD            = 3,
        JUST_RESPAWNED  = 4,
    };

    enum AttackState
    {
        ATTACK_STATE_NONE           = 0,
        ATTACK_STATE_IN_RANGE       = 1,
        ATTACK_STATE_OUT_OF_RANGE   = 2,
    };

    enum Resource
    {
        RESOURCE_PROMETIUM  = 0,
        RESOURCE_ENDURIUM   = 1,
        RESOURCE_TERBIUM    = 2,
        RESOURCE_XENOMIT    = 3,
        RESOURCE_PROMETID   = 4,
        RESOURCE_DURANIUM   = 5,
        RESOURCE_PROMERIUM  = 6,
        RESOURCE_PALLADIUM  = 7,
        RESOURCE_SEPROM     = 8
    };

    class Unit : public Object
    {
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

        /// Update
        /// @p_Diff : Execution Time
        void Update(uint32 const p_Diff);

        ///////////////////////////////////////////
        //             ATTACK SYSTEM
        ///////////////////////////////////////////

        /// Attack
        /// @p_Victim : Victim we are attacking
        void Attack(Unit* p_Victim);
        /// Update Attack
        /// @p_Diff : Execution Time
        void AttackerStateUpdate(uint32 const p_Diff);
        /// Cancel Attack
        void CancelAttack();
        /// Calculate Hit chance whether we can hit target
        bool CalculateHitChance();
        /// Calculate Damage done for target
        uint32 CalculateDamageDone();
        /// Deal Damage to target
        /// @p_Target : Target
        /// @p_Damage : Damage
        /// @p_CleanDamage : Deal damage neglecting shield
        void DealDamage(Unit* p_Target, int32 p_Damage, bool p_CleanDamage);
        /// Calculate Damage takem for target
        /// @p_Target : Target
        /// @p_Damage : Damage taken
        /// @p_ShieldDamage : Shield Damage taken
        void CalculateDamageTaken(Unit* p_Target, int32& p_Damage, int32& p_ShieldDamage);
        /// Returns whether unit is attacking us
        /// @p_Unit : Unit attacking us
        bool IsAttackingMe(Unit* p_Unit) const;
        /// Kill
        /// @p_Unit : Unit being killed
        void Kill(Unit* p_Unit);

        ///////////////////////////////////////////
        //                TARGET
        ///////////////////////////////////////////

        void SetTarget(Unit* p_Target)          { m_Target = p_Target; m_TargetGUID = p_Target->GetGUID();  }
        bool HasTarget()                        { return m_Target != nullptr;                               }
        Unit* GetTarget()                       { return m_Target;                                          }
        Unit const* GetTarget() const           { return m_Target;                                          }
        uint64 GetTargetGUID() const            { return m_TargetGUID;                                      }
        void ClearTarget()                      { m_Target = nullptr; m_TargetGUID = 0;                     }

        ///////////////////////////////////////////
        //            GETTERS/SETTERS
        ///////////////////////////////////////////
    public:
        int32 GetShield()           const { return m_Shield;            }
        uint32 GetShieldResistance()const { return m_ShieldResistance;  }
        uint32 GetMaxShield()       const { return m_MaxShield;         }
        int32 GetHitPoints()        const { return m_HitPoints;         }
        uint32 GetMinDamage()       const { return m_MinDamage;         }    
        uint32 GetMaxDamage()       const { return m_MaxDamage;         }    
        uint32 GetHitMaxPoints()    const { return m_MaxHitPoints;      }
        uint16 GetShipType()        const { return m_ShipType;          }
        Company GetCompany()        const { return m_Company;           }
        uint16 GetRank()            const { return m_Rank;              }
        uint16 GetGatesAchieved()   const { return m_GatesAchieved;     }
        uint32 GetClanId()          const { return m_ClanId;            }
        std::string GetClanName()   const { return m_ClanName;          }
        uint16 GetWeaponState()     const { return m_WeaponState;       }
        uint16 GetDeathState()      const { return m_DeathState;        }
        uint16 GetLaserType()       const { return m_LaserType;         }    
        uint32 GetExperience()      const { return m_Experience;        }
        uint16 GetBehaviour()       const { return m_Behaviour;         }
        uint32 GetRespawnTimer()    const { return m_RespawnTimer;      }
        uint32 GetHonor()           const { return m_Honor;             }
        uint32 GetCredits()         const { return m_Credits;           }
        uint32 GetUridium()         const { return m_Uridium;           }
        uint32 GetResource(uint32 const p_Index) const 
        {
            if (p_Index > MAX_RESOURCE_COUNTER)
                LOG_ASSERT(false, "Unit", "Attempted to get resource but index is unknown! Index: %0", p_Index);

            return m_Resources[p_Index];
        }

        bool IsAttacking()          const { return m_Attacking;                      }
        bool IsDead()               const { return m_DeathState == DeathState::DEAD; }

        void SetHonor(int32 const p_Honor)              { m_Honor = p_Honor;            }
        void SetWeaponState(uint16 const p_WeaponState) { m_WeaponState = p_WeaponState;}
        void SetHitPoints(uint32 const p_HitPoints)     { m_HitPoints = p_HitPoints;    }
        void SetShield(uint32 const p_Shield)           { m_Shield = p_Shield;          }
        void SetLaserType(uint16 const p_LaserType)     { m_LaserType = p_LaserType;    }
        void SetRocketType(uint16 const p_RocketType)   { m_RocketType = p_RocketType;  }
        void SetStats(uint32 const p_MinDamage, uint32 const p_MaxDamage, uint32 const p_Speed, uint32 const p_Shield, uint32 const p_ShieldResistance)
        {
            m_MinDamage         = p_MinDamage;
            m_MaxDamage         = p_MaxDamage;
            GetSpline()->SetSpeed(GetSpline()->GetSpeed() + p_Speed);
            m_MaxShield         += p_Shield;
            m_Shield            = m_MaxShield;
            m_ShieldResistance  = p_ShieldResistance;
        }
        void SetResource(uint32 const p_Index, uint32 const p_Resource);

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    protected:
        int32 m_HitPoints;
        uint32 m_MaxHitPoints;
        int32 m_Shield;
        uint32 m_ShieldResistance;
        uint32 m_MaxShield;
        uint32 m_MinDamage;
        uint32 m_MaxDamage;
        Company m_Company;
        uint16 m_ShipType;
        uint16 m_Rank;
        uint16 m_GatesAchieved;
        uint32 m_ClanId;
        std::string m_ClanName;
        uint16 m_WeaponState;
        uint16 m_LaserType;
        uint16 m_RocketType;
        uint16 m_DeathState;
        uint16 m_AttackRange;
        uint16 m_AttackState;
        uint32 m_SelectedLaser;
        uint32 m_SelectedRocket;
        uint32 m_Experience;
        uint16 m_Behaviour;
        uint32 m_RespawnTimer;
        int32 m_Honor;
        uint32 m_Respawn;
        uint32 m_Credits;
        uint32 m_Uridium;
        uint32 m_Resources[9];

        uint32 m_LastTimeAttacked;
        bool m_Attacking;
        Core::Diagnostic::IntervalTimer m_IntervalAttackUpdate;

        Unit* m_Target;
        uint64 m_TargetGUID;

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    };

}   ///< namespace Entity
}   ///< namespace Game
}   ///< namespace Steerstone