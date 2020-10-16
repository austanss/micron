#pragma once

namespace microCORE
{
	namespace Config
	{

		#ifdef I686
		constexpr const char* Architecture = "i686";
		#else
		constexpr const char* Architecture = "[unknown]";
		#endif
	}	
}
