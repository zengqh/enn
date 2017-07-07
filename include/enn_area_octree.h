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
// File: enn_area_octree.h
//

#pragma once

#include "enn_octree.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class AreaNode;
class AreaOctree : public AllocatedObject
{
public:
	enum Visibility
	{
		NONE,
		PARTIAL,
		FULL
	};

	enum ActionResult
	{
		CONTINUE,
		END
	};

	//////////////////////////////////////////////////////////////////////////
	struct IFindSceneObject
	{
		virtual Visibility onTestAABB(const AxisAlignedBox& box) = 0;
		virtual ActionResult onFindObject(Octree::ObjectType* obj, Visibility vis) = 0;
	};

public:
	AreaOctree();
	~AreaOctree();

public:
	void setAreaRoot(AreaNode* root) { area_root_ = root; }
	void initOctree(const AxisAlignedBox& box, int maxDepth);

	void updateMaxDepth(int maxDepth);
	void updateAreaBox(const AxisAlignedBox& box);
	void updateSceneNodeInOctree(SceneNode* node);

	const AxisAlignedBox& getCullBoundBox() const;
	ActionResult findObject(IFindSceneObject* finder);

protected:
	ActionResult findObjectImpl(OctreeNode* ocNode, IFindSceneObject* finder, Visibility parentVis);

protected:
	AreaNode*		area_root_;
	Octree*			octree_;
};

}