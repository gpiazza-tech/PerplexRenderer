#pragma once

#include <pxr/rendering/Renderer.h>
#include <pxr/rendering/Camera.h>

#include <pxr/backends/Framebuffer.h>
#include <pxr/backends/Shader.h>
#include <pxr/backends/TextureBuffer.h>
#include <pxr/backends/ScreenQuad.h>
#include <pxr/backends/RenderCommands.h>

#include <pxr/sprite/Sprite.h>
#include <pxr/sprite/SpriteAtlas.h>
#include <pxr/sprite/SpriteRegistry.h>

#include <pxr/util/Util.h> 
#include <pxr/util/Log.h>

#include <pxr/postprocessing/BloomRenderer.h>
#include <pxr/postprocessing/Tonemapper.h>
#include <pxr/postprocessing/Pixelator.h>

#include <pxr/particles/Particle.h>
#include <pxr/particles/ParticleSystemSettings.h>
#include <pxr/particles/SpriteParticleSystem.h>
#include <pxr/particles/BurstParticleSystem.h>