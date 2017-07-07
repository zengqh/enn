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
// Time: 2014/06/10
// File: enn_node_factory.cpp

#include "enn_node_factory.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
NodeFactory* g_node_factory_ = 0;

//////////////////////////////////////////////////////////////////////////
NodeFactory::NodeFactory()
{
	g_node_factory_ = this;
	_init();
}

NodeFactory::~NodeFactory()
{
	g_node_factory_ = 0;
}

SceneNode* NodeFactory::createSceneNode(ClassType type)
{
	CreatorType fn = _findCreator(type);
	if (fn)
		return fn();
	return 0;
}

void NodeFactory::destroySceneNode(SceneNode* node)
{
	node->destruct();
}

template<class T>
SceneNode* NodeFactory::_createSceneNode()
{
	return ENN_NEW T;
}

NodeFactory::CreatorType NodeFactory::_findCreator(ClassType type) const
{
	CreatorMap::const_iterator it = creator_map_.find(type);
	if (it != creator_map_.end())
	{
		return it->second;
	}
			
	return 0;
}

template<class T>
void NodeFactory::_registerCreator()
{
	if (_findCreator(ENN_CLASS_TYPE(T)))
	{
		ENN_ASSERT(0);
		return;
	}

	creator_map_.insert(CreatorMap::value_type(ENN_CLASS_TYPE(T), _createSceneNode<T>));
}

void NodeFactory::_init()
{
	_registerCreator<SceneNode>();
	_registerCreator<AreaNode>();
	_registerCreator<MeshNode>();
	_registerCreator<CameraNode>();
	_registerCreator<LightNode>();
	_registerCreator<EnvProbeNode>();
	_registerCreator<TerrainSection>();
	_registerCreator<WaterNode>();
}

}