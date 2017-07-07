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
// File: enn_resource.cpp
//

#include "enn_resource.h"
#include "enn_resource_manager.h"

namespace enn
{

Resource::~Resource()
{
	ENN_SAFE_RELEASE(res_creator_);
}

void Resource::setResCreator(IResourceCreator* creator)
{
	ENN_SAFE_RELEASE(res_creator_);
	res_creator_ = creator; 
}

void Resource::setNullCreator()
{
	ResourceNullCreator* s_creator = ENN_NEW ResourceNullCreator;
	setResCreator(s_creator);
}

void Resource::addListener(IResourceListener* listener)
{
	if (!listener)
	{
		ENN_ASSERT(0);
		return;
	}

	if (listeners_.insert(listener).second)
	{
		if (status_ == S_LOADED)
		{
			listener->onResourceLoaded(this);
		}
	}
}

void Resource::removeListener( IResourceListener* listener )
{
	if (!listener)
	{
		ENN_ASSERT(0);
		return;
	}

	if (listeners_.empty())
	{
		return;
	}

	if (listeners_.find(listener) != listeners_.end())
	{
		listeners_.erase( listener );
	}
}

void Resource::setLoadAsync( bool b )
{
	load_async_ = b;
}

void Resource::load(bool async)
{
	setLoadAsync(async);
	manager_->load(this, async);
}

void Resource::unload()
{
	manager_->unload(this, false);
}

bool Resource::checkLoad()
{
	// 如果加载已经成功直接返回
	if ( status_ == S_LOADED ) return true;

	manager_->checkLoad(this);
	return false;
}

void Resource::update()
{
	// 通知资源有更新
	ENN_ASSERT(status_ == S_LOADED);
	for ( ListenerList::iterator it = listeners_.begin(), ite = listeners_.end(); it != ite; ++it )	
	{
		(*it)->onResourceUpdate(this);
	}
}

bool Resource::setLoadingStatus()
{
	//
	// 设置加载中状态
	// unloaded => loading
	if ( status_ == S_UNLOADED )
	{
		status_ = S_LOADING;
		return true;
	}

	return false;
}

void Resource::setPreparedStatus( bool ok )
{
	//
	// 设置当前状态为准备
	// loading => prepared ok
	// loading => prepared failed
	if ( status_ == S_LOADING )
	{
		status_ = ok ? S_PREPARED_OK : S_PREPARED_FAILED;
	}
}

void Resource::setCompletedStatus( bool ok )
{
	//
	// 设置最后完成状态
	// prepared ok => loaded
	// prepared ok => badres
	// prepared failed => badres
	if ( status_ != S_PREPARED_OK && status_ != S_PREPARED_FAILED )
	{
		return;
	}

	// loaded唯一条件是prepare ok且最后也ok
	State finalState = S_BADRES;
	if ( (status_ == S_PREPARED_OK) && ok )
	{
		finalState = S_LOADED;
	}

	// 设置最后的状态
	status_ = finalState;

	if ( finalState == S_LOADED )
	{
		for ( ListenerList::iterator it = listeners_.begin(), ite = listeners_.end(); it != ite; ++it )
			(*it)->onResourceLoaded( this );
	}
	else
	{
		for ( ListenerList::iterator it = listeners_.begin(), ite = listeners_.end(); it != ite; ++it )
			(*it)->onResourceBad( this );
	}
}


bool Resource::setUnloadingStatus()
{	
	//
	// 设置卸载中状态
	// loaded => unloading
	// badres => unloading
	if ( status_ == S_LOADED || status_ == S_BADRES )
	{
		status_ = S_UNLOADING;
		return true;
	}

	return false;
}

void Resource::setUnloadedStatus()
{
	if ( status_ == S_UNLOADING )
	{
		status_ = S_UNLOADED;
	}
}

void Resource::release()
{
	--ref_count_;

	if (ref_count_ == 1 )
	{
		manager_->releaseRes(this);
	}
}

void Resource::setUserData(void* user_data)
{
	user_data_ = user_data;
}

void* Resource::getUserData() const
{
	return user_data_;
}

}