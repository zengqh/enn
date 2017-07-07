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
// Time: 2014/01/23
// File: enn_d3d11_gpu_buffer.h
//

#pragma once

#include "enn_d3d11_gpu_resource.h"

namespace enn
{

class RenderParameter;
//////////////////////////////////////////////////////////////////////////
class D3D11BufferDesc : public AllocatedObject
{
public:
	D3D11BufferDesc();
	virtual ~D3D11BufferDesc();

public:
	void setDefault();
	void setDefaultConstantBuffer(uint32 size, bool dynamic);
	void setDefaultVertexBuffer(uint32 size, bool dynamic);
	void setDefaultIndexBuffer(uint32 size, bool dynamic);

public:
	D3D11_BUFFER_DESC		buffer_desc_;
};


//////////////////////////////////////////////////////////////////////////
class D3D11GpuBuffer : public D3D11Resource
{
public:
	D3D11GpuBuffer() : buffer_(0) { ENN_CLEAR_BLOCK_ZERO(buffer_desc_); }
	virtual ~D3D11GpuBuffer() { ENN_SAFE_RELEASE2(buffer_); }

	const D3D11_BUFFER_DESC& getBufferDesc() const { return buffer_desc_; }
	void setBufferDesc(const D3D11_BUFFER_DESC& desc) { buffer_desc_ = desc; }

	virtual ID3D11Resource* getResource() const { return buffer_; }
	ID3D11Buffer* getBuffer() const { return buffer_; }

	uint32 getSize() const { return buffer_desc_.ByteWidth; }

protected:
	ID3D11Buffer*				buffer_;
	D3D11_BUFFER_DESC			buffer_desc_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11VertexBuffer : public D3D11GpuBuffer
{
public:
	D3D11VertexBuffer(ID3D11Buffer* buffer);
	~D3D11VertexBuffer();

	ResourceType getType() const { return RT_VERTEXBUFFER; }

	void setVertexSize(uint32 size) { size_ = size; }
	void setVertexCount(uint32 count) { count_ = count; }
	uint32 getVertexSize() const { return size_; }
	uint32 getVertexCount() const { return count_; }

protected:
	uint32			size_;
	uint32			count_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11IndexBuffer : public D3D11GpuBuffer
{
public:
	D3D11IndexBuffer(ID3D11Buffer* buffer);
	~D3D11IndexBuffer();

	void setIndexSize(uint32 size) { size_ = size; }
	void setIndexCount(uint32 count) { count_ = count; }

	uint32 getIndexSize() const { return size_; }
	uint32 getIndexCount() const { return count_; }

	virtual ResourceType getType() const { return RT_INDEXBUFFER; }

protected:
	uint32			size_;
	uint32			count_;
};

//////////////////////////////////////////////////////////////////////////
struct ConstantBufferMapping
{
	RenderParameter*			pParameter;
	size_t						offset;
};

//////////////////////////////////////////////////////////////////////////
class D3D11ConstanceBuffer : public D3D11GpuBuffer
{
public:
	D3D11ConstanceBuffer(ID3D11Buffer* buffer);
	~D3D11ConstanceBuffer();

	void upload(const void* data, size_t size);

	virtual ResourceType getType() const { return RT_CONSTANTBUFFER; }
};

}