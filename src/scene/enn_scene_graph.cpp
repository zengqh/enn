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
// Time: 2014/02/20
// File: enn_scene_graph.cpp
//

#include "enn_scene_graph.h"

namespace enn
{

SceneGraph* g_scene_graph_ = 0;
//////////////////////////////////////////////////////////////////////////
SceneGraph::SceneGraph()
{
	g_scene_graph_ = this;

	createRootNode("root");
}

SceneGraph::~SceneGraph()
{
	destroyAllSceneNodes();
}

void SceneGraph::setName(const String& name)
{
	name_ = name;
}

const String& SceneGraph::getName() const
{
	return name_;
}

SceneNode* SceneGraph::createSceneNode( ClassType type, const String& name )
{
	if (getSceneNode(name))
	{
		ENN_ASSERT(0);
		return 0;
	}

	SceneNode* snode = g_node_factory_->createSceneNode(type);
	snode->setName(name);
	snode->setSceneGraph(this);

	node_map_.insert(NodeMap::value_type(name, snode));

	return snode;
}

AreaNode* SceneGraph::createRootNode( const String& name )
{
	AreaNode* node = createSceneNode<AreaNode>(name);

	if (node)
	{
		roots_.push_back(node);
	}

	return node;
}

void SceneGraph::destroySceneNode( const String& name )
{
	NodeMap::iterator it = node_map_.find(name);
	ENN_ASSERT(it != node_map_.end());
	SceneNode* node = it->second;
	ENN_ASSERT(node->getSceneGraph() == this);
	node_map_.erase(it);
	node->setSceneGraph(0);

	if (ENN_OBJECT_TYPE(node) == ENN_CLASS_TYPE(AreaNode))
	{
		NodeList::iterator it = std::find(roots_.begin(), roots_.end(), node);
		ENN_ASSERT(it != roots_.end());
		roots_.erase(it);
	}

	// 删除节点
	// 注意它的子节点没有删除，但它的父子节点都会和他脱离干系
	g_node_factory_->destroySceneNode(node);
}

void SceneGraph::destroySceneNode(SceneNode* node)
{
	destroySceneNode(node->getName());
}

void SceneGraph::destroyDerivedSceneNode(const String& name)
{
	NodeMap::iterator it = node_map_.find(name);
	ENN_ASSERT(it != node_map_.end());
	destroyDerivedSceneNode(it->second);
}

void SceneGraph::destroyDerivedSceneNode(SceneNode* node)
{
	// 递归删除节点和他的所有子节点
	// 先删除孩子

	Node::NodeList& children = node->getChildren();
	while (children.size())
	{
		SceneNode* node = static_cast<SceneNode*>(*children.begin());
		destroyDerivedSceneNode(node);
	}

	destroySceneNode(node);
}

void SceneGraph::destroyAllSceneNodes()
{
	// 删除所有节点
	while (roots_.size())
	{
		destroyDerivedSceneNode(roots_.back());
	}

	// 删除不在场景图的node
	while (node_map_.size())
	{
		destroyDerivedSceneNode(node_map_.begin()->second);
	}
}

SceneNode* SceneGraph::getSceneNode( const String& name ) const
{
	// 根据名字访问
	NodeMap::const_iterator it = node_map_.find(name);
	if (it != node_map_.end())
	{
		return it->second;
	}
	
	return 0;
}

void SceneGraph::update()
{
	ENN_FOR_EACH(NodeList, roots_, it)
	{
		(*it)->updateDerived();
	}
}

void SceneGraph::find(AreaOctree::IFindSceneObject* finder)
{
	ENN_FOR_EACH(NodeList, roots_, it)
	{
		AreaNode* area_node = static_cast<AreaNode*>(*it);

		area_node->getArea()->findObject(finder);
	}
}


//////////////////////////////////////////////////////////////////////////
FindSceneBase::FindSceneBase()
: camera_(0)
, node_mask_(enn::Math::ENN_MAX_UNSIGNED)
, cull_disable_(false)
, context_(0)
, curr_find_sg_(0)
{

}

void FindSceneBase::setFinder(Camera* cam)
{
	camera_ = cam;
}

void FindSceneBase::setNodeMask(uint32 mask)
{
	node_mask_ = mask;
}

void FindSceneBase::setCullDisable(bool b)
{
	cull_disable_ = b;
}

void FindSceneBase::setContext(void* context)
{
	context_ = context;
}

void FindSceneBase::find(SceneGraph* sg)
{
	curr_find_sg_ = sg;

	curr_find_sg_->find(this);
}

}