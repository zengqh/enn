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
// Time: 2014/02/19
// File: enn_octree.h

#pragma once

#include "enn_platform_headers.h"
#include "enn_math.h"
#include "enn_scene_node.h"

namespace enn
{

class SceneNode;
class OctreeNode : public AllocatedObject
{
public:
	typedef SceneNode						ObjectType;
	typedef set<ObjectType*>::type			ObjectList;

public:
	OctreeNode();
	~OctreeNode();

public:
	void addObject(ObjectType* obj);
	void removeObject(ObjectType* obj);
	void removeAllObject();

	bool selfCanHold(const AxisAlignedBox& box, bool ignoreSize) const;
	bool childCanHoldBySize(const AxisAlignedBox& box) const;

	void getChildIndexes(const AxisAlignedBox &box, int *x, int *y, int *z);

	void setBoundBox( const AxisAlignedBox& box );
	void setBoundBox( const Vector3& vmin, const Vector3& vmax );

	const AxisAlignedBox& getBoundBox() const { return box_; }
	const AxisAlignedBox& getCullBoundBox() const { return cull_box_; }

protected:
	void updateCullBoundBox();

public:
	OctreeNode*		parent_;

	union
	{
		OctreeNode*		children_list_[8];
		OctreeNode*		children_[2][2][2];
	};

	ObjectList		obj_list_;

protected:
	AxisAlignedBox			box_;
	AxisAlignedBox			cull_box_;
};

//////////////////////////////////////////////////////////////////////////
class Octree : public AllocatedObject
{
public:
	typedef OctreeNode::ObjectType ObjectType;

public:
	Octree();
	~Octree();

public:
	void init( const AxisAlignedBox& box, int maxDepth );
	void clear();

	void updateMaxDepth(int maxDepth);
	void updateBox(const AxisAlignedBox& box);
	void updateObject(ObjectType* obj);

protected:
	void addObjectToNode(OctreeNode* node, ObjectType* obj, int depth);
	void removeObjectFromNode(ObjectType* obj);

public:
	OctreeNode*		root_;
	int				max_depth_;
};

}