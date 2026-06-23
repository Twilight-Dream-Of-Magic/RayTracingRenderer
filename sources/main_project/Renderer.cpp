#include "Renderer.hpp"

#include <MiniFB.h>
#include <MiniFB_keylist.h>


Renderer::Renderer( uint32_t width, uint32_t height ) : viewport_width(width), viewport_height(height), framebuffer( viewport_width * viewport_height, MFB_RGB( 0, 0, 0 ) )
{
	
}

void Renderer::Run()
{
	mfb_window* window = mfb_open_ex( "RayTracingRenderer", viewport_width, viewport_height, MFB_WF_RESIZABLE );
	if ( window == nullptr )
	{
		return;
	}

	std::vector<std::thread> RunningRenderThreads(std::thread::hardware_concurrency());

	for(auto& RunningRenderThread : RunningRenderThreads)
	{
		RunningRenderThread = std::thread(&Renderer::RenderThread, this);
		RunningRenderThread.detach();
	}

	mfb_set_target_fps( 60 );

	//Present
	while ( true )
	{
		const mfb_update_state state = mfb_update_ex( window, framebuffer.data(), viewport_width, viewport_height );
		if ( state != MFB_STATE_OK )
		{
			break;
		}

		const uint8_t* keys = mfb_get_key_buffer( window );
		if ( keys != nullptr && keys[ MFB_KB_KEY_ESCAPE ] )
		{
			break;
		}

		if ( !mfb_wait_sync( window ) )
		{
			break;
		}
	}

	mfb_close( window );

	this->framebuffer.assign(framebuffer.size(), 0);
	this->framebuffer.shrink_to_fit();

	window = nullptr;
}

Color Renderer::RenderPixel( uint32_t x, uint32_t y )
{
	Color color(0.0f, 0.0f, 0.0f);
	color.r = static_cast<float>(x) / viewport_width;
	color.g = static_cast<float>(y) / viewport_height;
	color.b = 0.0f;

	return color;
}

void Renderer::RenderThread() 
{
	//Compute buffer color with multi-thread
	while ( true )
	{
		uint32_t pixel_index = this->current_pixel_index.fetch_add(1);
		if(pixel_index >= this->viewport_width * this->viewport_height)
		{
			break;
		}

		uint32_t x = pixel_index % viewport_width;
		uint32_t y = pixel_index / viewport_width;

		Color color = Renderer::RenderPixel(x, y);

		uint32_t red = glm::clamp(static_cast<uint32_t>(color.r * 255.0f), 0u, 255u);
		uint32_t green = glm::clamp(static_cast<uint32_t>(color.g * 255.0f), 0u, 255u);
		uint32_t blue = glm::clamp(static_cast<uint32_t>(color.b * 255.0f), 0u, 255u);

		this->framebuffer[y * viewport_width + x] = (red << 16) | (green << 8) | (blue);
	}
}
