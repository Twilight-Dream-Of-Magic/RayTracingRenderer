#pragma once

#include <atomic>
#include <thread>
#include <vector>

#include "Common.hpp"
#include "Camera.hpp"
#include "Sphere.hpp"

using Color = glm::vec3;

class Renderer
{
public:
	Renderer(uint32_t width, uint32_t height);
	virtual ~Renderer() {}
	void Run();

private:
	Color RenderPixel(uint32_t x, uint32_t y);
	void RenderThread();

	uint32_t viewport_width = 800;
	uint32_t viewport_height = 600;

	//Viewing pixel each color, minifb: format 0xAARRGGBB (Alpha, Red, Green, Blue)
	std::vector<uint32_t> framebuffer;
	std::atomic<uint32_t> current_pixel_index = 0;

	Camera camera;
	Sphere* sphere;
};