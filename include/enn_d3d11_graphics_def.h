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
// Time: 2014/03/03
// File: enn_d3d11_graphics_def.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_math.h"
#include "enn_io_headers.h"
#include "enn_d3d11.h"
#include "enn_d3d11_element_format.h"

namespace enn
{

enum ResourceType
{
	RT_VERTEXBUFFER,
	RT_INDEXBUFFER,
	RT_CONSTANTBUFFER,
	RT_STRUCTUREDBUFFER,
	RT_BYTEADDRESSBUFFER,
	RT_INDIRECTARGSBUFFER,
	RT_TEXTURE1D,
	RT_TEXTURE2D,
	RT_TEXTURE3D
};

enum TextureUnitUsage
{
	TU_DIFFUSE,
	TU_NORMAL_MAP,
	TU_LIGHT_MAP,
	TU_SPECULAR_MAP,
	TU_EMISSIVE_MAP,
	TU_COUNT
};

enum VertexElementUsage
{
	ELEMENT_POSITION = 0,
	ELEMENT_NORMAL,
	ELEMENT_DIFFUSE,
	ELEMENT_SPECULAR,
	ELEMENT_BLENDWEIGHTS,
	ELEMENT_BLENDINDICES,
	ELEMENT_TEXCOORD,
	ELEMENT_TANGENT,
	ELEMENT_BINORMAL,
	MAX_VERTEX_ELEMENTS
};

enum TopologyType
{
	TT_POINT_LIST,
	TT_LINE_LIST,
	TT_TRIANGLE_LIST,
	TT_TRIANGLE_FAN,
	TT_TRIANGLE_STRIP
};

enum StreamType
{
	ST_GEOMETRY,
	ST_INSTANCE
};

enum CullMode
{
	CULL_NONE,
	CULL_CW,
	CULL_CCW,
	CM_NUM
};

enum IndexType
{
	IDX_16,
	IDX_32,
	IDX_NUM
};

struct VertexP3N3
{
	float x, y, z;
	float nx, ny, nz;
};

struct VertexP3N3U2
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct VertexP3U2
{
	float x, y, z;
	float u, v;
};

struct VertexP3U2U2
{
	float x, y, z;
	float u, v;
	float u1, v1;
};

}