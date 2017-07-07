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
// Time: 2014/02/12
// File: enn_root.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_render_setting.h"
#include "enn_d3d11_render_device.h"
#include "enn_timer.h"
#include "enn_d3d11_render_window.h"
#include "enn_msg_queue.h"
#include "enn_file_system.h"
#include "enn_scene_graph.h"
#include "enn_texture_manager.h"
#include "enn_mesh_manager.h"
#include "enn_material_manager.h"
#include "enn_render_pipe.h"
#include "enn_scene_graph.h"
#include "enn_d3d11_sprite_render.h"

#include "../src/OIS/includes/OISInputManager.h"
#include "../src/OIS/includes/OISException.h"
#include "../src/OIS/includes/OISKeyboard.h"
#include "../src/OIS/includes/OISMouse.h"
#include "../src/OIS/includes/OISJoyStick.h"
#include "../src/OIS/includes/OISEvents.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
struct RootSetting : public AllocatedObject
{
	RenderSetting	render_setting_;

	String			file_system_path_;
	Color			clear_clr_;
	int				num_threads_;
};


//////////////////////////////////////////////////////////////////////////
class Root : public AllocatedObject
{
	typedef unordered_map<String, SceneGraph*>::type SceneGraphMap;
	typedef map<int, CameraNode*>::type CameraMap;
	typedef vector<CameraNode*>::type CameraList;

public:
	Root();
	~Root();

public:
	void init(const RootSetting& root_setting);
	void shutdown();

public:
	const RootSetting& getRootSetting() const { return root_setting_; }

public:
	SceneGraph* createSceneGraph(const String& name);
	void destroySceneGraph(const String& name);
	void destroyAllSceneGraph();
	SceneGraph* getSceneGraph(const String& name) const;

public:
	void addActiveCamera(CameraNode* cameraNode);
	void removeActiveCamera(CameraNode* cameraNode);

protected:
	void createRenderDevice();
	void createRenderWindow();
	void createTimeSystem();
	void createWorkQueue();
	void createMsgQueue();
	void createResScheduler();
	void createFileSystem();
	void createTexManager();
	void createMeshManager();
	void createMtrManager();
	void createRenderPipe();
	void createNodeFactroy();
	void createSceneGraph();

	void destroyRenderDevice();
	void destroyRenderWindow();
	void destroyTimeSystem();
	void destroyWorkQueue();
	void destroyMsgQueue();
	void destroyResScheduler();
	void destroyFileSystem();
	void destroyTexManager();
	void destroyMeshManager();
	void destroyMtrManager();
	void destroyRenderPipe();
	void destroySceneGraph();
	void destroyNodeFactroy();

	void createWorld();
	void destroyWorld();

public:
	void run();

	void start();
	void stop();

	void start_console();
	void stop_console();

protected:
	RootSetting			root_setting_;

	SceneGraphMap       scene_graph_map_;

	CameraList			camera_list_;
};

extern Root*		g_root_;

}