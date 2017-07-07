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
// File: enn_resource_manager.cpp
//

#include "enn_resource_manager.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
void ResAutoCreatorBase::release()
{
	ENN_DELETE this;
}

bool ResAutoCreatorBase::prepareResource( Resource* res, void* context )
{
	MemoryBuffer* buff = (MemoryBuffer*)context;
	return prepareResourceImpl( res, buff );
}

String ResAutoCreatorBase::getLocateFileName( Resource* res ) const
{
	const String& name = res->getName();
	if ( name.size() && name[0] == '/' )
		return  name;

	return m_basePath + "/" + name;
}


//////////////////////////////////////////////////////////////////////////
ResourceScheduler*	g_res_scheduler_ = 0;

ResourceScheduler::ResourceScheduler() 
{
	g_res_scheduler_ = this;
}

ResourceScheduler::~ResourceScheduler()
{

}

void ResourceScheduler::registerMsg()
{
	g_msg_queue_mgr_->registerMsg(MsgQueueManager::UI_QUEUE, MQMSG_UI_BUILD_RESOURCE, this);

	g_msg_queue_mgr_->registerMsg(MsgQueueManager::WORK_QUEUE, MQMSG_CREATE_MANUAL_RESOURCE, this);
	g_msg_queue_mgr_->registerMsg(MsgQueueManager::UI_QUEUE, MQMSG_UI_BUILD_MANUAL_RESOURCE, this);

	g_msg_queue_mgr_->registerMsg(MsgQueueManager::UI_QUEUE, MQMSG_UI_DESTRUCT_RESOURCE, this);
	g_msg_queue_mgr_->registerMsg(MsgQueueManager::UI_QUEUE, MQMSG_UI_RELEASE_RESOURCE, this);
}

void ResourceScheduler::release()
{
	ENN_DELETE this;
}

void ResourceScheduler::load( Resource* res, bool async )
{
	if ( !res->setLoadingStatus() ) return;

	if ( async ) 
	{
		res->addRef();

		IResourceCreator* creator = res->getResCreator();
		if ( creator ) 
		{
			g_msg_queue_mgr_->postMessage(MsgQueueManager::WORK_QUEUE, MQMSG_CREATE_MANUAL_RESOURCE, 0, res, "", res->getLoadPriority());
		}
		else 
		{
			String fileName = res->getManager()->getAutoCreator()->getLocateFileName( res );
			g_file_system_->getAsyncFileData( fileName, res );
		}
	}
	else 
	{
		IResourceCreator* creator = res->getResCreator();

		if ( creator ) 
		{
			bool workOk = creator->prepareResource( res, 0 );
			res->setPreparedStatus( workOk );

			if ( workOk )
			{
				workOk = creator->buildResource( res );
			}

			res->setCompletedStatus( workOk );
		}
		else
		{
			ResAutoCreatorBase* creator = res->getManager()->getAutoCreator();

			bool workOk = false;
			String fileName = creator->getLocateFileName( res );

			MemoryBuffer* db;
			if ( g_file_system_->getFileData( fileName, db ) )
			{
				workOk = creator->prepareResource( res, db );
			}

			res->setPreparedStatus( workOk );

			if ( workOk ) 
			{
				workOk = creator->buildResource( res );
			}

			res->setCompletedStatus( workOk );
		}
	}
}

void ResourceScheduler::unload( Resource* res, bool async )
{
	if ( !res->setUnloadingStatus() )
	{
		ENN_ASSERT(0);
		return;
	}

	if ( async )
	{
		res->addRef();
		g_msg_queue_mgr_->postMessage(MsgQueueManager::UI_QUEUE, MQMSG_UI_DESTRUCT_RESOURCE, res, 0, "");
	}
	else
	{
		res->destructResImpl();
		res->setUnloadedStatus();
	}
}

void ResourceScheduler::checkLoad( Resource* res )
{
	bool async = res->getLoadAsync();
	load( res, async );
}

void ResourceScheduler::releaseRes( Resource* res )
{
	// delete resource always in main thread
	ENN_ASSERT(res->getRefCount() == 1);
	res->getManager()->deleteImpl( res );
}

void ResourceScheduler::onGetFileResult( const String& fileName, void* userData, MemoryBuffer* buff, FileSystemGetResult result )
{
	Resource* res = static_cast<Resource*>(userData);
	bool ok = false;

	if ( result == FS_GR_OK )
	{
		ok = res->getManager()->getAutoCreator()->prepareResource( res, buff );
	}

	res->setPreparedStatus( ok );

	g_msg_queue_mgr_->postMessage(MsgQueueManager::UI_QUEUE, MQMSG_UI_BUILD_RESOURCE, 0, res, "");
}

