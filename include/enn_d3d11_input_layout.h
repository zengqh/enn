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
// Time: 2014/06/01
// File: enn_d3d11_input_layout.h

// RenderLayout.hpp
// KlayGE 渲染流布局类 头文件
// Ver 3.6.0
// 版权所有(C) 龚敏敏, 2006-2007
// Homepage: http://www.klayge.org
//
// 3.6.0
// 去掉了TT_TriangleFan (2007.6.23)
//
// 3.2.0
// 初次建立 (2006.1.9)
//
// 修改记录
/////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "enn_d3d11_graphics_def.h"
#include "enn_d3d11_gpu_buffer.h"
#include "enn_d3d11_gpu_program.h"

namespace enn
{

struct VertexElement
{
	VertexElement()
	{
	}
	VertexElement(VertexElementUsage usage, uint8_t usage_index, ElementFormat format)
		: usage(usage), usage_index(usage_index), format(format)
	{
	}

	VertexElementUsage usage;
	int usage_index;

	ElementFormat format;

	uint16_t element_size() const
	{
		return NumFormatBytes(format);
	}

	friend bool operator==(VertexElement const & lhs, VertexElement const & rhs)
	{
			return (lhs.usage == rhs.usage)
				&& (lhs.usage_index == rhs.usage_index)
				&& (lhs.format == rhs.format);
	}
};

typedef vector<VertexElement>::type VertexElementsType;
typedef vector<D3D11_INPUT_ELEMENT_DESC>::type	InputElemsType;

//////////////////////////////////////////////////////////////////////////
class D3D11InputLayout : public AllocatedObject
{
	struct StreamUnit
	{
		D3D11GpuBuffer*				stream;
		VertexElementsType			format;
		uint32						vertex_size;
		StreamType					type;
		uint32						freq;

		StreamUnit()
		{
			stream = 0;
			vertex_size = 0;
			type = ST_GEOMETRY;
			freq = 1;
		}
	};


	typedef vector<StreamUnit>::type				StreamUnitList;

	//////////////////////////////////////////////////////////////////////////
	template <typename tuple_type, int N>
	struct Tuple2Vector
	{
		static VertexElementsType Do(tuple_type const & t)
		{
			VertexElementsType ret = Tuple2Vector<tuple_type, N - 1>::Do(t);
			ret.push_back(std::get<N - 1>(t));
			return ret;
		}
	};

	template <typename tuple_type>
	struct Tuple2Vector<tuple_type, 1>
	{
		static VertexElementsType Do(tuple_type const & t)
		{
			return VertexElementsType(1, std::get<0>(t));
		}
	};

	//////////////////////////////////////////////////////////////////////////

public:
	D3D11InputLayout();
	~D3D11InputLayout();

	ID3D11InputLayout* getInputLayout(D3D11VertexShader* shader);

	void setTopologyType(TopologyType type)
	{
		topology_type_ = type;
	}

	TopologyType getTopology() const
	{
		return topology_type_;
	}

	void setNumVertices(uint32 n)
	{
		force_num_vertices_ = n;
	}

	uint32 getNumVertices() const
	{
		uint32_t n;

		if (force_num_vertices_ == 0xFFFFFFFF)
		{
			n = vertex_streams_[0].stream->getSize() / vertex_streams_[0].vertex_size;
		}
		else
		{
			n = force_num_vertices_;
		}

		return n;
	}

	void setNumIndices(uint32_t n)
	{
		force_num_indices_ = n;
	}

	uint32_t getNumIndices() const
	{
		uint32_t n = 0;
		if (index_stream_)
		{
			if (0xFFFFFFFF == force_num_indices_)
			{
				n = index_stream_->getSize() / NumFormatBytes(index_format_);
			}
			else
			{
				n = force_num_indices_;
			}
		}

		return n;
	}

	void bindVertexStream(D3D11VertexBuffer* vb, const VertexElementsType& vertex_elems, StreamType st = ST_GEOMETRY, uint32 freq = 1);
	void bindIndexStream(D3D11IndexBuffer* ib, ElementFormat format);

	uint32 getNumVertexStreams() const
	{
		return vertex_streams_.size();
	}

	D3D11VertexBuffer* getVertexStream(uint32 idx) const
	{
		return static_cast<D3D11VertexBuffer*>(vertex_streams_[idx].stream);
	}

	void setVertexStream(uint32 idx, D3D11VertexBuffer* vb)
	{
		vertex_streams_[idx].stream = vb;
	}

	uint32 getVertexSize(uint32 idx) const
	{
		return vertex_streams_[idx].vertex_size;
	}

	const VertexElementsType& getVertexStreamFormat(uint32_t idx) const
	{
		return vertex_streams_[idx].format;
	}

	D3D11IndexBuffer* getIndexStream() const
	{
		return index_stream_;
	}

	ElementFormat getIndexStreamFormat() const
	{
		return index_format_;
	}

	const VertexElementsType& getInstanceStreamFormat() const
	{
		return instance_stream_.format;
	}

	D3D11VertexBuffer* getInstanceStream() const
	{
		return static_cast<D3D11VertexBuffer*>(instance_stream_.stream);
	}

	uint32 getInstanceSize() const
	{
		return instance_stream_.vertex_size;
	}

	void setInstanceStream(D3D11VertexBuffer* stream)
	{
		instance_stream_.stream = stream;
	}

	void setNumInstances(uint32 n)
	{
		force_num_instances_ = n;
	}

	uint32 getNumInstances() const
	{
		uint32_t n;
		if (0xFFFFFFFF == force_num_instances_)
		{
			if (!vertex_streams_.empty())
			{
				n = vertex_streams_[0].freq;
			}
			else
			{
				n = 1;
			}
		}
		else
		{
			n = force_num_instances_;
		}
		return n;
	}

