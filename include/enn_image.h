//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2014/12/24
// File: enn_image.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_io_headers.h"

namespace enn
{

static const int COLOR_LUT_SIZE = 16;

/// Supported compressed image formats.
enum CompressedFormat
{
	CF_NONE = 0,
	CF_DXT1,
	CF_DXT3,
	CF_DXT5,
	CF_ETC1,
	CF_PVRTC_RGB_2BPP,
	CF_PVRTC_RGBA_2BPP,
	CF_PVRTC_RGB_4BPP,
	CF_PVRTC_RGBA_4BPP,
};

/// Compressed image mip level.
struct CompressedLevel
{
	/// Construct empty.
	CompressedLevel() :
		data_(0),
		width_(0),
		height_(0),
		depth_(0),
		blockSize_(0),
		dataSize_(0),
		rowSize_(0),
		rows_(0)
	{
	}

	/// Decompress to RGBA. The destination buffer required is width * height * 4 bytes. Return true if successful.
	bool Decompress(unsigned char* dest);

	/// Compressed image data.
	unsigned char* data_;
	/// Compression format.
	CompressedFormat format_;
	/// Width.
	int width_;
	/// Height.
	int height_;
	/// Depth.
	int depth_;
	/// Block size in bytes.
	unsigned blockSize_;
	/// Total data size in bytes.
	unsigned dataSize_;
	/// Row size in bytes.
	unsigned rowSize_;
	/// Number of rows.
	unsigned rows_;
};

class Image : public AllocatedObject
{
public:
	Image();
	virtual ~Image();

	virtual bool Load(Deserializer& source);

	bool SetSize(int width, int height, unsigned components);
	bool SetSize(int width, int height, int depth, unsigned components);
	void SetData(const unsigned char* pixelData);
	void SetPixel(int x, int y, const Color& color);
	void SetPixel(int x, int y, int z, const Color& color);
	bool LoadColorLUT(Deserializer& source);
	void FlipVertical();
	bool Resize(int width, int height);
	void Clear(const Color& color);
	bool SaveBMP(const String& fileName) const;
	bool SavePNG(const String& fileName) const;
	bool SaveTGA(const String& fileName) const;
	bool SaveJPG(const String& fileName, int quality) const;

	Color GetPixel(int x, int y) const;
	Color GetPixel(int x, int y, int z) const;
	Color GetPixelBilinear(float x, float y) const;
	Color GetPixelTrilinear(float x, float y, float z) const;
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	int GetDepth() const { return depth_; }
	unsigned GetComponents() const { return components_; }
	unsigned char* GetData() const { return data_; }
	bool IsCompressed() const { return compressedFormat_ != CF_NONE; }
	CompressedFormat GetCompressedFormat() const { return compressedFormat_; }
	unsigned GetNumCompressedLevels() const { return numCompressedLevels_; }
	Image* GetNextLevel() const;
	CompressedLevel GetCompressedLevel(unsigned index) const;

private:
	static unsigned char* GetImageData(Deserializer& source, int& width, int& height, unsigned& components);
	static void FreeImageData(unsigned char* pixelData);

	/// Width.
	int width_;
	/// Height.
	int height_;
	/// Depth.
	int depth_;
	/// Number of color components.
	unsigned components_;
	/// Number of compressed mip levels.
	unsigned numCompressedLevels_;
	/// Compressed format.
	CompressedFormat compressedFormat_;
	/// Pixel data.
	uchar* data_;
};

}