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
// Time: 2015/01/18
// File: enn_water.cpp
//

#include "enn_water.h"
#include "enn_terrain.h"

namespace enn
{

static int kBlockGridSize = Terrain::kSectionVertexSize - 1;

struct WaterIdentityVertex
{
	struct Vertex
	{
		int _x, _y;
		Vector3 position;
	};

	Array<Vertex> vertArray;

	int add(int _x, int _y, const Vector3& pos)
	{
		for (int i = 0; i < vertArray.size(); ++i)
		{
			if (vertArray[i]._x == _x && vertArray[i]._y == _y)
			{
				return i;
			}
		}

		Vertex v = { _x, _y, pos };

		vertArray.push_back(v);

		return vertArray.size() - 1;
	}
};

WaterBlockIn::WaterBlockIn()
{

}

WaterBlockIn::~WaterBlockIn()
{
	shutdown();
}

void WaterBlockIn::init(Water* water, int x, int z)
{
	shutdown();

	water_ = water;
	index_x_ = x;
	index_z_ = z;

	Terrain* terrain;

	int startX = kBlockGridSize * x;
	int startZ = kBlockGridSize * z;
	int sizeZ = water->GetSizeZ() - 1;

	WaterIdentityVertex wiv;
	Array<uint16> indexArray;

	for (int j = startZ; j <= startZ + kBlockGridSize; ++j)
	{
		for (int i = startX; i <= startX + kBlockGridSize; ++i)
		{
			char flag = water_->getData(i, j);

			if (flag == 1)
			{
				int _x = i;
				int _y = sizeZ - j;

				Vector3 p0 = terrain->getPosition(_x + 0, _y + 0);
				Vector3 p1 = terrain->getPosition(_x + 1, _y + 0);
				Vector3 p2 = terrain->getPosition(_x + 0, _y + 1);
				Vector3 p3 = terrain->getPosition(_x + 1, _y + 1);

				int i0 = wiv.add(_x + 0, _y + 0, p0);
				int i1 = wiv.add(_x + 1, _y + 0, p1);
				int i2 = wiv.add(_x + 0, _y + 1, p2);
				int i3 = wiv.add(_x + 1, _y + 1, p3);

				indexArray.push_back(i0);
				indexArray.push_back(i1);
				indexArray.push_back(i2);
				indexArray.push_back(i3);

				bound_.merge(p0);
				bound_.merge(p1);
				bound_.merge(p2);
				bound_.merge(p3);
			}
		}
	}

	if (indexArray.size() == 0)
	{
		return;
	}

	int iVertexCount = wiv.vertArray.size();
	int iIndexCount = (indexArray.size() - 2) * 3;
	vb_ = g_render_device_->createVertexBuffer(0, iVertexCount * sizeof(Vector3));
	ib_ = g_render_device_->createIndexBuffer(0, iIndexCount * sizeof(uint16));

	Vector3* vert = (Vector3*)g_render_device_->lock(vb_);
	{
		for (int i = 0; i < iVertexCount; ++i)
		{
			vert[i] = wiv.vertArray[i].position;


		}
	}
	g_render_device_->unLock(vb_);

	uint16* idx = (uint16*)g_render_device_->lock(ib_);
	{
		for (int i = 0; i < indexArray.size(); i += 4)
		{
			*idx++ = indexArray[i + 0];
			*idx++ = indexArray[i + 1];
			*idx++ = indexArray[i + 2];

			*idx++ = indexArray[i + 2];
			*idx++ = indexArray[i + 1];
			*idx++ = indexArray[i + 3];
		}
	}
	g_render_device_->unLock(ib_);
}

void WaterBlockIn::shutdown()
{

}

//////////////////////////////////////////////////////////////////////////
Water::Water()
{

}

Water::~Water()
{
	shutdown();
}

void Water::setHeight(float height)
{
	height_ = height;
}

void Water::init()
{
	shutdown();

	Terrain * terrain = 0;

	if (!terrain)
	{
		return;
	}

	size_x_ = terrain->getConfig().xVertexCount - 1;
	size_z_ = terrain->getConfig().zVertexCount - 1;

	grid_size_ = terrain->getConfig().xSize / size_x_;
	data_ = ENN_MALLOC_ARRAY_T(char, size_x_ * size_z_);

	allocBlock();

	initRT();
}

void Water::shutdown()
{

}

void Water::resize(int w, int h)
{

}

void Water::load(const char* source)
{
	MemoryBuffer* buffer = 0;

	g_file_system_->getFileData(source, buffer);

	if (!buffer)
	{
		ENN_ASSERT(0);
		return;
	}

	int magic, version;

	buffer->ReadBin(magic);
	buffer->ReadBin(version);

	ENN_ASSERT(magic == K_Magic);
	ENN_ASSERT(version == 0);

	if (version == 0)
	{
		int sizeX, sizeZ;

		buffer->ReadBin(sizeX);
		buffer->ReadBin(sizeZ);

		ENN_ASSERT(sizeX == size_x_ && sizeZ == size_z_);

		buffer->ReadBin(height_);
		buffer->Read(data_, sizeX * sizeZ);
	}

	initBlock();
}

void Water::save(const char* source)
{
	File file(source, FILE_WRITE);
	
	int K_Version = 0;

	file.WriteBin(K_Magic);
	file.WriteBin(K_Version);

	file.WriteBin(size_x_);
	file.WriteBin(size_z_);

	file.WriteBin(height_);
	file.Write(data_, size_x_ * size_z_);
}

char Water::getData(int i, int j)
{
	ENN_ASSERT(i < size_x_ && j < size_z_ && data_);
	return data_[j * size_x_ + i];
}

void Water::setData(int i, int j, char v)
{
	ENN_ASSERT(i < size_x_ && j < size_z_ && data_);
	data_[j * size_x_ + i] = v;
}

void Water::mapCoord(int& _x, int& _z, float x, float z)
{
	_x = x / grid_size_;
	_z = z / grid_size_;
}

bool Water::hasWater(float x, float z)
{
	int i = int((x / grid_size_));
	int j = int((z / grid_size_));

	if (i < 0 || i >= size_x_ ||
		j < 0 || j >= size_z_)
	{
		return false;
	}

	i /= kBlockGridSize;
	j /= kBlockGridSize;


	WaterBlockIn* block = &blocks_[j * block_count_x_ + i];

}

void Water::notifyChange(const IntRect& rc)
{

}

void Water::render()
{

}

void Water::allocBlock()
{
	Terrain* terrain;

	block_count_x_ = terrain->getConfig().xSectionCount;
	block_count_z_ = terrain->getConfig().zSectionCount;

	blocks_ = ENN_NEW WaterBlockIn[terrain->getConfig().iSectionCount];

	initBlock();
}

void Water::initBlock()
{
	int index = 0;
	for (int z = 0; z < block_count_z_; ++z)
	{
		for (int x = 0; x < block_count_x_; ++x)
		{
			blocks_[index++].init(this, x, z);
		}
	}
}

void Water::initRT()
{

}

void Water::preRender()
{

}

void Water::renderUnderWater()
{

}

void Water::renderUpWater()
{

}

void Water::renderReflection()
{

}

}