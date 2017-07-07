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
// File: enn_main_result_pass.cpp
//

#include "enn_main_result_pass.h"
#include "enn_general_render.h"
#include "enn_render_pipe.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
FindSceneMainResult::FindSceneMainResult(
	RenderableNodeList* ra_list, 
	LightNodeList* lt_list, 
	EnvProbeNodeList* ep_list,
	WaterNodeList* water_nodes)
	: ra_nodes_(ra_list)
	, lt_nodes_(lt_list)
	, env_probe_nodes_(ep_list)
	, water_nodes_(water_nodes)
{
	ra_list->clear();
	lt_list->clear();
	ep_list->clear();
	water_nodes->clear();
}

FindSceneMainResult::~FindSceneMainResult()
{

}

AreaOctree::Visibility FindSceneMainResult::onTestAABB(const AxisAlignedBox& box)
{
	return (AreaOctree::Visibility)camera_->testVisibilityEx(box);
}

AreaOctree::ActionResult FindSceneMainResult::onFindObject(Octree::ObjectType* obj, AreaOctree::Visibility vis)
{
	if (!preCull(obj))
	{
		return AreaOctree::CONTINUE;
	}

	if (vis == AreaOctree::FULL)
	{
		onFindSceneNode(obj);
	}
	else
	{
		if (camera_->testVisibility(obj->getWorldAABB()))
		{
			onFindSceneNode(obj);
		}
	}

	return AreaOctree::CONTINUE;
}

void FindSceneMainResult::onFindRenderNode(SceneNode* node)
{
	ra_nodes_->push_back(static_cast<RenderableNode*>(node));
}

void FindSceneMainResult::onFindLightNode(SceneNode* node)
{
	lt_nodes_->push_back(static_cast<LightNode*>(node));
}

void FindSceneMainResult::onFindEnvProbeNode(SceneNode* node)
{
	env_probe_nodes_->push_back(static_cast<EnvProbeNode*>(node));
}

void FindSceneMainResult::onFindTerrainSection(SceneNode* node)
{
	TerrainSection* ts = static_cast<TerrainSection*>(node);
	ts->addRenderQueue();
}

void FindSceneMainResult::onFindWaterNode(SceneNode* node)
{
	water_nodes_->push_back(static_cast<WaterNode*>(node));
}

void FindSceneMainResult::onFindSceneNode(SceneNode* node)
{
	if (node->isRenderableNode())
	{
		onFindRenderNode(node);
	}
	else if (node->isLightNode())
	{
		onFindLightNode(node);
	}
	else if (node->isTerranSection())
	{
		onFindTerrainSection(node);
	}
	else if (node->isWaterNode())
	{
		onFindWaterNode(node);
	}
}

bool FindSceneMainResult::preCull(SceneNode* node)
{
	if (!(node_mask_ & node->getTypeFlag().to_ulong_flag()))
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void MainResultPass::prepare()
{

}

void MainResultPass::render()
{
	g_general_render_->beginRender(GeneralRender::GR_MAIN_RESULT);

	const RenderableNodeList& ra_list = g_render_pipe_->getMainRAList();
	ENN_FOR_EACH_CONST(RenderableNodeList, ra_list, it)
	{
		RenderableNode* ra_node = *it;

		ra_node->addToRender();
	}

	g_general_render_->endAdd();

	MainResultEffectParams eff_params;
	eff_params.render_cam_ = g_render_pipe_->getMainCam();
	eff_params.light_buffer_ = g_render_pipe_->getLightBufferPass()->getLitBuffer();

	g_render_device_->setSolidStateLessAndEqual(true);
	g_general_render_->renderSolidDirect(eff_params);

	g_render_device_->setSolidStateLessAndEqual(false);
	g_general_render_->renderTransDirect(eff_params);

	// restore common solid state.
	g_render_device_->setSolidState(true);
}

}