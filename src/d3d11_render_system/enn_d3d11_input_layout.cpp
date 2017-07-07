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
// File: enn_d3d11_input_layout.cpp

#include "enn_d3d11_input_layout.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

D3D11InputLayout::D3D11InputLayout()
: topology_type_(TT_TRIANGLE_LIST)
, index_stream_(0)
, force_num_vertices_(0xFFFFFFFF)
, force_num_indices_(0xFFFFFFFF)
, force_num_instances_(0xFFFFFFFF)
, start_vertex_location_(0)
, start_index_location_(0)
, base_vertex_location_(0)
, start_instance_location_(0)
, raw_(0), index_format_(EF_Unknown)
{

}

D3D11InputLayout::~D3D11InputLayout()
{
	ENN_SAFE_RELEASE2(raw_);
}

ID3D11InputLayout* D3D11InputLayout::getInputLayout(D3D11VertexShader* shader)
{
	if (raw_) return raw_;

	InputElemsType elems;
	elems.reserve(vertex_streams_.size());

	for (uint32 i = 0; i < getNumVertexStreams(); ++i)
	{
		InputElemsType stream_elems;
		D3D11RenderDevice::Mapping(stream_elems, i, getVertexStreamFormat(i), vertex_streams_[i].type, vertex_streams_[i].freq);
		elems.insert(elems.end(), stream_elems.begin(), stream_elems.end());
	}

	if (instance_stream_.stream)
	{
		InputElemsType stream_elems;
		D3D11RenderDevice::Mapping(stream_elems, getNumVertexStreams(), getInstanceStreamFormat(), instance_stream_.type, instance_stream_.freq);
		elems.insert(elems.end(), stream_elems.begin(), stream_elems.end());
	}

	raw_ = g_render_device_->createInputLayout(elems, shader);

	return raw_;
}

void D3D11InputLayout::bindVertexStream(D3D11VertexBuffer* vb, const VertexElementsType& vertex_elems, StreamType st, uint32 freq)
{
	uint32_t size = 0;
	for (size_t i = 0; i < vertex_elems.size(); ++i)
	{
		size += vertex_elems[i].element_size();
	}

	if (ST_GEOMETRY == st)
	{
		for (size_t i = 0; i < vertex_streams_.size(); ++i)
		{
			if (vertex_streams_[i].format == vertex_elems)
			{
				vertex_streams_[i].stream = vb;
				vertex_streams_[i].vertex_size = size;
				vertex_streams_[i].type = st;
				vertex_streams_[i].freq = freq;
				return;
			}
		}

		StreamUnit vs;
		vs.stream = vb;
		vs.format = vertex_elems;
		vs.vertex_size = size;
		vs.type = st;
		vs.freq = freq;
		vertex_streams_.push_back(vs);
	}
	else
	{
		instance_stream_.stream = vb;
		instance_stream_.format = vertex_elems;
		instance_stream_.vertex_size = size;
		instance_stream_.type = st;
		instance_stream_.freq = freq;
	}
}

void D3D11InputLayout::bindIndexStream(D3D11IndexBuffer* ib, ElementFormat format)
{
	index_stream_ = ib;
	index_format_ = format;
}

}