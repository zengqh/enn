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
// Time: 2014/05/15
// File: enn_root.cpp
//

#include "enn_root.h"



namespace enn
{

Root* g_root_ = 0;

Root::Root()
{
	g_root_ = this;
}

Root::~Root()
{

}

void Root::init(const RootSetting& root_setting)
{
	root_setting_ = root_setting;

	createTimeSystem();
	createWorkQueue();
	createMsgQueue();
	createResScheduler();
	createFileSystem();
	createRenderDevice();
	createRenderWindow();
	createMtrManager();
	createTexManager();
	createMeshManager();
	createNodeFactroy();
	createSceneGraph();
	createRenderPipe();
	createWorld();
}

void Root::shutdown()
{
	destroyWorld();
	destroyRenderPipe();
	destroySceneGraph();
	destroyNodeFactroy();
	destroyMeshManager();
	destroyTexManager();
	destroyMtrManager();
	destroyRenderWindow();
	destroyRenderDevice();
	destroyFileSystem();
	destroyResScheduler();
	destroyMsgQueue();
	destroyWorkQueue();
	destroyTimeSystem();
}

SceneGraph* Root::createSceneGraph(const String& name)
{
	if (scene_graph_map_.find(name) != scene_graph_map_.end())
	{
		ENN_ASSERT(0);
	}

	SceneGraph* sg = ENN_NEW SceneGraph();
	sg->setName(name);

	scene_graph_map_.insert(SceneGraphMap::value_type(name, sg));

	return sg;
}

void Root::destroySceneGraph(const String& name)
{
	SceneGraphMap::iterator it = scene_graph_map_.find(name);

	if (it == scene_graph_map_.end())
	{
		ENN_ASSERT(0);
	}

	SceneGraph* sg = it->second;
	ENN_SAFE_DELETE(sg);

	scene_graph_map_.erase(it);
}


void Root::destroyAllSceneGraph()
{
	ENN_FOR_EACH(SceneGraphMap, scene_graph_map_, it)
	{
		SceneGraph* sg = it->second;

		ENN_SAFE_DELETE(sg);
	}

	scene_graph_map_.clear();
}

SceneGraph* Root::getSceneGraph(const String& name) const
{
	SceneGraphMap::const_iterator it = scene_graph_map_.find(name);

	if (it == scene_graph_map_.end())
	{
		ENN_ASSERT(0);
		return 0;
	}

	SceneGraph* sg = it->second;
	return sg;
}

void Root::addActiveCamera(CameraNode* cameraNode)
{
	ENN_FOR_EACH(CameraList, camera_list_, it)
	{
		CameraNode* cn = *it;
		if (cameraNode == cn)
		{
			ENN_ASSERT(0);
			return;
		}
	}

	camera_list_.push_back(cameraNode);
}

void Root::removeActiveCamera(CameraNode* cameraNode)
{
	ENN_FOR_EACH(CameraList, camera_list_, it)
	{
		CameraNode* cn = *it;
		if (cameraNode == cn)
		{
			camera_list_.erase(it);
			return;
		}
	}
}

void Root::createRenderDevice()
{
	ENN_NEW D3D11RenderDevice();
}

void Root::createRenderWindow()
{
	ENN_NEW D3D11RenderWindow(root_setting_.render_setting_);
}

void Root::createTimeSystem()
{
	ENN_NEW TimeSystem();
}

void Root::createWorkQueue()
{
	ENN_NEW WorkQueue();
}

void Root::createMsgQueue()
{
	ENN_NEW MsgQueueManager(g_work_queue_);
}

void Root::createResScheduler()
{
	ENN_NEW ResourceScheduler();
}

void Root::createFileSystem()
{
	ENN_NEW FileSystem();

	g_file_system_->setRootPath(root_setting_.file_system_path_);
}

void Root::createTexManager()
{
	ENN_NEW TextureManager();
}

void Root::createMeshManager()
{
	ENN_NEW MeshManager();
}

void Root::createMtrManager()
{
	ENN_NEW MaterialManager();
}

void Root::createRenderPipe()
{
	ENN_NEW RenderPipe();
}

void Root::createNodeFactroy()
{
	ENN_NEW NodeFactory();
}

void Root::createSceneGraph()
{
	ENN_NEW SceneGraph();
}

void Root::destroyRenderDevice()
{
	ENN_SAFE_DELETE(g_render_device_);
}

void Root::destroyRenderWindow()
{
	ENN_SAFE_DELETE(g_render_wnd_);
}

void Root::destroyTimeSystem()
{
	ENN_SAFE_DELETE(g_time_system_);
}

void Root::destroyWorkQueue()
{
	ENN_SAFE_DELETE(g_work_queue_);
}

void Root::destroyMsgQueue()
{
	ENN_SAFE_DELETE(g_msg_queue_mgr_);
}

void Root::destroyResScheduler()
{
	ENN_SAFE_DELETE(g_res_scheduler_);
}

void Root::destroyFileSystem()
{
	ENN_SAFE_DELETE(g_file_system_);
}

void Root::destroyTexManager()
{
	ENN_SAFE_DELETE(g_tex_manager_);
}

void Root::destroyMeshManager()
{
	ENN_SAFE_DELETE(g_mesh_mgr_);
}

void Root::destroyMtrManager()
{
	ENN_SAFE_DELETE(g_mtr_mgr_);
}

void Root::destroyRenderPipe()
{
	ENN_SAFE_DELETE(g_render_pipe_);
}

void Root::destroySceneGraph()
{
	ENN_SAFE_DELETE(g_scene_graph_);
}

void Root::destroyNodeFactroy()
{
	ENN_SAFE_DELETE(g_node_factory_);
}

void Root::createWorld()
{
	createSceneGraph("world");
}

void Root::destroyWorld()
{
	destroySceneGraph("world");
}

void Root::start()
{
	g_work_queue_->init();
	g_work_queue_->create_threads(root_setting_.num_threads_);

	g_render_device_->init();
	g_render_wnd_->init();
	g_tex_manager_->init();
	g_mtr_mgr_->init();
	g_mesh_mgr_->init();
	g_render_pipe_->init();
}

void Root::stop()
{
	g_work_queue_->uninit();
	g_render_pipe_->shutdown();
}

void Root::start_console()
{

}

void Root::stop_console()
{

}

void Root::run()
{
	g_time_system_->update();

	float ui_queue_time_piece = Math::maxVal(1.0f / 60 - g_time_system_->getElapsedTime(), 0.005f);

	g_msg_queue_mgr_->updateQueue(MsgQueueManager::UI_QUEUE, ui_queue_time_piece);

	ENN_FOR_EACH(SceneGraphMap, scene_graph_map_, it)
	{
		SceneGraph* sg = it->second;

		sg->update();
	}

	g_render_pipe_->beginRender();

	ENN_FOR_EACH(CameraList, camera_list_, it)
	{
		CameraNode* cn = *it;

		g_render_pipe_->render(cn);
	}

	g_render_pipe_->endRender();
}

}