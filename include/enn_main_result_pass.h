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
// Time: 2014/02/21
// File: enn_main_result_pass.h
//

#pragma once

#include "enn_pass_base.h"
#include "enn_scene_graph.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class FindSceneMainResult : public FindSceneBase
{
public:
	FindSceneMainResult(RenderableNodeList* ra_list, LightNodeList* lt_list, EnvProbeNodeList* ep_list,
		WaterNodeList* water_nodes);
	~FindSceneMainResult();

public:
	virtual AreaOctree::Visibility onTestAABB(const AxisAlignedBox& box);
	virtual AreaOctree::ActionResult onFindObject(Octree::ObjectType* obj, AreaOctree::Visibility vis);

protected:
	void onFindRenderNode(SceneNode* node);
	void onFindLightNode(SceneNode* node);
	void onFindEnvProbeNode(SceneNode* node);
	void onFindTerrainSection(SceneNode* node);
	void onFindWaterNode(SceneNode* node);
	void onFindSceneNode(SceneNode* node);

	bool preCull(SceneNode* node);

protected:
	RenderableNodeList*		ra_nodes_;
	LightNodeList*			lt_nodes_;
	EnvProbeNodeList*		env_probe_nodes_;
	WaterNodeList*			water_nodes_;
};

//////////////////////////////////////////////////////////////////////////
class MainResultPass : public RenderPassBase
{
public:
	virtual void prepare();
	virtual void render();
};

}