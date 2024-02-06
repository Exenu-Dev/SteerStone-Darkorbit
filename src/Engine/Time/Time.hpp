/*
* Liam Ashdown
* Copyright (C) 2023
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

#include "Core/Core.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace SteerStone { namespace Core { namespace Time {
	
	/// Get the current time in seconds
	/// @return The current time in seconds
	int32 GetTime()
	{
		return static_cast<int32>(std::time(nullptr));
	}

	/// Convert a time to a string
	/// @param p_Time The time to convert
	/// @return The time as a string
	int32 ConvertToTime(std::string p_DateTime)
	{
		std::tm l_Time = {};
		std::istringstream l_Stream(p_DateTime);
		l_Stream >> std::get_time(&l_Time, "%Y-%m-%d %H:%M:%S");
		return static_cast<int32>(std::mktime(&l_Time));
	}
} // namespace Time
} // namespace Core
} // namespace SteerStone
}