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
// File: enn_mesh_node.cpp
//

#include "enn_mesh_node.h"
#include "enn_material_manager.h"
#include "enn_general_render.h"
#include "enn_mesh_manager.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
LightsInfo* SubEntity::getLightsInfo() const
{
	return mesh_node_->getLightsInfo();
}

void SubEntity::setMaterial(const String& mtr_name)
{
	if (!mtr_name.empty())
	{
		Material* mtr = static_cast<Material*>(g_mtr_mgr_->getOrCreateResource(mtr_name));
		mtr_.attach(mtr);

		mtr->checkLoad();
	}
}


const Matrix4& SubEntity::getWorldMatrix() const
{
	return mesh_node_->getDerivedMatrix();
}

const AxisAlignedBox& SubEntity::getAABBWorld() const
{
	if (aabb_world_dirty_)
	{
		SubEntity* pthis = const_cast<SubEntity*>(this);

		pthis->world_aabb_ = mesh_node_->getMesh()->getSubMesh(sub_idx_)->getAABB();
		pthis->world_aabb_.transformAffine(mesh_node_->getDerivedMatrix());

		pthis->aabb_world_dirty_ = false;
	}

	return world_aabb_;
}

void SubEntity::addToRenderer()
{
	g_general_render_->addRenderable(this);
}

void SubEntity::render(void* context)
{
	SubMesh* sm = mesh_node_->getMesh()->getSubMesh(sub_idx_);
	sm->render(context);
}

//////////////////////////////////////////////////////////////////////////
MeshNode::MeshNode()
{

}

MeshNode::~MeshNode()
{
	clearSubEntity();
	if (mesh_)
	{
		mesh_->removeListener(this);
	}
}

void MeshNode::setMesh(const String& name)
{
	clearSubEntity();

	_bindResourceRoutine(mesh_, name, g_mesh_mgr_, this);
}

Mesh* MeshNode::getMesh() const
{
	return mesh_.get();
}

void MeshNode::setMaterial(const String& name)
{
	ENN_FOR_EACH(SubEntityList, sub_entity_list_, it)
	{
		(*it)->setMaterial(name);
	}
}

void MeshNode::addToRender()
{
	ENN_FOR_EACH(SubEntityList, sub_entity_list_, it)
	{
		(*it)->addToRenderer();
	}
}

void MeshNode::setWorldAABBDirty()
{
	SceneNode::setWorldAABBDirty();

	ENN_FOR_EACH(SubEntityList, sub_entity_list_, it)
	{
		(*it)->setAABBWorldDirty();
	}
}

const AxisAlignedBox& MeshNode::getObjectAABB() const
{
	return mesh_ ? mesh_->getAABB() : AxisAlignedBox::BOX_NULL;
}

void MeshNode::clearSubEntity()
{
	ENN_FOR_EACH(SubEntityList, sub_entity_list_, it)
	{
		SubEntity* se = *it;
		ENN_SAFE_DELETE(se);
	}

	sub_entity_list_.clear();
}

void MeshNode::initSubEntity()
{
	ENN_ASSERT(sub_entity_list_.empty());

	uint32 sub_mesh_cnt = mesh_->getSubMeshCount();
	sub_entity_list_.resize(sub_mesh_cnt);

	for (size_t i = 0; i < sub_mesh_cnt; ++i)
	{
		SubMesh* sm = mesh_->getSubMesh(i);

		SubEntity* se = ENN_NEW SubEntity;

		se->setMeshNode(this);
		se->setSubIndex(i);
		se->setMaterial(sm->getMaterialName());

		sub_entity_list_[i] = se;
	}
}

void MeshNode::initSkeleton()
{

}

void MeshNode::onResourceLoaded(Resource* res)
{
	initSubEntity();
	setWorldAABBDirty();
}

void MeshNode::onResourceUpdate(Resource* res)
{
	clearSubEntity();
	initSubEntity();
	setWorldAABBDirty();
}

}