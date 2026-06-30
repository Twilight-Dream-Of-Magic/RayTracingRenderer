#include <thread>
#include <random>

#include "Renderer.hpp"
#include "SceneObject.hpp"

#include <MiniFB.h>
#include <MiniFB_keylist.h>


Renderer::Renderer( uint32_t width, uint32_t height ) : viewport_width(width), viewport_height(height), framebuffer( viewport_width * viewport_height, MFB_RGB( 0, 0, 0 ) )
{
	this->current_pixel_index = 0;

	this->owner_scene = new Scene();

	Camera camera;
	camera.Initialize
	(
		Vector3DFloat(0.0f, 0.0f, 0.0f), // Camera position
		Vector3DFloat(0.0f, 0.0f, 1.0f), // target position
		Vector3DFloat(0.0f, 1.0f, 0.0f), // up vector
		glm::radians(60.0f), //FOV
		0.1f, //near plane
		1000.0f, //far plane
		width, height //viewport width * height
	);

	this->owner_scene->SetCamera(camera);
	
	SceneObject* owner_scene_object = this->owner_scene->CreateSceneObject(Vector3DFloat(0,0,5), Vector3DFloat(0,0,0), 2.0f);
	/* Rectangle */
	owner_scene_object->CreatePrimitive<Triangle>(Vector3DFloat(-1.0f,-1.0f,0), Vector3DFloat(1.0f,-1.0f,0), Vector3DFloat(1.0f,1.0f,0));
	owner_scene_object->CreatePrimitive<Triangle>(Vector3DFloat(-1.0f,-1.0f,0), Vector3DFloat(1.0f,1.0f,0), Vector3DFloat(-1.0f,1.0f,0));

	SceneObject* owner_scene_object2 = this->owner_scene->CreateSceneObject(Vector3DFloat(0,0,2), Vector3DFloat(0,0,0), 1.0f);
	/* Sphere */
	owner_scene_object2->CreatePrimitive<Sphere>(0.5f);

	//auto* disk = new Disk(Vector3DFloat(0.0f,-2.0f,5.0f), Vector3DFloat(glm::radians(90.0f),0,0), 1.0f);
	//auto* triangle = new Triangle(Vector3DFloat(-1.0f,0,0), Vector3DFloat(0,1.0f,0), Vector3DFloat(1.0f,0,0), MakeWorldTransform(Vector3DFloat(0,0,5.0f), Vector3DFloat(0,0,0), 1.0f));

	//this->geometry_set.push_back(disk);
	//this->geometry_set.push_back(triangle);
}

Renderer::~Renderer()
{
	if(this->owner_scene != nullptr)
	{
		delete this->owner_scene;
		this->owner_scene = nullptr;
	}

	this->framebuffer.assign(framebuffer.size(), 0);
}

void Renderer::SetScreenSamplePerPixel( uint32_t sample_par_pixel )
{
	this->sample_par_pixel = sample_par_pixel;
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

	stop_rendering = true;

	window = nullptr;
}

Color Renderer::RenderPixel( uint32_t x, uint32_t y )
{
	auto RenderSubPixel = [this](float x_float, float y_float) -> Color
	{
		Ray ray = this->owner_scene->GetCamera().GetRay(x_float ,y_float);

		Intersection intersect;

		Color color(0,0,0);

		if(this->owner_scene != nullptr && this->owner_scene->Intersect(ray, intersect))
		{
			//{-1,1} -> {0,1}
			color = intersect.normal * 0.5f + 0.5f;
		}

		return color;
	};

	/* Super-Sampling Anti-Aliasing */
	thread_local std::mt19937 prng(std::random_device{}());
	thread_local std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

	
	Color result_color(0.0f, 0.0f, 0.0f);
	for ( size_t i = 0; i < this->sample_par_pixel; i++ )
	{
		float offseted_x = x + distribution(prng);
		float offseted_y = y + distribution(prng);
		//Value Safe
		result_color += RenderSubPixel(offseted_x, offseted_y) / static_cast<float>(sample_par_pixel);
	}

	return result_color;
}

void Renderer::RenderThread() 
{
	//Compute buffer color with multi-thread
	while ( !this->stop_rendering )
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

		if(this->stop_rendering)
		{
			break;
		}

		this->framebuffer[y * viewport_width + x] = (red << 16) | (green << 8) | (blue);
	}
}
