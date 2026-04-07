#pragma once

namespace pxr
{
	class SpriteRegistry;

	struct Sprite
	{
		// The index of the atlas stored in the AtlasGroups vector in AssetRegistry.
		// Used to determine what texture unit index to sample from in the shader
		int TextureUnit;
		// The renderer ids of the color and emission texture atlases
		int ColorAtlas;
		int EmissionAtlas;

		// The position and width in pixels relative to its atlas
		int PixelX;
		int PixelY;
		int PixelWidth;
		int PixelHeight;

		// Determines what size quad to draw based on the pixel dimensions and the pixels per unit
		// For example, if the texture is 16x32 and the pixels per unit is 8, then
		// ScaleFactorX would be 2.0f and ScaleFactorY would be 4.0f
		// This is calculated once when adding the texture to its atlas to avoid excessive divisions in Renderer::RenderQuad()
		float ScaleFactorX;
		float ScaleFactorY;

		// Texture Coordinates relative to its atlas
		float Xmin;
		float Ymin;
		float Xmax;
		float Ymax;
	};
}