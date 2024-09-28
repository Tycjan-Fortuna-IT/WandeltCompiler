/**
 * @file FileSystem.hpp
 * @author SW
 * @version 0.0.1
 * @date 2024-09-28
 *
 * @copyright Copyright (c) 2024 SW
 */
#pragma once

namespace WandeltCore
{
	class FileSystem
	{
	public:
		// Check if a file system entry exists at the given path.
		static bool Exists(const char* path);
		static bool Exists(const std::string& path);
		static bool Exists(const std::filesystem::path& path);

		// Read the contents of a file at the given path.
		// The file must exist.
		static std::string ReadFile(const char* filepath);
		static std::string ReadFile(const std::string& filepath);
		static std::string ReadFile(const std::filesystem::path& filepath);
	};
} // namespace WandeltCore
