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
// File: enn_file.h
//

#pragma once


#include <enn_deserializer.h>
#include <enn_serializer.h>

namespace enn
{

enum FileMode
{
	FILE_READ = 0,
	FILE_WRITE,
	FILE_READWRITE
};

//////////////////////////////////////////////////////////////////////////
class File : public Deserializer, public Serializer
{
public:
	File();
	File(const enn::String& fileName, FileMode mode = FILE_READ);
	~File();

	/// Read bytes from the file. Return number of bytes actually read.
	virtual unsigned Read(void* dest, unsigned size);

	/// Set position from the beginning of the file.
	virtual unsigned Seek(unsigned position);

	/// Write bytes to the file. Return number of bytes actually written.
	virtual unsigned Write(const void* data, unsigned size);

	/// Open a filesystem file. Return true if successful.
	bool Open(const enn::String& fileName, FileMode mode = FILE_READ);

	/// Close the file.
	void Close();
	/// Flush any buffered output to the file.
	void Flush();

	/// Return the open mode.
	FileMode GetMode() const { return mode_; }
	/// Return whether is open.
	bool IsOpen() const { return handle_ != 0; }
	/// Return the file handle.
	void* GetHandle() const { return handle_; }

	const enn::String& GetName() const { return fileName_; }

protected:
	/// File name.
	enn::String fileName_;
	/// Open mode.
	FileMode mode_;
	/// File handle.
	void* handle_;
};

}