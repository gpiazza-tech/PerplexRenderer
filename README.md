# PerplexRenderer

PerplexRenderer is a neo pixel art 2D renderer for Windows using OpenGL. I plan on adding support soon for Linux and eventually MacOS with Metal. 

![Rendered Logo](https://github.com/gpiazza-tech/PerplexRenderer/blob/master/res/screenshots/Logo_Pixel.png)
![Rendered Kablooey](https://github.com/gpiazza-tech/PerplexRenderer/blob/master/res/screenshots/Kablooey_Pixel.png)
![Rendered Particles](https://github.com/gpiazza-tech/PerplexRenderer/blob/master/res/screenshots/Particles.png)

## How to build

Windows
 * Ensure git and Visual Studio 2026 are installed
 * Navigate to your desired installation directory and run `git clone https://github.com/gpiazza-tech/PerplexRenderer`
 * Run scripts/Setup-Windows.bat to build as a static library or scripts/Setup-Examples-Windows.bat to build an example application. (Note: building as a static library will not link ImGui or GLFW, only Glew will be linked)
 * Open the generated .sln file in Visual Studio, and build the solution

## Getting started

### Setup

Once the project is built, add `PerplexRenderer/include` to your include paths and link the library.
PerplexRenderer also includes a math library called glm for general math functions and utilities.
You will also need a valid opengl graphics context and window, which can be handled with a third party library such as GLFW.

To include PerplexRenderer in your cpp file:
```c
#include <pxr/pxr.h>
```

Having created a graphics context and window, you can initialize the renderer with pxr::Renderer::Init(int), which takes in
the pixels per unit configuration of the project. This is a global setting that cannot be changed at runtime. Standard values 
include 8, 16, 32, and 64, but any positive integer will work. As a general rule of thumb, I like to base my pixels per unit 
value on the pixel width of the main character sprite. For this example I will use the value 16, which is popular for indie games.
```c
pxr::Renderer::Init(16);
```

Before we can render anything, we also need a Camera object. You can add a camera with
```c
pxr::Camera camera(screenSize, 16, 6.0f, pxr::ScalingMode::SmallerSide);
```
screenSize is a glm::vec2 which should be equal to the screen's width and height. It then takes a 
pixels per unit value, which should match the value used for initializing the renderer. The next two values are the zoom
level and the scaling mode. The zoom level determines the length in units of one of the sides of the camera based on the 
ScalingMode. In this case, for example, the scalling mode is set to SmallerSide, which means the smaller side of the camera
will have a length of 6 units, and because our pixels per unit value is 16, the smaller side will have a length of 16 * 6 pixels,
or 96 pixels.

The last thing we'll need is to set up some kind of render loop, which should be already handled by whatever windowing
library you are using. To keep the code nice and separated, I would create a seperate Render function for Perplex-only code. This function
should be called every frame immediately before swapping buffers. A super simple example for GLFW might look like this:
```c
... GLFW init code ...

while (!glfwWindowShouldClose(window))
{
    Render(); // all Perplex rendering code will be in here

    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

Let's also setup a rendering initialization function that runs once before the update loop called Start, so our code can look like this:

```c
pxr::Camera camera(screenSize, 16, 6.0f, pxr::ScalingMode::SmallerSide);

void Start()
{
	pxr::Renderer::Init(16);
}

void Render()
{
	\\ Perplex rendering code here
}
```

### Drawing pixels and sprites

We can now do some basic rendering. The steps to drawing anything in PerplexRenderer are
1. Start the frame
2. Begin a batch using our camera's projection
3. Draw the pixel with a specified position
4. End the batch
5. Flush

Here's a simple example of drawing multiple pixels onto the screen

```c
void Render()
{
    pxr::Renderer::BeginFrame();
	pxr::Renderer::BeginBatch(camera.GetProjection());

	pxr::Renderer::DrawPixel({ -0.0625f, 0.0625f });
	pxr::Renderer::DrawPixel({ 0.0625f, 0.0625f });
	pxr::Renderer::DrawPixel({ -0.125f, -0.0625f });
	pxr::Renderer::DrawPixel({ -0.0625f, -0.125f });
	pxr::Renderer::DrawPixel({ 0.0f, -0.125f });
	pxr::Renderer::DrawPixel({ 0.0625f, -0.125f });
	pxr::Renderer::DrawPixel({ 0.125f, -0.0625f });

	pxr::Renderer::EndBatch();
	pxr::Renderer::Flush();
}
```

We can specify a color when rendering pixels like so:

```c
void Render()
{
    pxr::Renderer::BeginFrame();
	pxr::Renderer::BeginBatch(camera.GetProjection());
	
	glm::vec4 color = { 0.7f, 0.8f, 0.3f, 1.0f };
	
	pxr::Renderer::DrawPixel({ -0.0625f, 0.0625f }, color);
	pxr::Renderer::DrawPixel({ 0.0625f, 0.0625f }, color);
	pxr::Renderer::DrawPixel({ -0.125f, -0.0625f }, color);
	pxr::Renderer::DrawPixel({ -0.0625f, -0.125f }, color);
	pxr::Renderer::DrawPixel({ 0.0f, -0.125f }, color);
	pxr::Renderer::DrawPixel({ 0.0625f, -0.125f }, color);
	pxr::Renderer::DrawPixel({ 0.125f, -0.0625f }, color);
	
	pxr::Renderer::EndBatch();
	pxr::Renderer::Flush();
}
```

Rendering a sprite is a similar process, only we need to add the file to the SpriteRegistry before rendering it.
The path specified in pxr::SpriteRegistry::GetSprite is relative to the root folder of PerplexRenderer.
The renderer must be initialized before using the SpriteRegistry

```c
pxr::Camera camera(screenSize, 16, 6.0f, pxr::ScalingMode::SmallerSide);

pxr::Sprite sprite;

void Start()
{
	pxr::Renderer::Init();

	sprite = pxr::SpriteRegistry::GetSprite("res/mysprite.png");
}

void Render()
{
    pxr::Renderer::BeginFrame();
	pxr::Renderer::BeginBatch(camera.GetProjection());
	
	pxr::DrawQuad({ 0.0f, 0.0f }, sprite);
	
	pxr::Renderer::EndBatch();
	pxr::Renderer::Flush();
}
```

And that's all for right now! For more detail on how to do things like postprocessing effects, 
check out the included examples in `PerplexRenderer/examples`. It's still very much a work in progress, and I plan on 
adding to this a lot as I continue developing it.