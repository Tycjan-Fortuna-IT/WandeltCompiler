/**
 * @file Utils.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-10-05
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

namespace WandeltCore
{
	struct SourceLocation
	{
		std::string_view Filepath;
		u32 Line;
		u32 Column;
	};

	template <typename T>
	FORCE_INLINE T* valueOrReturnNullptr(T* value)
	{
		if (!value)
			return nullptr;

		return value;
	}

	[[nodiscard]] static std::string getIndent(u32 level)
	{
		return std::string(level * 2, ' ');
	}
} // namespace WandeltCore
