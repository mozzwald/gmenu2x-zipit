// Written by Maarten ter Huurne in 2011.
// Based on the libpng example.c source, with some additional inspiration
// from SDL_image and openMSX.
// License: GPL version 2 or later.


#include "imageio.h"

#include "debug.h"

#include <SDL.h>
#include <png.h>
#include <cassert>


SDL_Surface *cleanup(SDL_Surface *s,FILE *fp,png_structp *png, png_infop *info)
{
	// Clean up.
	png_destroy_read_struct(png, info, NULL);
	if (fp) fclose(fp);

	return s;
 }

SDL_Surface *loadPNG(const std::string &path) {
	// Declare these with function scope and initialize them to NULL,
	// so we can use a single cleanup block at the end of the function.
	SDL_Surface *surface = NULL;
	FILE *fp = NULL;
	png_structp png = NULL;
	png_infop info = NULL;

	// Create and initialize the top-level libpng struct.
	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) return cleanup(surface, fp, &png, &info);
	// Create and initialize the image information struct.
	info = png_create_info_struct(png);
	if (!info) return cleanup(surface, fp, &png, &info);
	// Setup error handling for errors detected by libpng.
	if (setjmp(png_jmpbuf(png))) {
		// Note: This gets executed when an error occurs.
		if (surface) {
			SDL_FreeSurface(surface);
			surface = NULL;
		}
		return cleanup(surface, fp, &png, &info);
	}

	// Open input file.
	fp = fopen(path.c_str(), "rb");
	if (!fp) return cleanup(surface, fp, &png, &info);
	// Set up the input control if you are using standard C streams.
	png_init_io(png, fp);

	// The call to png_read_info() gives us all of the information from the
	// PNG file before the first IDAT (image data chunk).
	png_read_info(png, info);
	png_uint_32 width, height;
	int bitDepth, colorType;
	png_get_IHDR(
		png, info, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);

	// Select ARGB pixel format:
	// (ARGB is the native pixel format for the JZ47xx frame buffer in 24bpp)
	// - strip 16 bit/color files down to 8 bits/color
	png_set_strip_16(png);
	// - convert 1/2/4 bpp to 8 bpp
	png_set_packing(png);
	// - expand paletted images to RGB
	// - expand grayscale images of less than 8-bit depth to 8-bit depth
	// - expand tRNS chunks to alpha channels
	png_set_expand(png);
	// - convert grayscale to RGB
	png_set_gray_to_rgb(png);
	// - add alpha channel
	png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
	// - convert RGBA to ARGB
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
		png_set_swap_alpha(png);
	} else {
		png_set_bgr(png); // BGRA in memory becomes ARGB in register
	}

	// Update the image info to the post-conversion state.
	png_read_update_info(png, info);
	png_get_IHDR(
		png, info, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);
	assert(bitDepth == 8);
	assert(colorType == PNG_COLOR_TYPE_RGB_ALPHA);

	// Refuse to load outrageously large images.
	if (width > 65536) {
		WARNING("Refusing to load image because it is too wide\n");
		return cleanup(surface, fp, &png, &info);
	}
	if (height > 2048) {
		WARNING("Refusing to load image because it is too high\n");
		return cleanup(surface, fp, &png, &info);
	}

	// Allocate ARGB surface to hold the image.
	surface = SDL_CreateRGBSurface(
		SDL_SWSURFACE | SDL_SRCALPHA, width, height, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
		);
	if (!surface) {
		// Failed to create surface, probably out of memory.
		return cleanup(surface, fp, &png, &info);
	}

	// Compute row pointers.
	png_bytep rowPointers[height];
	for (png_uint_32 y = 0; y < height; y++) {
		rowPointers[y] =
			static_cast<png_bytep>(surface->pixels) + y * surface->pitch;
	}

	// Read the entire image in one go.
	png_read_image(png, rowPointers);

	// Read rest of file, and get additional chunks in the info struct.
	// Note: We got all we need, so skip this step.
	//png_read_end(png, info);

cleanup:
	// Clean up.
	png_destroy_read_struct(&png, &info, NULL);
	if (fp) fclose(fp);

	return surface;
}
