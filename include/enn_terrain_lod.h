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
// File: enn_terrain_lod.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_d3d11_render_device.h"
#include "enn_terrain.h"

namespace enn
{
	
class TerrainLod : public AllocatedObject
{
public:
	struct _IndexPool
	{
		int              size;
		uint16 *          indices;

		_IndexPool() : size(0), indices(0) {}
	};

	struct _IndexData
	{
		int					prim_count;
		TopologyType		prim_type;
		int					start;
		int					index_count;
		D3D11ResourceProxy*	index_buffer;

		_IndexData() : prim_count(0), prim_type(TT_TRIANGLE_FAN),
			start(0), index_count(0), index_buffer(0) {}
	};

	struct _Key
	{
		union
		{
			struct
			{
				unsigned char       level;
				unsigned char       north;
				unsigned char       south;
				unsigned char       west;
				unsigned char       east;
				unsigned char       unused[3];
			};

			int64 value;
		};

		_Key() : value(0) {}
		~_Key() {}

		_Key(const _Key & k) : value(k.value) {}

		_Key & operator = (const _Key & r)
		{
			value = r.value;
			return *this;
		}

		bool operator < (const _Key & r) const
		{
			return value < r.value;
		}

		bool operator <= (const _Key & r) const
		{
			return value <= r.value;
		}

		bool operator >(const _Key & r) const
		{
			return value > r.value;
		}

		bool operator >= (const _Key & r) const
		{
			return value >= r.value;
		}

		bool operator == (const _Key & r) const
		{
			return value == r.value;
		}

		bool operator != (const _Key & r) const
		{
			return value != r.value;
		}
	};

public:
	TerrainLod(int lodLevel);
	~TerrainLod();

	const _IndexData &  getIndexData(const _Key & k);

protected:
	void generateBodyIndex();
	void _generateBodyIndex(int level);
	void generateConecterIndex();

	void _generateConecterIndexNorth(int level, int conecter);
	void _generateConecterIndexSouth(int level, int conecter);
	void _generateConecterIndexWest(int level, int conecter);
	void _generateConecterIndexEast(int level, int conecter);

	bool invalidKey(_Key key);

protected:
	_IndexPool				body_index[Terrain::kMaxDetailLevel];
	_IndexPool				conecter_index_[Terrain::kMaxDetailLevel][Terrain::kMaxDetailLevel][4];
	Map<_Key, _IndexData>	index_buffer_map_;
	int						level_details_;
};

}