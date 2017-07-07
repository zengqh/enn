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
// File: enn_file_system.h
//

#pragma once

#include "enn_file.h"
#include "enn_memory_buffer.h"
#include "enn_msg_queue.h"
#include "enn_msg_id.h"

namespace enn
{

enum FileSystemGetResult
{
	FS_GR_OK,
	FS_GR_FAILED
};

struct IFileSystemListener
{
	virtual void onGetFileResult( const String& fileName, void* userData, MemoryBuffer* buff, FileSystemGetResult result ) = 0;
};

class FileSystem : public IMsgQueueListener, public Noncopyable, public AllocatedObject
{
public:
	FileSystem();
	virtual ~FileSystem();

	void setRootPath(const String& path)
	{
		root_path_ = path;
	}

	const String& getRootPath() const
	{
		return root_path_;
	}

	void setQueueManager(MsgQueueManager* msg_queue_mgr)
	{
		msg_queue_mgr_ = msg_queue_mgr;

		msg_queue_mgr_->registerMsg(MsgQueueManager::WORK_QUEUE, MQMSG_DEFAULT_FILE_SYSTEM, this);
	}

	void setFileListener(IFileSystemListener* listener)
	{
		file_listener_ = listener;
	}

	bool getFileData( const String& fileName, MemoryBuffer*& buff );
	void getAsyncFileData( const String& fileName, void* userData );
	void freeFileData(MemoryBuffer* buff);

	virtual void onWorkImpl( Message* msg );

protected:
	String					root_path_;
	MsgQueueManager*		msg_queue_mgr_;
	IFileSystemListener*	file_listener_;
};

extern FileSystem*		g_file_system_;

bool shader_getFileData(const String& file_name, String& data);

}