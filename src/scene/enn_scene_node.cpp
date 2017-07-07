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
// Time: 2014/02/16
// File: enn_scene_node.cpp
//

#include "enn_scene_node.h"
#include "enn_area_octree.h"
#include "enn_scene_graph.h"
#include "enn_render_pipe.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
SceneNode::SceneNode()
: sg_(0)
, octree_node_(0)
, area_(0)
{
	type_flag_.set_flag(NODE_SCENE);
	dirty_flag_.set_flag(DF_WORLD_AABB | DF_OCTREE_POS);
}

SceneNode::~SceneNode()
{

}

const AxisAlignedBox& SceneNode::getWorldAABB() const
{
	const_cast<SceneNode*>(this)->checkUpdateWorldAABB();
	return world_aabb_;
}

const AxisAlignedBox& SceneNode::getObjectAABB() const
{
	return object_aabb_;
}

void SceneNode::onSelfAdd()
{
	Node::onSelfAdd();

	SceneNode* parent = static_cast<SceneNode*>(parent_);
	area_ = parent->getArea();
}

void SceneNode::onSelfRemove()
{
	Node::onSelfRemove();

	if (octree_node_)
	{
		octree_node_->removeObject(this);
	}

	area_ = 0;
}

void SceneNode::setSelfTransformDirty()
{
	Node::setSelfTransformDirty();
	setWorldAABBDirty();
}

void SceneNode::setParentTransformDirty()
{
	Node::setParentTransformDirty();
	setWorldAABBDirty();
}

void SceneNode::update()
{
	checkUpdateOctreeNode();
}

void SceneNode::onObjectAABBDirty()
{
	setWorldAABBDirty();
}

void SceneNode::setWorldAABBDirty()
{
	dirty_flag_.set_flag(DF_WORLD_AABB);

	setOctreePosDirty();
}

void SceneNode::checkUpdateWorldAABB()
{
	checkUpdateTransform();

	if (isTerranSection())
	{
		int c = 1;
	}

	if (dirty_flag_.test_flag(DF_WORLD_AABB))
	{
		world_aabb_ = getObjectAABB();

		world_aabb_.transformAffine(derived_matrix_);

		dirty_flag_.reset_flag(DF_WORLD_AABB);
	}
}

void SceneNode::checkUpdateOctreeNode()
{
	checkUpdateWorldAABB();

	if (dirty_flag_.test_flag(DF_OCTREE_POS))
	{
		if (area_)
		{
			area_->updateSceneNodeInOctree(this);
		}

		dirty_flag_.reset_flag(DF_OCTREE_POS);
	}
}

void SceneNode::setOctreePosDirty()
{
	dirty_flag_.set_flag(DF_OCTREE_POS);
}

bool SceneNode::isCameraNode() const
{
	return type_flag_.test_flag(NODE_CAMERA);
}

bool SceneNode::isLightNode() const
{
	return type_flag_.test_flag(NODE_LIGHT);
}

bool SceneNode::isMeshNode() const
{
	return type_flag_.test_flag(NODE_MESH);
}

bool SceneNode::isWaterNode() const
{
	return type_flag_.test_flag(NODE_WATER);
}

bool SceneNode::isRenderableNode() const
{
	return type_flag_.test_flag(NODE_RENDERABLE);
}

bool SceneNode::isTerranSection() const
{
	return type_flag_.test_flag(NODE_TERRAIN_SECTION);
}

//////////////////////////////////////////////////////////////////////////
RenderableNode::RenderableNode() : lights_info_(0), light_info_update_(0)
{
	type_flag_.set_flag(NODE_RENDERABLE);
}

RenderableNode::~RenderableNode()
{
	ENN_SAFE_DELETE(lights_info_);
}

LightsInfo* RenderableNode::getLightsInfo()
{
	if (!lights_info_)
	{
		lights_info_ = ENN_NEW LightsInfo();
	}

	uint32 sys_frame_no = g_render_device_->getCurrFrameNo();

	if (light_info_update_ < sys_frame_no)
	{
		light_info_update_ = sys_frame_no;
		g_render_pipe_->queryLightsInfo(world_aabb_, *lights_info_, isReceiveShadow());
	}

	return lights_info_;
}

}