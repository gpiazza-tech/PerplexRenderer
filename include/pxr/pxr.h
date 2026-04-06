#pragma once

#include <rendering/Renderer.h>
#include <rendering/Camera.h>

#include <backends/Framebuffer.h>
#include <backends/Shader.h>
#include <backends/TextureBuffer.h>
#include <backends/ScreenQuad.h>
#include <backends/RenderCommands.h>

#include <texture/Texture.h>
#include <texture/TextureAtlas.h>
#include <texture/TextureRegistry.h>

#include <util/Util.h> 

#include <postprocessing/BloomRenderer.h>
#include <postprocessing/Tonemapper.h>
#include <postprocessing/Pixelator.h>

#include <particles/Particle.h>
#include <particles/ParticleSystemSettings.h>
#include <particles/SpriteParticleSystem.h>
#include <particles/BurstParticleSystem.h>