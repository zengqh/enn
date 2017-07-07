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
// File: enn_scene_node.h

#pragma once

#include "enn_node.h"
#include "enn_scene_object.h"

namespace enn
{

class SceneGraph;
class OctreeNode;
class AreaOctree;
class LightsInfo;
//////////////////////////////////////////////////////////////////////////
class SceneNode : public Node, public ISceneObjectListener
{
	ENN_DECLARE_RTTI(SceneNode)

public:
	SceneNode();
	virtual ~SceneNode();

public:
	virtual const AxisAlignedBox& getWorldAABB() const;
	virtual const AxisAlignedBox& getObjectAABB() const;

	virtual void onSelfAdd();
	virtual void onSelfRemove();
	
public:
	void setSceneGraph(SceneGraph* sg) { sg_ = sg; }
	SceneGraph* getSceneGraph() const { return sg_; }

	void setOctreeNode(OctreeNode* oc) { octree_node_ = oc; }
	OctreeNode* getOctreeNode() const { return octree_node_; }

	virtual AreaOctree* getArea() const { return area_; }

public:
	virtual bool isCameraNode() const;
	virtual bool isLightNode() const;
	virtual bool isMeshNode() const;
	virtual bool isWaterNode() const;
	virtual bool isRenderableNode() const;
	virtual bool isTerranSection() const;

protected:
	virtual void setSelfTransformDirty();
	virtual void setParentTransformDirty();

	virtual void update();
	virtual void onObjectAABBDirty();

protected:
	virtual void setWorldAABBDirty();
	virtual void checkUpdateWorldAABB();
	virtual void checkUpdateOctreeNode();
	virtual void setOctreePosDirty();

protected:
	SceneGraph*							sg_;
	OctreeNode*							octree_node_;

	AreaOctree*							area_;

	mutable AxisAlignedBox				world_aabb_;
	mutable AxisAlignedBox				object_aabb_;
};

//////////////////////////////////////////////////////////////////////////
class RenderableNode : public SceneNode
{
	ENN_DECLARE_RTTI(RenderableNode)

public:
	RenderableNode();

	virtual ~RenderableNode();

public:
	virtual void addToRender() {}

	void setCastShadow( bool en ) 
	{ 
		if (en)
		{
			features_flag_.set_flag(SHADOW_ENABLE_CAST); 
		}
		else
		{
			features_flag_.reset_flag(SHADOW_ENABLE_CAST); 
		}
	}

	bool isCastShadow() const 
	{ 
		return features_flag_.test_flag(SHADOW_ENABLE_CAST); 
	}

	void setReceiveShadow( bool en ) 
	{ 
		if (en)
		{
			features_flag_.set_flag(SHADOW_ENABLE_RECEIVE); 
		}
		else
		{
			features_flag_.reset_flag(SHADOW_ENABLE_RECEIVE); 
		}
	}

	bool isReceiveShadow() const 
	{ 
		return features_flag_.test_flag(SHADOW_ENABLE_RECEIVE); 
	}

	LightsInfo* getLightsInfo();

protected:
	LightsInfo*					lights_info_;
	uint32						light_info_update_;
};

}