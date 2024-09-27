#include <iostream>

#include <Logger/Logger.hpp>

int main()
{
	const SW::LogSystemSpecification spec = {
	    .LogFileName              = "logs/wandelt.log",
	    .ConsoleSinkLoggerPattern = "%^[%T] [%n] [%l]: %v%$",
	    .FileSinkLoggerPattern    = "[%T] [%l] [%n] [%l]: %v",
	    .SystemLoggerName         = "COMPILER",
	    .AppLoggerName            = "APP",
	};

	SW::LogSystem::Initialize(spec);

	SYSTEM_DEBUG("Hello, World!");

	SW::LogSystem::Shutdown();

	return 0;
}
