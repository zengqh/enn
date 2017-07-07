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
// File: enn_resource_manager.h
//

#pragma once

#include "enn_resource.h"
#include "enn_auto_name_manager.h"

namespace enn
{

class ResourceManager;
class MsgQueueManager;
//////////////////////////////////////////////////////////////////////////
class ResAutoCreatorBase : public AllocatedObject, public IResourceCreator
{
public:
	virtual ~ResAutoCreatorBase() {}

	virtual void release();
	virtual bool prepareResource( Resource* res, void* context );

public:
	void setBasePath( const String& basePath ) { m_basePath = basePath; }
	const String& getBasePath() const { return m_basePath; }
	virtual String getLocateFileName( Resource* res ) const;

protected:
	virtual bool prepareResourceImpl( Resource* res, MemoryBuffer* buff ) = 0;

protected:
	String m_basePath;
};

//////////////////////////////////////////////////////////////////////////
struct IResourceScheduler
{
	virtual void release() = 0;
	virtual void load( Resource* res, bool async ) = 0;
	virtual void unload( Resource* res, bool async ) = 0;
	virtual void checkLoad( Resource* res ) = 0;
	virtual void releaseRes( Resource* res ) = 0;
};


//////////////////////////////////////////////////////////////////////////
class ResourceScheduler : 
	public AllocatedObject, 
	public IResourceScheduler, 
	public IMsgQueueListener,
	public IFileSystemListener,
	public AutoNameManager
{
public:
	ResourceScheduler();
	virtual ~ResourceScheduler();

public:
	void registerMsg();

public:
	virtual void release();
	virtual void load( Resource* res, bool async );
	virtual void unload( Resource* res, bool async );
	virtual void checkLoad( Resource* res );
	virtual void releaseRes( Resource* res );

	virtual void onWorkImpl( Message* msg );

	virtual void onGetFileResult( const String& fileName, void* userData, MemoryBuffer* buff, FileSystemGetResult result );
};

extern ResourceScheduler*	g_res_scheduler_;

//////////////////////////////////////////////////////////////////////////
class ResourceManager : public AllocatedObject, public AutoNameManager
{
public:
	typedef enn::unordered_map<String, Resource*>::type ResourceMap;

public:
	ResourceManager();
	virtual ~ResourceManager();

public:
	virtual void init();

	void setAutoCreator( ResAutoCreatorBase* creator );
	void setScheduler( IResourceScheduler* scheduler );

	ResAutoCreatorBase* getAutoCreator()  const 
	{ 
		return m_autoCreator; 
	}

	IResourceScheduler* getScheduler() const 
	{ 
		return m_scheduler; 
	}

	Resource* createResource( const String& name );
	Resource* getOrCreateResource( const String& name );
	Resource* getResource( const String& name ) const;

public:
	void load( Resource* res, bool async );
	void unload( Resource* res, bool async );
	void checkLoad( Resource* res );
	void releaseRes( Resource* res );

protected:
	void destroyAllResources();
	virtual Resource* createImpl() = 0;

public:
	virtual void deleteImpl( Resource* res );

protected:
	ResAutoCreatorBase*  m_autoCreator;
	IResourceScheduler*  m_scheduler;
	ResourceMap          m_resMap;
};

//////////////////////////////////////////////////////////////////////////
template<class T>
void _bindResourceRoutine(ResPtr<T>& ptr, const String& name, ResourceManager* res_mgr)
{
	ptr.release();

	if (!name.empty())
	{
		if (T* res = static_cast<T*>(res_mgr->getOrCreateResource(name)))
		{
			ptr.attach(res);
			res->checkLoad();
		}
	}
}

template<class T>
void _bindResourceRoutine(ResPtr<T>& ptr, const String& name, ResourceManager* res_mgr, IResourceListener* listener)
{
	if (ptr)
	{
		ptr->removeListener(listener);
		ptr.release();
	}

	if (name.size())
	{
		if (T* res = static_cast<T*>(res_mgr->getOrCreateResource(name)))
		{
			ptr.attach(res);
			res->addListener(listener);
			res->checkLoad();
		}
	}
}

#define KHAOS_RESMAN_COMM_OVERRIDE(type) \
	public: \
	type* create##type(const String& name) { return static_cast<type*>(createResource(name)); } \
	type* getOrCreate##type(const String& name) { return static_cast<type*>(getOrCreateResource(name)); } \
	type* get##type(const String& name) { return static_cast<type*>(getResource(name)); }

}