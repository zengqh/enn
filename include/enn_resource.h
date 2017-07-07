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
// File: enn_resource.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_msg_id.h"
#include "enn_file_system.h"
#include "enn_res_ptr.h"
#include "enn_math.h"

namespace enn
{

class ResourceManager;
class Resource;

//////////////////////////////////////////////////////////////////////////
struct IResourceCreator
{
	virtual void release() = 0;
	virtual bool prepareResource( Resource* res, void* context ) = 0;
	virtual bool buildResource( Resource* res ) = 0;
};

struct IResourceListener
{
	virtual void onResourceLoaded( Resource* res ) {}
	virtual void onResourceBad( Resource* res ) {}
	virtual void onResourceUpdate( Resource* res ) {}
};

class ResourceNullCreator : public AllocatedObject, public IResourceCreator
{
public:
	virtual void release() { ENN_DELETE this; }
	virtual bool prepareResource( Resource* res, void* context ) { return true; }
	virtual bool buildResource( Resource* res ){ return true; }
};

//////////////////////////////////////////////////////////////////////////
class Resource : public Noncopyable, public AllocatedObject
{
	ENN_DECLARE_RTTI(Resource)

public:
	enum State
	{
		S_UNLOADED,
		S_LOADING,
		S_PREPARED_OK,
		S_PREPARED_FAILED,
		S_LOADED,
		S_BADRES,
		S_UNLOADING
	};

protected:
	typedef enn::set<IResourceListener*>::type ListenerList;

public:
	Resource() : manager_(0), res_creator_(0), 
		ref_count_(1), load_async_(false), status_(S_UNLOADED)
		, res_load_priority_(0)
	{

	}

	virtual ~Resource();

public:
	void setName(const String& name)
	{
		name_ = name;
	}

	const String& getName() const
	{
		return name_;
	}

	void setLoadPriority(uint32 pri)
	{
		res_load_priority_ = pri;
	}

	uint32 getLoadPriority() const
	{
		return res_load_priority_;
	}

	void setManager(ResourceManager* rm)
	{
		manager_ = rm;
	}

	ResourceManager* getManager() const
	{
		return manager_;
	}

	void setResCreator(IResourceCreator* creator);

	void setNullCreator();

	IResourceCreator* getResCreator() const
	{
		return res_creator_;
	}

	bool getLoadAsync() const
	{
		return load_async_;
	}

	void load(bool async);
	void unload();
	bool checkLoad();
	void update();

public:
	bool setLoadingStatus();
	void setPreparedStatus( bool ok );
	void setCompletedStatus( bool ok );

	bool setUnloadingStatus();
	void setUnloadedStatus();

	uint32 getStatus() const 
	{ 
		return status_; 
	}

	bool  isLoaded() const 
	{ 
		return status_ == S_LOADED; 
	}

public:
	void addRef() 
	{
		++ref_count_; 
	}

	void release();

	uint getRefCount() const 
	{ 
		return ref_count_; 
	}

public:
	virtual void destructResImpl() {}

public:
	void addListener(IResourceListener* listener);
	void removeListener(IResourceListener* listener);

public:
	void setLoadAsync(bool b);

public:
	void setUserData(void* user_data);
	void* getUserData() const;

protected:
	ResourceManager*            manager_;
	IResourceCreator*           res_creator_;
	String                      name_;
	ListenerList                listeners_;
	uint                        ref_count_;
	bool						load_async_;
	volatile uint32             status_;
	void*						user_data_;

	uint32						res_load_priority_;
};

}