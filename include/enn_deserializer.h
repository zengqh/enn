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
// Time: 2014/02/17
// File: enn_deserializer.h
//

#pragma once

#include <enn_platform_headers.h>

namespace enn
{

/// Abstract stream for reading.
class Deserializer
{
public:
	/// Construct with zero size.
	Deserializer();
	/// Construct with defined size.
	Deserializer(unsigned size);
	/// Destruct.
	virtual ~Deserializer();

	/// Read bytes from the stream. Return number of bytes actually read.
	virtual unsigned Read(void* dest, unsigned size) = 0;
	/// Set position from the beginning of the stream.
	virtual unsigned Seek(unsigned position) = 0;
	/// Return current position.
	unsigned GetPosition() const { return position_; }
	/// Return size.
	unsigned GetSize() const { return size_; }
	/// Return whether the end of stream has been reached.
	bool IsEof() const { return position_ >= size_; }

	String ReadFileID();

	template<class T>
	unsigned ReadBin( T& t )
	{
		return Read(&t, sizeof(t));
	}

	template<class T>
	unsigned ReadBin( T* t, int n )
	{
		unsigned ret = 0;
		for ( int i = 0; i < n; ++i )
		{
			ret += ReadBin( *t );
			++t;
		}

		return ret;
	}

	unsigned ReadUInt()
	{
		unsigned ret;
		ReadBin(ret);

		return ret;
	}

	unsigned ReadInt()
	{
		int ret;
		ReadBin(ret);

		return ret;
	}

	/// Read a null-terminated string.
	enn::String ReadString();
	/// Read a text line.
	enn::String ReadLine();

	/// Read a len + null-terminated string.
	virtual enn::String ReadLenString();
	/// Read a len + null-terminated string, align 4.
	virtual enn::String ReadLenStringAlign4();

protected:
	/// Stream position.
	unsigned position_;
	/// Stream size.
	unsigned size_;
};


}