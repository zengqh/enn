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
// Time: 2015/01/05
// File: enn_terrain_lod.cpp
//

#include "enn_terrain_lod.h"

#define __North         (1 << 0)
#define __South         (1 << 1)
#define __East          (1 << 2)
#define __West          (1 << 3)

#define __NorthIndex    0
#define __SouthIndex    1
#define __EastIndex     2
#define __WestIndex     3

namespace enn
{

TerrainLod::TerrainLod(int lodLevel)
{
	level_details_ = lodLevel;

	memset(body_index, 0, Terrain::kMaxDetailLevel * sizeof(_IndexPool));
	memset(conecter_index_, 0, Terrain::kMaxDetailLevel * Terrain::kMaxDetailLevel * 4 * sizeof(_IndexPool));

	generateBodyIndex();
	generateConecterIndex();
}

TerrainLod::~TerrainLod()
{
	for (int i = 0; i < Terrain::kMaxDetailLevel; ++i)
	{
		ENN_SAFE_FREE(body_index[i].indices);

		for (int j = 0; j < Terrain::kMaxDetailLevel; ++j)
		{
			ENN_SAFE_FREE(conecter_index_[i][j][__NorthIndex].indices);
			ENN_SAFE_FREE(conecter_index_[i][j][__SouthIndex].indices);
			ENN_SAFE_FREE(conecter_index_[i][j][__WestIndex].indices);
			ENN_SAFE_FREE(conecter_index_[i][j][__EastIndex].indices);
		}
	}
}

const TerrainLod::_IndexData &  TerrainLod::getIndexData(const _Key & k)
{
	ENN_ASSERT(!invalidKey(k));

	Map<_Key, _IndexData>::iterator it = index_buffer_map_.find(k);

	if (it == index_buffer_map_.end())
	{
		_IndexData data;

		data.start = 0;
		data.index_count = 0;
		data.prim_count = 0;
		data.prim_type = TT_TRIANGLE_STRIP;

		const _IndexPool& body = body_index[k.level];

		const _IndexPool& north = conecter_index_[k.level][k.north][__NorthIndex];
		const _IndexPool& south = conecter_index_[k.level][k.south][__SouthIndex];
		const _IndexPool& west = conecter_index_[k.level][k.west][__WestIndex];
		const _IndexPool& east = conecter_index_[k.level][k.east][__EastIndex];

		if (body.indices)
		{
			data.index_count += body.size +2;
		}

		
		if (north.indices)
		{
			data.index_count += north.size + 2;
		}
		

		
		if (south.indices)
		{
			data.index_count += south.size + 2;
		}
		

		
		if (west.indices)
		{
			data.index_count += west.size + 2;
		}

		if (east.indices)
		{
			data.index_count += east.size;
		}
		

		ENN_ASSERT(data.index_count > 0 && data.index_count < 65536);

		uint16* index_data = ENN_MALLOC_ARRAY_T(uint16, data.index_count);
		int index = 0;
		bool flag = false;

		if (body.indices)
		{
			memcpy(index_data + index, body.indices, body.size * sizeof(uint16));
			index += body.size;
			flag = true;
		}

		
		if (north.indices)
		{
			if (flag)
			{
				index_data[index++] = index_data[index - 1];
				index_data[index++] = north.indices[0];
			}

			memcpy(index_data + index, north.indices, north.size * sizeof(uint16));
			index += north.size;
			flag = true;
		}
		

		
		if (south.indices)
		{
			if (flag)
			{
				index_data[index++] = index_data[index - 1];
				index_data[index++] = south.indices[0];
			}

			memcpy(index_data + index, south.indices, south.size * sizeof(uint16));
			index += south.size;
			flag = true;
		}
		

		
		if (west.indices)
		{
			if (flag)
			{
				index_data[index++] = index_data[index - 1];
				index_data[index++] = west.indices[0];
			}

			memcpy(index_data + index, west.indices, west.size * sizeof(uint16));
			index += west.size;
			flag = true;
		}

		if (east.indices)
		{
			if (flag)
			{
				index_data[index++] = index_data[index - 1];
				index_data[index++] = east.indices[0];
			}

			memcpy(index_data + index, east.indices, east.size * sizeof(short));
			index += east.size;
			flag = true;
		}
		

		D3D11BufferDesc ib_desc;

		ib_desc.setDefaultIndexBuffer(data.index_count * sizeof(uint16), false);
		D3D11_SUBRESOURCE_DATA sub_res_data;

		sub_res_data.pSysMem = index_data;
		sub_res_data.SysMemPitch = 0;
		sub_res_data.SysMemSlicePitch = 0;

		data.index_buffer = g_render_device_->createIndexBuffer(ib_desc, &sub_res_data);
		data.prim_count = index - 2;
		it = index_buffer_map_.insert(Map<_Key, _IndexData>::value_type(k, data)).first;

		ENN_SAFE_FREE(index_data);
	}

	return it->second;
}

void TerrainLod::generateBodyIndex()
{
	for (int i = 0; i < level_details_; ++i)
	{
		_generateBodyIndex(i);
	}
}

void TerrainLod::_generateBodyIndex(int level)
{
	ENN_ASSERT(body_index[level].size == 0 && body_index[level].indices == 0);

	int xtile = (Terrain::kSectionVertexSize - 1) >> level;
	int ytile = (Terrain::kSectionVertexSize - 1) >> level;
	int step = 1 << level;

	int start = 0;
	if (level < level_details_ - 1)
	{
		xtile -= 2;
		ytile -= 2;
		start = step * Terrain::kSectionVertexSize + step;
	}

	if (xtile == 0 || ytile == 0)
		return;

	int count = (xtile * 2 + 2 + 2) * ytile;
	body_index[level].indices = ENN_MALLOC_ARRAY_T(uint16, count);

	int& index = body_index[level].size;
	uint16* indices = body_index[level].indices;

	int x = 0, y = 0;
	short row_c = start;
	short row_n = row_c + Terrain::kSectionVertexSize * step;

	for (int y = 0; y < ytile; ++y)
	{
		for (int x = 0; x < xtile + 1; ++x)
		{
			indices[index++] = row_n + x * step;
			indices[index++] = row_c + x * step;
		}

		row_c += Terrain::kSectionVertexSize * step;
		row_n += Terrain::kSectionVertexSize * step;

		// degenerate triangle
		indices[index++] = indices[index - 1];
		indices[index++] = row_n;
	}

	ENN_ASSERT(index == count);
}

void TerrainLod::generateConecterIndex()
{
	for (int i = 0; i < level_details_; ++i)
	{
		for (int j = 0; j < level_details_; ++j)
		{
			_generateConecterIndexNorth(i, j);
			_generateConecterIndexSouth(i, j);
			_generateConecterIndexWest(i, j);
			_generateConecterIndexEast(i, j);
		}
	}
}

void TerrainLod::_generateConecterIndexNorth(int level, int conecter)
{
	ENN_ASSERT(conecter_index_[level][conecter][__NorthIndex].size == 0 &&
		conecter_index_[level][conecter][__NorthIndex].indices == 0);

	if (conecter < level || level == level_details_ - 1)
	{
		conecter_index_[level][conecter][__NorthIndex].size = 0;
		conecter_index_[level][conecter][__NorthIndex].indices = 0;
		return;
	}

	int self_step = 1 << level;
	int neighbor_step = 1 << conecter;
	int self_tile = (Terrain::kSectionVertexSize - 1) >> level;
	int neighbor_tile = (Terrain::kSectionVertexSize - 1) >> conecter;

	ENN_ASSERT(self_tile >= neighbor_tile);

	int count = self_tile * 2 + 2;
	conecter_index_[level][conecter][__NorthIndex].indices = ENN_MALLOC_ARRAY_T(uint16, count);

	int& index = conecter_index_[level][conecter][__NorthIndex].size;
	uint16* indices = conecter_index_[level][conecter][__NorthIndex].indices;

	indices[index++] = 0;
	indices[index++] = 0;

	for (int i = 1; i < self_tile; ++i)
	{
		int x1 = i * self_step;
		int y1 = self_step;
		int x0 = x1 / neighbor_step * neighbor_step;
		int y0 = y1 - self_step;

		int index0 = y1 * Terrain::kSectionVertexSize + x1;
		int index1 = y0 * Terrain::kSectionVertexSize + x0;

		indices[index++] = index0;
		indices[index++] = index1;
	}

	// ender
	indices[index++] = Terrain::kSectionVertexSize - 1;
	indices[index++] = Terrain::kSectionVertexSize - 1;

	ENN_ASSERT(index == count);
}

void TerrainLod::_generateConecterIndexSouth(int level, int conecter)
{
	ENN_ASSERT(conecter_index_[level][conecter][__SouthIndex].size == 0 &&
		conecter_index_[level][conecter][__SouthIndex].indices == 0);

	if (conecter < level || level == level_details_ - 1)
	{
		conecter_index_[level][conecter][__SouthIndex].size = 0;
		conecter_index_[level][conecter][__SouthIndex].indices = 0;
		return;
	}

	int self_step = 1 << level;
	int neighbor_step = 1 << conecter;
	int self_tile = (Terrain::kSectionVertexSize - 1) >> level;
	int neighbor_tile = (Terrain::kSectionVertexSize - 1) >> conecter;

	ENN_ASSERT(self_tile >= neighbor_tile);

	int count = self_tile * 2 + 2;
	conecter_index_[level][conecter][__SouthIndex].indices = ENN_MALLOC_ARRAY_T(uint16, count);

	int& index = conecter_index_[level][conecter][__SouthIndex].size;
	uint16* indices = conecter_index_[level][conecter][__SouthIndex].indices;

	indices[index++] = (Terrain::kSectionVertexSize - 1) * Terrain::kSectionVertexSize;
	indices[index++] = (Terrain::kSectionVertexSize - 1) * Terrain::kSectionVertexSize;

	// middler
	for (int i = 1; i < self_tile; ++i)
	{
		int x0 = i * self_step;
		int y0 = Terrain::kSectionVertexSize - 1 - self_step;
		int x1 = x0 / neighbor_step * neighbor_step;
		int y1 = y0 + self_step;

		int index0 = y1 * Terrain::kSectionVertexSize + x1;
		int index1 = y0 * Terrain::kSectionVertexSize + x0;

		indices[index++] = index0;
		indices[index++] = index1;
	}

	//ender
	indices[index++] = Terrain::kSectionVertexSize * Terrain::kSectionVertexSize - 1;
	indices[index++] = Terrain::kSectionVertexSize * Terrain::kSectionVertexSize - 1;

	ENN_ASSERT(index == count);
}

void TerrainLod::_generateConecterIndexWest(int level, int conecter)
{
	ENN_ASSERT(conecter_index_[level][conecter][__WestIndex].size == 0 &&
		conecter_index_[level][conecter][__WestIndex].indices == 0);

	if (conecter < level || level == level_details_ - 1)
	{
		conecter_index_[level][conecter][__WestIndex].size = 0;
		conecter_index_[level][conecter][__WestIndex].indices = 0;
		return;
	}

	int self_step = 1 << level;
	int neighbor_step = 1 << conecter;
	int self_tile = (Terrain::kSectionVertexSize - 1) >> level;
	int neighbor_tile = (Terrain::kSectionVertexSize - 1) >> conecter;

	ENN_ASSERT(self_tile >= neighbor_tile);

	int count = self_tile * 2 + 2;
	conecter_index_[level][conecter][__WestIndex].indices = ENN_MALLOC_ARRAY_T(uint16, count);

	int& index = conecter_index_[level][conecter][__WestIndex].size;
	uint16* indices = conecter_index_[level][conecter][__WestIndex].indices;

	indices[index++] = 0;
	indices[index++] = 0;

	for (int i = 1; i < self_tile; ++i)
	{
		int x0 = 0;
		int y0 = i * self_step / neighbor_step * neighbor_step;
		int x1 = self_step;
		int y1 = i * self_step;

		int index0 = y0 * Terrain::kSectionVertexSize + x0;
		int index1 = y1 * Terrain::kSectionVertexSize + x1;

		indices[index++] = index0;
		indices[index++] = index1;
	}

	//ender
	indices[index++] = (Terrain::kSectionVertexSize - 1) * Terrain::kSectionVertexSize;
	indices[index++] = (Terrain::kSectionVertexSize - 1) * Terrain::kSectionVertexSize;

	assert(index == count);
}

void TerrainLod::_generateConecterIndexEast(int level, int conecter)
{
	ENN_ASSERT(conecter_index_[level][conecter][__EastIndex].size == 0 &&
		conecter_index_[level][conecter][__EastIndex].indices == 0);

	if (conecter < level || level == level_details_ - 1)
	{
		conecter_index_[level][conecter][__EastIndex].size = 0;
		conecter_index_[level][conecter][__EastIndex].indices = 0;
		return;
	}

	int self_step = 1 << level;
	int neighbor_step = 1 << conecter;
	int self_tile = (Terrain::kSectionVertexSize - 1) >> level;
	int neighbor_tile = (Terrain::kSectionVertexSize - 1) >> conecter;

	ENN_ASSERT(self_tile >= neighbor_tile);

	int count = self_tile * 2 + 2;
	conecter_index_[level][conecter][__EastIndex].indices = ENN_MALLOC_ARRAY_T(uint16, count);

	int& index = conecter_index_[level][conecter][__EastIndex].size;
	uint16* indices = conecter_index_[level][conecter][__EastIndex].indices;

	indices[index++] = Terrain::kSectionVertexSize - 1;
	indices[index++] = Terrain::kSectionVertexSize - 1;

	for (int i = 1; i < self_tile; ++i)
	{
		int x0 = Terrain::kSectionVertexSize - 1 - self_step;
		int y0 = i * self_step;
		int x1 = Terrain::kSectionVertexSize - 1;
		int y1 = i * self_step / neighbor_step * neighbor_step;

		int index0 = y0 * Terrain::kSectionVertexSize + x0;
		int index1 = y1 * Terrain::kSectionVertexSize + x1;

		indices[index++] = index0;
		indices[index++] = index1;
	}

	//ender
	indices[index++] = Terrain::kSectionVertexSize * Terrain::kSectionVertexSize - 1;
	indices[index++] = Terrain::kSectionVertexSize * Terrain::kSectionVertexSize - 1;

	ENN_ASSERT(index == count);
}

bool TerrainLod::invalidKey(_Key key)
{
	return key.level >= Terrain::kMaxDetailLevel ||
		key.east >= Terrain::kMaxDetailLevel ||
		key.south >= Terrain::kMaxDetailLevel ||
		key.west >= Terrain::kMaxDetailLevel ||
		key.north >= Terrain::kMaxDetailLevel;
}

}