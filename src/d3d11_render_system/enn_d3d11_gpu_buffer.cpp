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
// Time: 2014/03/13
// File: enn_d3d11_gpu_buffer.cpp
//

#include "enn_d3d11_gpu_buffer.h"
#include "enn_render_parameter.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
D3D11BufferDesc::D3D11BufferDesc()
{
	setDefault();
}

D3D11BufferDesc::~D3D11BufferDesc()
{

}

void D3D11BufferDesc::setDefault()
{
	buffer_desc_.ByteWidth = 1;
	buffer_desc_.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc_.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc_.MiscFlags = 0;
	buffer_desc_.StructureByteStride = 0;
}

void D3D11BufferDesc::setDefaultConstantBuffer(uint32 size, bool dynamic)
{
	buffer_desc_.ByteWidth = size;
	buffer_desc_.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc_.MiscFlags = 0;
	buffer_desc_.StructureByteStride = 0;

	if (dynamic)
	{
		buffer_desc_.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		buffer_desc_.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc_.CPUAccessFlags = 0;
	}
}

void D3D11BufferDesc::setDefaultVertexBuffer(uint32 size, bool dynamic)
{
	buffer_desc_.ByteWidth = size;
	buffer_desc_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc_.MiscFlags = 0;
	buffer_desc_.StructureByteStride = 0;

	if (dynamic)
	{
		buffer_desc_.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		buffer_desc_.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc_.CPUAccessFlags = 0;
	}
}

void D3D11BufferDesc::setDefaultIndexBuffer(uint32 size, bool dynamic)
{
	buffer_desc_.ByteWidth = size;
	buffer_desc_.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffer_desc_.MiscFlags = 0;
	buffer_desc_.StructureByteStride = 0;

	if (dynamic)
	{
		buffer_desc_.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc_.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		buffer_desc_.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc_.CPUAccessFlags = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
D3D11VertexBuffer::D3D11VertexBuffer(ID3D11Buffer* buffer)
: size_(0), count_(0)
{
	buffer_ = buffer;
	buffer_->GetDesc(&buffer_desc_);
}

D3D11VertexBuffer::~D3D11VertexBuffer()
{

}

//////////////////////////////////////////////////////////////////////////
D3D11IndexBuffer::D3D11IndexBuffer(ID3D11Buffer* buffer)
: size_(0), count_(0)
{
	buffer_ = buffer;
	buffer_->GetDesc(&buffer_desc_);
}

D3D11IndexBuffer::~D3D11IndexBuffer()
{

}

//////////////////////////////////////////////////////////////////////////
D3D11ConstanceBuffer::D3D11ConstanceBuffer(ID3D11Buffer* buffer)
{
	buffer_ = buffer;
	buffer_->GetDesc(&buffer_desc_);
}

D3D11ConstanceBuffer::~D3D11ConstanceBuffer()
{

}

void D3D11ConstanceBuffer::upload(const void* data, size_t size)
{
	if (!buffer_)
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE resource = g_render_device_->mapResource(this, 0, D3D11_MAP_WRITE_DISCARD, 0);
	memcpy(resource.pData, data, size);
	g_render_device_->unMapResource(this, 0);
}

}
