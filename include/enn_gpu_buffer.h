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
// File: enn_gpu_buffer.h
//

#pragma once

#include "enn_platform_headers.h"

namespace enn
{

class GpuBuffer : public AllocatedObject
{
public:
	enum Usage
	{
		HBU_STATIC = 1,
		HBU_DYNAMIC = 2
	};

	enum LockOptions
	{
		HBL_NORMAL,
		HBL_DISCARD,
		HBL_READ_ONLY,
		HBL_NO_OVERWRITE,
		HBL_WRITE_ONLY
	};

public:
	GpuBuffer(Usage usage, bool use_system_memory = false)
		: usage_(usage), use_system_memory_(use_system_memory), is_locked_(false) {}

	virtual ~GpuBuffer() {}

	virtual void release() { ENN_DELETE this; }
	virtual bool createBuff(size_t size, void* data) = 0;

	virtual void* lock(size_t offset, size_t length, LockOptions options);
	virtual void* lock(LockOptions options);
	virtual void unlock();
	
public:
	virtual size_t getSize() const { return size_in_bytes_; }
	virtual Usage getUsage() const { return usage_; }

protected:
	virtual void* lockImpl(size_t offset, size_t length, LockOptions options) = 0;
	virtual void unlockImpl() = 0;

public:
	static unsigned GetVertexSize(unsigned elementMask);
	static unsigned GetElementOffset(unsigned elementMask, VertexElement element);

protected:
	size_t			size_in_bytes_;
	Usage			usage_;
	bool			use_system_memory_;
	bool			is_locked_;

public:
	static const unsigned elementSize[];
	static const unsigned elementComponents[];
};

}