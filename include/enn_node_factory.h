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
// File: enn_node_factory.h

#pragma once

#include "enn_camera_node.h"
#include "enn_mesh_node.h"
#include "enn_area_node.h"
#include "enn_light_node.h"
#include "enn_env_probe_node.h"
#include "enn_terrain_section.h"
#include "enn_water_node.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class NodeFactory : public AllocatedObject
{
public:
	typedef SceneNode* (*CreatorType)();
	typedef unordered_map<ClassType, CreatorType>::type CreatorMap;

public:
	NodeFactory();
	~NodeFactory();

public:
	SceneNode* createSceneNode(ClassType type);
	void       destroySceneNode(SceneNode* node);

protected:
	template<class T>
	static SceneNode* _createSceneNode();

	CreatorType _findCreator(ClassType type) const;

	template<class T>
	void _registerCreator();

	void _init();

protected:
	CreatorMap creator_map_;
};

//////////////////////////////////////////////////////////////////////////
extern NodeFactory* g_node_factory_;

}