	void setStartVertexLocation(uint32_t location)
	{
		start_vertex_location_ = location;
	}

	uint32_t getStartVertexLocation() const
	{
		return start_vertex_location_;
	}

	void setStartIndexLocation(uint32_t location)
	{
		start_index_location_ = location;
	}

	uint32_t getStartIndexLocation() const
	{
		return start_index_location_;
	}

	void setStartInstanceLocation(uint32_t location)
	{
		start_instance_location_ = location;
	}

	uint32_t getStartInstanceLocation() const
	{
		return start_instance_location_;
	}

public:

	template <typename tuple_type>
	static VertexElementsType makeVertexElements(const tuple_type& vertex_elems)
	{
		return Tuple2Vector<tuple_type, std::tuple_size<tuple_type>::value>::Do(vertex_elems);
	}

protected:
	TopologyType									topology_type_;
	StreamUnitList									vertex_streams_;
	StreamUnit										instance_stream_;
	D3D11IndexBuffer*								index_stream_;
	ElementFormat									index_format_;

	uint32_t										force_num_vertices_;
	uint32_t										force_num_indices_;
	uint32_t										force_num_instances_;

	uint32_t										start_vertex_location_;
	uint32_t										start_index_location_;
	int32_t											base_vertex_location_;
	uint32_t										start_instance_location_;

	ID3D11InputLayout*								raw_;
};

//////////////////////////////////////////////////////////////////////////
#define MASK_P3N3U2 (D3D11InputLayout::makeVertexElements(std::make_tuple( \
	VertexElement(ELEMENT_POSITION, 0, EF_BGR32F), \
	VertexElement(ELEMENT_NORMAL, 0, EF_BGR32F), \
	VertexElement(ELEMENT_TEXCOORD, 0, EF_GR32F))))

#define MASK_P3 (D3D11InputLayout::makeVertexElements(std::make_tuple( \
	VertexElement(ELEMENT_POSITION, 0, EF_BGR32F))))

#define MASK_P3N3 (D3D11InputLayout::makeVertexElements(std::make_tuple( \
	VertexElement(ELEMENT_POSITION, 0, EF_BGR32F), \
	VertexElement(ELEMENT_NORMAL, 0, EF_BGR32F))))

#define MASK_P3U2 (D3D11InputLayout::makeVertexElements(std::make_tuple( \
	VertexElement(ELEMENT_POSITION, 0, EF_BGR32F), \
	VertexElement(ELEMENT_TEXCOORD, 0, EF_GR32F))))

#define MASK_P2U2 (D3D11InputLayout::makeVertexElements(std::make_tuple( \
	VertexElement(ELEMENT_POSITION, 0, EF_GR32F), \
	VertexElement(ELEMENT_TEXCOORD, 0, EF_GR32F))))

#define MASK_P3U2U2 (D3D11InputLayout::makeVertexElements(std::make_tuple( \
	VertexElement(ELEMENT_POSITION, 0, EF_BGR32F), \
	VertexElement(ELEMENT_TEXCOORD, 0, EF_BGR32F), \
	VertexElement(ELEMENT_TEXCOORD, 1, EF_GR32F))))

#define VE_POSITION				VertexElement(ELEMENT_POSITION, 0, EF_BGR32F)
#define VE_NORMAL				VertexElement(ELEMENT_NORMAL, 0, EF_BGR32F)
#define VE_TANGENT				VertexElement(ELEMENT_TANGENT, 0, EF_ABGR32F)
#define VE_TEXCOORD1			VertexElement(ELEMENT_TEXCOORD, 0, EF_GR32F)
#define VE_TEXCOORD2			VertexElement(ELEMENT_TEXCOORD, 1, EF_GR32F)
#define VE_BLENDWEIGHTS			VertexElement(ELEMENT_BLENDWEIGHTS, 0, EF_ABGR32F)
#define VE_BLENDINDICES			VertexElement(ELEMENT_BLENDINDICES, 0, EF_ABGR8)
#define VE_POSITION2			VertexElement(ELEMENT_POSITION, 0, EF_GR32F)
#define VE_POSITION1			VertexElement(ELEMENT_TEXCOORD, 0, EF_R32F)
#define VE_COLOR4				VertexElement(ELEMENT_DIFFUSE, 0, EF_ARGB8)

#define MVE(x)					(D3D11InputLayout::makeVertexElements(std::make_tuple(x)))
#define MVE(x1, x2)				(D3D11InputLayout::makeVertexElements(std::make_tuple(x1, x2)))
#define MVE(x1, x2, x3)			(D3D11InputLayout::makeVertexElements(std::make_tuple(x1, x2, x3)))	
#define MVE(x1, x2, x3, x4)		(D3D11InputLayout::makeVertexElements(std::make_tuple(x1, x2, x3, x4)))	
#define MVE(x1, x2, x3, x4, x5)		(D3D11InputLayout::makeVertexElements(std::make_tuple(x1, x2, x3, x4, x5)))	

#define MASK_P2 (D3D11InputLayout::makeVertexElements(std::make_tuple(VE_POSITION2)))
#define MASK_N3 (D3D11InputLayout::makeVertexElements(std::make_tuple(VE_NORMAL)))
#define MASK_P1 (D3D11InputLayout::makeVertexElements(std::make_tuple(VE_POSITION1)))

// water vertex format
#define MASK_P3C4P2 (D3D11InputLayout::makeVertexElements(std::make_tuple( VE_POSITION, VE_POSITION2, VE_COLOR4)))

}