void ResourceScheduler::onWorkImpl( Message* msg )
{
	Resource* res = static_cast<Resource*>(msg->lparam);

	switch ( msg->msg )
	{
	case MQMSG_CREATE_MANUAL_RESOURCE: 
		{
			bool ok = res->getResCreator()->prepareResource( res, 0 );
			res->setPreparedStatus( ok );
			g_msg_queue_mgr_->postMessage(MsgQueueManager::UI_QUEUE, MQMSG_UI_BUILD_MANUAL_RESOURCE, 0, res, "");
		}
		break;

	case MQMSG_UI_BUILD_MANUAL_RESOURCE: 
		{
			bool ok = false;
			if ( res->getStatus() == Resource::S_PREPARED_OK )
			{
				ok = res->getResCreator()->buildResource( res );
			}

			res->setCompletedStatus( ok );
			res->release();
		}
		break;

	case MQMSG_UI_BUILD_RESOURCE: 
		{
			bool ok = false;
			if ( res->getStatus() == Resource::S_PREPARED_OK )
			{
				ok = res->getManager()->getAutoCreator()->buildResource( res );
			}

			res->setCompletedStatus( ok );
			res->release();
		}
		break;

	case MQMSG_UI_DESTRUCT_RESOURCE: 
		{
			res->destructResImpl();
			res->setUnloadedStatus();

			res->release(); 
		}
		break;

	case MQMSG_UI_RELEASE_RESOURCE: 
		{
			res->getManager()->deleteImpl( res );
		}
		break;

	default:
		ENN_ASSERT(0);
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
ResourceManager::ResourceManager() : m_autoCreator(0), m_scheduler(0)
{

}

ResourceManager::~ResourceManager()
{
	destroyAllResources();

	setScheduler(0);

	setAutoCreator(0);
}

void ResourceManager::init()
{
	setScheduler(g_res_scheduler_);
}

void ResourceManager::setAutoCreator( ResAutoCreatorBase* creator )
{
	ENN_ASSERT(creator);

	ENN_SAFE_RELEASE(m_autoCreator);
	m_autoCreator = creator;
}

void ResourceManager::setScheduler( IResourceScheduler* scheduler )
{
	ENN_ASSERT(scheduler);

	ENN_SAFE_RELEASE(m_scheduler);
	m_scheduler = scheduler;
}

Resource* ResourceManager::createResource( const String& name1 )
{
	const String& name = _checkNextName(name1); 

	if ( m_resMap.find(name) != m_resMap.end() )
	{
		ENN_ASSERT(0);
		return 0;
	}

	Resource* ptr = createImpl();
	ptr->addRef();
	ptr->setName( name );
	ptr->setManager( this );
	m_resMap.insert( ResourceMap::value_type(ptr->getName(), ptr) );

	return ptr;
}

Resource* ResourceManager::getResource( const String& name ) const
{
	if ( name.empty() ) return 0;

	ResourceMap::const_iterator it = m_resMap.find( name );
	if ( it != m_resMap.end() )
	{
		Resource* ptr = it->second;
		ptr->addRef();
		return ptr;
	}

	return 0;
}

Resource* ResourceManager::getOrCreateResource( const String& name )
{
	Resource* ptr = getResource( name );
	if ( ptr ) return ptr;
	return createResource( name );
}

void ResourceManager::load( Resource* res, bool async )
{
	m_scheduler->load( res, async );
}

void ResourceManager::unload( Resource* res, bool async)
{
	m_scheduler->unload( res, async );
}

void ResourceManager::checkLoad( Resource* res )
{
	m_scheduler->checkLoad( res ); 
}

void ResourceManager::releaseRes( Resource* res )
{
	ENN_ASSERT( res->getRefCount() == 1 );

	ResourceMap::iterator it = m_resMap.find( res->getName() );
	ENN_ASSERT( it != m_resMap.end() );
	m_resMap.erase( it );

	m_scheduler->releaseRes(res);
}

void ResourceManager::destroyAllResources()
{
	for (  ResourceMap::iterator it = m_resMap.begin(), ite = m_resMap.end(); it != ite; ++it )
	{
		Resource* ptr = it->second;
		deleteImpl( ptr );
	}

	m_resMap.clear();
}

void ResourceManager::deleteImpl( Resource* res )
{
	ENN_ASSERT(res->getRefCount() == 1);

	ENN_DELETE res;
}

}
