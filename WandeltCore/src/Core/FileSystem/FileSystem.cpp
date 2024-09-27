#include "FileSystem.hpp"

#include <fstream>

namespace WandeltCore
{
	bool FileSystem::Exists(const char* path)
	{
		return std::filesystem::exists(path);
	}

	bool FileSystem::Exists(const std::string& path)
	{
		return std::filesystem::exists(path);
	}

	bool FileSystem::Exists(const std::filesystem::path& path)
	{
		return std::filesystem::exists(path);
	}

	std::string FileSystem::ReadFile(const char* filepath)
	{
		std::string result;

		std::ifstream stream(filepath, std::ios::in | std::ios::binary);

		ASSERT(stream.is_open(), "Could not open file {}", filepath);

		stream.seekg(0, std::ios::end);
		result.resize(static_cast<size_t>(stream.tellg()));

		stream.seekg(0, std::ios::beg);
		stream.read(result.data(), static_cast<std::streamsize>(result.size()));
		stream.close();

		return result;
	}

	std::string FileSystem::ReadFile(const std::string& filepath)
	{
		return ReadFile(filepath.c_str());
	}

	std::string FileSystem::ReadFile(const std::filesystem::path& filepath)
	{
		return ReadFile(filepath.string().c_str());
	}
} // namespace WandeltCore
