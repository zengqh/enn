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
// File: enn_scene_graph.h
//

#pragma once

#include "enn_node_factory.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class SceneGraph : public AllocatedObject
{
public:
	typedef unordered_map<String, SceneNode*>::type		NodeMap;
	typedef vector<SceneNode*>::type					NodeList;

public:
	SceneGraph();
	~SceneGraph();

public:
	void setName(const String& name);
	const String& getName() const;

public:
	SceneNode* createSceneNode( ClassType type, const String& name );

	template<class T>
	T* createSceneNode( const String& name ) { return static_cast<T*>(createSceneNode(ENN_CLASS_TYPE(T), name)); }

	template<class T>
	T* createSceneNode()
	{ 
		static int uniqueId = 0;
		String name("__SceneNode_");
		name += to_string(uniqueId++);

		return static_cast<T*>(createSceneNode(ENN_CLASS_TYPE(T), name)); 
	}

	AreaNode* createRootNode( const String& name );
	AreaNode* getRootNode() const { return static_cast<AreaNode*>(roots_[0]); }

	void destroySceneNode( const String& name );
	void destroySceneNode(SceneNode* node);

	void destroyDerivedSceneNode(const String& name);
	void destroyDerivedSceneNode(SceneNode* node);

	void destroyAllSceneNodes();

	SceneNode* getSceneNode( const String& name ) const;

	void update();
	void find(AreaOctree::IFindSceneObject* finder);

protected:
	String				name_;
	NodeMap				node_map_;
	NodeList			roots_;
};

extern SceneGraph* g_scene_graph_;

//////////////////////////////////////////////////////////////////////////
class FindSceneBase : public AreaOctree::IFindSceneObject, public AllocatedObject
{
public:
	FindSceneBase();
	virtual ~FindSceneBase() {}
	
	void setFinder(Camera* cam);
	void setNodeMask(uint32 mask);
	void setCullDisable(bool b);
	void setContext(void* context);

	void find(SceneGraph* sg);

protected:
	Camera*					camera_;
	uint32					node_mask_;
	bool					cull_disable_;
	void*					context_;

	SceneGraph*				curr_find_sg_;
};

}