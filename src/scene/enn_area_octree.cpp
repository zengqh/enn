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
// File: enn_area_octree.cpp
//

#include "enn_area_octree.h"

namespace enn
{

AreaOctree::AreaOctree()
: area_root_(0)
, octree_(0)
{

}

AreaOctree::~AreaOctree()
{
	ENN_SAFE_DELETE(octree_);
}

void AreaOctree::initOctree(const AxisAlignedBox& box, int maxDepth)
{
	octree_ = ENN_NEW Octree();
	octree_->init(box, maxDepth);
}

void AreaOctree::updateMaxDepth(int maxDepth)
{
	octree_->updateMaxDepth(maxDepth);
}

void AreaOctree::updateAreaBox(const AxisAlignedBox& box)
{
	octree_->updateBox(box);
}

void AreaOctree::updateSceneNodeInOctree(SceneNode* node)
{
	octree_->updateObject(node);
}

const AxisAlignedBox& AreaOctree::getCullBoundBox() const
{
	return octree_->root_->getCullBoundBox();
}

AreaOctree::ActionResult AreaOctree::findObject(IFindSceneObject* finder)
{
	return findObjectImpl(octree_->root_, finder, PARTIAL);
}

AreaOctree::ActionResult AreaOctree::findObjectImpl(OctreeNode* ocNode, IFindSceneObject* finder, Visibility parentVis)
{
	Visibility vis;

	if (parentVis == FULL)
	{
		vis = FULL;
	}
	else
	{
		vis = finder->onTestAABB(ocNode->getCullBoundBox());
	}

	switch (vis)
	{
	case NONE:
		break;

	case PARTIAL:
	case FULL:
		ENN_FOR_EACH(OctreeNode::ObjectList, ocNode->obj_list_, it)
		{
			if (finder->onFindObject(*it, vis) == END)
			{
				return END;
			}
		}

		// test child
		for (int i = 0; i < 8; ++i)
		{
			if (OctreeNode* child = ocNode->children_list_[i])
			{
				if (findObjectImpl(child, finder, vis) == END)
				{
					return END;
				}
			}
		}

		break;
	}

	return CONTINUE;
}

}