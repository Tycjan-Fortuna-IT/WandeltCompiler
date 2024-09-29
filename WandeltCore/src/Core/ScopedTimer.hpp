/**
 * @file ScopedTimer.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

namespace WandeltCore
{
	class ScopedTimer
	{
	public:
		ScopedTimer(const char* fmt);
		~ScopedTimer();

	private:
		const char* m_Fmt;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	};
} // namespace WandeltCore
