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

#define PING_TIMER 25000

enum class Company : int16
{
    NOMAD       = -1,
    MMO         = 1,
    EARTH       = 2,
    VRU         = 3,
    MAX_COMPANY = 4
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

enum AccessLevel
{
    ACCESS_LEVEL_PLAYER         = 0,
    ACCESS_LEVEL_MODERATOR      = 1,
    ACCESS_LEVEL_ADMIN          = 2,
    ACCESS_LEVEL_DEVELOPER	    = 3,
};

enum RoomType
{
    ROOM_TYPE_NORMAL            = 0,
    ROOM_TYPE_PRIVATE           = 1,
    ROOM_TYPE_CLAN              = 2,
    ROOM_TYPE_SUPPORT		    = 3,
};

enum RoomId
{
	ROOM_ID_GLOBAL              = 1,
	ROOM_ID_MMO 			    = 2,
    ROOM_ID_EIC 			    = 3,
    ROOM_ID_VRU 			    = 4,
    ROOM_ID_CLAN_SEARCH 		= 5,
};