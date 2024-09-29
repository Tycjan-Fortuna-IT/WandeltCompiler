#include "ScopedTimer.hpp"

namespace WandeltCore
{
	ScopedTimer::ScopedTimer(const char* fmt) : m_Fmt(fmt), m_StartTime(std::chrono::high_resolution_clock::now())
	{
	}

	ScopedTimer::~ScopedTimer()
	{
		auto endTime = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();

		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTime).time_since_epoch().count();

		auto ns = end - start;

		double ms = ns * 0.001;

		SYSTEM_INFO(m_Fmt, ms, ns);
	}
} // namespace WandeltCore
