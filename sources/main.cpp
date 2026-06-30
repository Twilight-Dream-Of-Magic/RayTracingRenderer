#include <cstdint>
#include <cstdlib>
#include <format>
#include <print>
#include <string>

#include <Renderer.hpp>

void modern_print()
{
	const std::string message = std::format( "RayTracingRenderer blank window: {}x{}", 800, 600 );
	std::print( "{}\n", message );
	system( "pause" );
}

int main()
{
	modern_print();

	Renderer renderer(1920, 1080);
	renderer.SetScreenSamplePerPixel(100);
	renderer.Run();

	return 0;
}
