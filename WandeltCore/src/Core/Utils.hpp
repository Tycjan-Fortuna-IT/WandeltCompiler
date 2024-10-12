/**
 * @file Utils.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-10-05
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

#define valueOrReturnNullptr(type, variable, value) \
	type variable = value;                          \
	if (!variable)                                  \
		return nullptr;

namespace WandeltCore
{
	struct SourceLocation
	{
		std::string_view Filepath;
		u32 Line;
		u32 Column;
	};

	[[nodiscard]] static std::string getIndent(u32 level, u32 multiplier = 2)
	{
		return std::string(level * multiplier, ' ');
	}
} // namespace WandeltCore
