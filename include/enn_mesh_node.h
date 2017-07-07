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
// Time: 2014/02/18
// File: enn_mesh_node.h
//

#pragma once

#include "enn_renderable.h"
#include "enn_scene_node.h"
#include "enn_resource.h"
#include "enn_mesh.h"
#include "enn_skeleton.h"

namespace enn
{

class MeshNode;
//////////////////////////////////////////////////////////////////////////
class SubEntity : public Renderable
{
public:
	SubEntity()
	: mesh_node_(0)
	, sub_idx_(0)
	, aabb_world_dirty_(true)
	, mtr_(0)
	{

	}

	virtual ~SubEntity() {}

public:
	void setMeshNode(MeshNode* node)
	{
		mesh_node_ = node;
	}

	void setSubIndex(int idx)
	{
		sub_idx_ = idx;
	}

	virtual int getSubIdx() const 
	{
		return sub_idx_;
	}

	virtual LightsInfo* getLightsInfo() const;

	void setAABBWorldDirty()
	{
		aabb_world_dirty_ = true;
	}

	void setMaterial(const String& mtr_name);

	virtual Material* getMaterial() const { return mtr_.get(); }

	virtual const Matrix4& getWorldMatrix() const;

	virtual const AxisAlignedBox& getAABBWorld() const;

	void addToRenderer();

	virtual void render(void* context);

protected:
	MeshNode*				mesh_node_;
	int						sub_idx_;
	AxisAlignedBox			world_aabb_;
	bool					aabb_world_dirty_;

	MaterialPtr				mtr_;
};

//////////////////////////////////////////////////////////////////////////
// Mesh node
class MeshNode : public RenderableNode, public IResourceListener
{
	ENN_DECLARE_RTTI(MeshNode)

public:
	typedef enn::vector<SubEntity*>::type SubEntityList;

public:
	MeshNode();
	virtual ~MeshNode();

public:
	void setMesh(const String& name);
	Mesh* getMesh() const;

public:
	void setMaterial(const String& name);

protected:
	virtual void addToRender();
	virtual void setWorldAABBDirty();
	virtual const AxisAlignedBox& getObjectAABB() const;

	void clearSubEntity();
	void initSubEntity();
	void initSkeleton();

public:
	virtual void onResourceLoaded( Resource* res );
	virtual void onResourceUpdate( Resource* res );

protected:
	MeshPtr					mesh_;
	SubEntityList			sub_entity_list_;
};

}