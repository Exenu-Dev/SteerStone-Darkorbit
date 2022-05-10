/*
* Liam Ashdown
* Copyright (C) 2022
*
* Thisprogram isfree software: you can redistribute it and/or modify
* it under the termsof the GNU General Public License aspublished by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Thisprogram isdistributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY, without even the implied warranty of
* MERCHANTABILITY or FITNESsFOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with thisprogram.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <PCH/Precompiled.hpp>
#include "Core/Core.hpp"

#define PLAYER_RADIUS_SCAN 1000.0f

enum PlayerShips
{
    Phoenix     = 1,
    Yamato      = 2,
    Leonov      = 3,
    Defcom      = 4,
    Liberator   = 5,
    Piranha     = 6,
    Nostromo    = 7,
    Vengeance   = 8,
    Bigboy      = 9,
    Goliath     = 10,
    GGoliath    = 52,
    CBSGoliath  = 53
};

enum NpcShips
{
    Streuner            = 1,
    Lordakia            = 2,
    Devolarium          = 3,
    Mordon              = 4,
    Sibelon             = 5,
    Saimon              = 6,
    Sibelonit           = 7,
    Lordakium           = 8,
    Kristallin          = 9,
    Kristallon          = 10,
    StreuneR            = 11,
    Protegit            = 12,
    Cubikon             = 13,
    BossStreuner        = 14,
    BossLordakia        = 15,
    BossMordon          = 16,
    BossSaimon          = 17,
    BossDevolarium      = 18,
    BossSibelonit       = 19,
    BossSibelon         = 20,
    BossLordakium       = 21,
    BossKristallin      = 22,
    BossKristallon      = 23,
    BossStreuneR        = 24,
    UFO                 = 25,
    UFONIT              = 26,
    AggroStreuner       = 27,
    UberStreuner        = 28,
    UberLordakia        = 29,
    UberMordon          = 30,
    UberSaimon          = 31,
    UberDevolarium      = 32,
    UberSibelonit       = 33,
    UberSibelon         = 34,
    UberLordakium       = 35,
    UberKristallin      = 36,
    UberKristallon      = 37,
    UberStreuneR        = 38,
};

enum MiscConditionType
{
    NOTHING               = 0,
    ON_ENEMY_MAP          = 1,
    ON_PVP_MAP            = 2,
    VISIBLE               = 3,
    INVISIBLE             = 4,
    IN_RADIATION_AREA     = 5,
    INSTANT_SHIELD_ACTIVE = 6,
    LOW_ON_LIFE           = 7,
    ATTACKING             = 8,
    ATTACKED              = 9,
    MARTYR                = 10,
    REPAIRING             = 11,
    OUTLAW                = 12,
    ON_HOME_MAP           = 13,
    ON_HOSTILE_HOME_MAP   = 14,
    ON_MARS_MAP           = 15,
    ON_EARTH_MAP          = 16,
    ON_VENUS_MAP          = 17,
    ON_OWN_FACTION_MAP    = 18,
    ON_X_1                = 19,
    ON_X_2                = 20,
    ON_X_3                = 21,
};

enum ConditionType
{
    QUESTCASE            = 0,
    TIMER                = 1,
    HASTE                = 2,
    ENDURANCE            = 3,
    COUNTDOWN            = 4,
    COLLECT              = 5,
    KILL_NPC             = 6,
    DAMAGE               = 7,
    AVOID_DAMAGE         = 8,
    TAKE_DAMAGE          = 9,
    AVOID_DEATH          = 10,
    COORDINATES          = 11,
    DISTANCE             = 12,
    TRAVEL               = 13,
    FUEL_SHORTAGE        = 14,
    PROXIMITY            = 15,
    MAP                  = 16,
    MAP_DIVERSE          = 17,
    EMPTY                = 18,
    MISCELLANEOUS        = 19,
    AMMUNITION           = 20,
    SAVE_AMMUNITION      = 21,
    SPEND_AMMUNITION     = 22,
    SALVAGE              = 23,
    STEAL                = 24,
    KILL                 = 25,
    DEAL_DAMAGE          = 26,
    KILL_NPCS            = 27,
    KILL_PLAYERS         = 28,
    DAMAGE_NPCS          = 29,
    DAMAGE_PLAYERS       = 30,
    VISIT_MAP            = 31,
    DIE                  = 32,
    AVOID_KILL_NPC       = 33,
    AVOID_KILL_NPCS      = 34,
    AVOID_KILL_PLAYERS   = 35,
    AVOID_DAMAGE_NPCS    = 36,
    AVOID_DAMAGE_PLAYERS = 37,
    PREVENT              = 38,
    JUMP                 = 39,
    AVOID_JUMP           = 40,
    STEADINESS           = 41,
    MULTIPLIER           = 42,
    STAY_AWAY            = 43,
    IN_GROUP             = 44,
    KILL_ANY             = 45,
    WEB                  = 46,
    CLIENT               = 47,
    cCARGO               = 48,
    SELL_ORE             = 49,
    LEVEL                = 50,
};

enum DamageType
{
    DAMAGE_TYPE_ANY         = 0,
    DAMAGE_TYPE_LASER       = 1,
    DAMAGE_TYPE_ROCKET      = 2,
    DAMAGE_TYPE_MINE        = 3,
    DAMAGE_TYPE_RADIATION   = 4,
    DAMAGE_TYPE_DIRECT      = 5
};

enum class Company : uint16
{
    NOMAD        = 0,
    MMO          = 1,
    EARTH        = 2,
    VRU          = 3,
    MAX_COMPANY  = 4
};

enum InventoryType
{
    INVENTORY_TYPE_BATTERY              = 0,
    INVENTORY_TYPE_SPEED_GENERATOR      = 1,
    INVENTORY_TYPE_SHIELD_GENERATOR     = 2,
};

enum RocketType
{
    ROCKET_TYPE_NONE     = 0,
    ROCKET_TYPE_R310     = 1,
    ROCKET_TYPE_PLT_2026 = 2,
    ROCKET_TYPE_PLT_2021 = 3,
    MAX_ROCKET           = 4
};

enum RocketDamage
{
    ROCKET_DAMAGE_R310        = 1000,
    ROCKET_DAMAGE_PLT_2026    = 2000,
    ROCKET_DAMAGE_PLT_2021    = 4000
};

enum BatteryType : uint16
{
    BATTERY_TYPE_NONE   = 0,
    BATTERY_TYPE_LCB10  = 1,
    BATTERY_TYPE_MCB25  = 2,
    BATTERY_TYPE_MCB50  = 3,
    BATTERY_TYPE_UCB100 = 4,
    BATTERY_TYPE_SAB50  = 5,
    MAX_BATTERY         = 6
};

enum MinesType
{
    MINE            = 0,
    SMART_BOMB      = 1,
    INSTANT_SHIELD  = 2,
    MAX_MINES       = 3
};

enum class PortalType : uint16
{
    PORTAL_TYPE_NORMAL      = 0,
    PORTAL_TYPE_GALAXY_GATE = 1,
};

enum class StationType : uint16
{
    STATION_TYPE_NORMAL     = 0,
    STATION_TYPE_UNKNOWN    = 1
};

enum Rank
{
    BASIC_SPACE_PILOT = 1,
    SPACE_PILOT       = 2,
    CHIEF_SPACE_PILOT = 3,
    BASIC_SERGEANT    = 4,
    SERGEANT          = 5,
    CHIEF_SERGEANT    = 6,
    BASIC_LIEUTENANT  = 7,
    LIEUTENANT        = 8,
    CHIEF_LIEUTENANT  = 9,
    BASIC_CAPTAIN     = 10,
    CAPTAIN           = 11,
    CHIEF_CAPTAIN     = 12,
    BASIC_MAJOR       = 13,
    MAJOR             = 14,
    CHIEF_MAJOR       = 15,
    BASIC_COLONEL     = 16,
    COLONEL           = 17,
    CHIEF_COLONEL     = 18,
    BASIC_GENERAL     = 19,
    GENERAL           = 20,
    ADMIN             = 21,
};

enum class EventType
{
    EVENT_TYPE_NONE    = 0,
    EVENT_TYPE_PORTAL  = 1,
    EVENT_TYPE_STATION = 2,
    EVENT_TYPE_RADIATION_ZONE = 3
};

enum class MapType
{
    MAP_TYPE_NORMAL,
    MAP_TYPE_BIG
};

enum DroneType
{
    DRONE_TYPE_FLAK = 0,
    DRONE_TYPE_IRIS = 1
};

enum DroneLevel
{
    DRONE_LEVEL_1 = 1,
    DRONE_LEVEL_2 = 2,
    DRONE_LEVEL_3 = 3,
    DRONE_LEVEL_4 = 4,
    DRONE_LEVEL_5 = 5,
    DRONE_LEVEL_6 = 6
};

enum class BonusBoxType
{
    BONUS_BOX_TYPE_CARGO = 0,
};

enum WeaponState
{
    WEAPON_STATE_NOT_EQUIPPED   = 0,
    WEAPON_STATE_SEMI_EQUIPPED  = 1,
    WEAPON_STATE_FULLY_EQUIPPED = 2
};

namespace LevelExperience
{
    enum Level : uint64
    {
        LEVEL_1 = 0,
        LEVEL_2 = 10000,
        LEVEL_3 = 20000,
        LEVEL_4 = 40000,
        LEVEL_5 = 80000,
        LEVEL_6 = 160000,
        LEVEL_7 = 320000,
        LEVEL_8 = 640000,
        LEVEL_9 = 1280000,
        LEVEL_10 = 2560000,
        LEVEL_11 = 5120000,
        LEVEL_12 = 10240000,
        LEVEL_13 = 20480000,
        LEVEL_14 = 40960000,
        LEVEL_15 = 81920000,
        LEVEL_16 = 163840000,
        LEVEL_17 = 327680000,
        LEVEL_18 = 655360000,
        LEVEL_19 = 1310720000,
        LEVEL_20 = 2621440000,
        LEVEL_21 = 5242880000,
        LEVEL_22 = 10485760000,
        LEVEL_23 = 20971520000,
        LEVEL_24 = 41943040000,
        LEVEL_25 = 83886080000,
        LEVEL_26 = 167772160000,
        LEVEL_27 = 335544320000,
        LEVEL_28 = 671088640000,
        MAX_LEVEL = 28
    };

    static const Level All[] = { LEVEL_1, LEVEL_2, LEVEL_3,
                                 LEVEL_4, LEVEL_5, LEVEL_6,
                                 LEVEL_7, LEVEL_8, LEVEL_9,
                                 LEVEL_10, LEVEL_11, LEVEL_12,
                                 LEVEL_13, LEVEL_14, LEVEL_15,
                                 LEVEL_16, LEVEL_17, LEVEL_18,
                                 LEVEL_19, LEVEL_20, LEVEL_21,
                                 LEVEL_22, LEVEL_23, LEVEL_24,
                                 LEVEL_25, LEVEL_26, LEVEL_27,
                                 LEVEL_28 };

}   ///< namespace LevelExperience

enum class LogBookType : uint16
{
    LOG_BOOK_TYPE_OVERVIEW = 0,
    LOG_BOOK_TYPE_DETAILED = 1
};