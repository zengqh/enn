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
// File: enn_octree.cpp
//

#include "enn_octree.h"

namespace enn
{

OctreeNode::OctreeNode()
{
	ENN_CLEAR_BLOCK_ZERO(children_list_);
}

OctreeNode::~OctreeNode()
{
	removeAllObject();

	for (int i = 0; i < 8; ++i)
	{
		ENN_SAFE_DELETE(children_list_[i]);
	}
}

void OctreeNode::addObject(ObjectType* obj)
{
	ENN_ASSERT(obj_list_.find(obj) == obj_list_.end());
	ENN_ASSERT(obj->getOctreeNode() == 0);

	obj_list_.insert(obj);
	obj->setOctreeNode(this);
}

void OctreeNode::removeObject(ObjectType* obj)
{
	ObjectList::iterator it = obj_list_.find(obj);

	ENN_ASSERT(it != obj_list_.end());
	ENN_ASSERT(obj->getOctreeNode() == this);

	obj_list_.erase(it);
	obj->setOctreeNode(0);
}

void OctreeNode::removeAllObject()
{
	ENN_FOR_EACH(ObjectList, obj_list_, it)
	{
		(*it)->setOctreeNode(0);
	}

	obj_list_.clear();
}

bool OctreeNode::selfCanHold(const AxisAlignedBox& box, bool ignoreSize) const
{
	// �ж�box�Ƿ������ɵ��ӽڵ㣬���￼�����ĵ�ʹ�С
	// ��ɢ�İ˲���
	// һ�������ܲ�������������ĵ�λ�ڽڵ��ڣ������С�������ڵ㷶Χ

	// �������޴��޷�����
	if (box.isInfinite())
	{
		return false;
	}

	if (box.isNull())
	{
		return false;
	}

	if (!box_.contains(box.getCenter()))
	{
		return false;
	}

	if (ignoreSize)
	{
		return true;
	}

	Vector3 boxSize = box.getSize();
	Vector3 selfSize = box_.getSize();

	return (boxSize.x <= selfSize.x) && (boxSize.y <= selfSize.y) && (boxSize.z <= selfSize.z);
}

bool OctreeNode::childCanHoldBySize(const AxisAlignedBox& box) const
{
	// �ж�box�Ƿ������ɵ��ӽڵ㣬ֻ���Ǵ�С������������λ��
	// ��ɢ�İ˲���
	// һ�������ܲ�������������ĵ�λ�ڽڵ��ڣ������С�������ڵ㷶Χ
	// ����ֻ���ǽڵ㷶Χ������������

	// �������޴��޷�����
	if (box.isInfinite())
	{
		return false;
	}

	if (box.isNull())
	{
		return false;
	}

	// �����СҪ�Ⱥ��Ӵ�СС
	Vector3 childSize = box_.getHalfSize();
	Vector3 boxSize = box.getSize();
	return ((boxSize.x <= childSize.x) && (boxSize.y <= childSize.y) && (boxSize.z <= childSize.z));
}

void OctreeNode::getChildIndexes(const AxisAlignedBox &box, int *x, int *y, int *z)
{
	Vector3 selfCenter = box_.getCenter();
	Vector3 othCenter  = box.getCenter();

	*x = othCenter.x > selfCenter.x ? 1 : 0;
	*y = othCenter.y > selfCenter.y ? 1 : 0;
	*z = othCenter.z > selfCenter.z ? 1 : 0;
}

void OctreeNode::setBoundBox( const AxisAlignedBox& box )
{
	box_ = box;
	updateCullBoundBox();
}

void OctreeNode::setBoundBox( const Vector3& vmin, const Vector3& vmax )
{
	box_.setExtents( vmin, vmax );
	updateCullBoundBox();
}

void OctreeNode::updateCullBoundBox()
{
	// ��ɢ�˲��������������߸�������50%
	Vector3 halfSize = box_.getHalfSize();
	cull_box_.setExtents(box_.getMinimum() - halfSize, box_.getMaximum() + halfSize);
}

//////////////////////////////////////////////////////////////////////////
Octree::Octree()
: root_(0)
, max_depth_(0)
{

}

Octree::~Octree()
{
	clear();
}

void Octree::init(const AxisAlignedBox& box, int maxDepth)
{
	updateBox(box);
	updateMaxDepth(maxDepth);
}

void Octree::clear()
{
	ENN_SAFE_DELETE(root_);
}

void Octree::updateMaxDepth(int maxDepth)
{
	ENN_ASSERT(maxDepth > 0);

	if (maxDepth == max_depth_)
	{
		return;
	}

	if (maxDepth < max_depth_)
	{
		AxisAlignedBox box = root_->getBoundBox();

		ENN_SAFE_DELETE(root_);

		root_ = ENN_NEW OctreeNode();
		root_->setBoundBox(box);
	}

	max_depth_ = maxDepth;
}

void Octree::updateBox(const AxisAlignedBox& box)
{
	ENN_SAFE_DELETE(root_);
	root_ = ENN_NEW OctreeNode;
	root_->setBoundBox(box);
}

void Octree::updateObject(ObjectType* obj)
{
	const AxisAlignedBox& objBox = obj->getWorldAABB();
	OctreeNode* node = obj->getOctreeNode();

	// �״ι���
	if (!node)
	{
		// ���ڵ���������ݹ����
		if (root_->selfCanHold(objBox, true))
		{
			addObjectToNode(root_, obj, 0);
		}
		else
		{
			root_->addObject(obj);
		}

		return;
	}

	if (!node->selfCanHold(objBox, false))
	{
		removeObjectFromNode(obj);

		// ���²���
		// ���ڵ���������ݹ����
		if (root_->selfCanHold(objBox, true))
		{
			addObjectToNode(root_, obj, 0);
		}
		else
		{
			root_->addObject(obj);
		}
	}
}

void Octree::addObjectToNode(OctreeNode* node, ObjectType* obj, int depth)
{
	// ��һ��������뵽�˲���
	// ע��Ҫ��������������ڸ��ڵ㷶Χ��
	const AxisAlignedBox& objBox = obj->getWorldAABB();

	// ����˰˲����ڵ������С�Ǵ˶���2����Ҳ����˵���Լ������뵽���Ľڵ�
	// ͬʱ�˲��������û�й���
	if (depth < max_depth_ && node->childCanHoldBySize(objBox))
	{
		int x, y, z;
		node->getChildIndexes(objBox, &x, &y, &z);

		OctreeNode*& nodeChild = node->children_[x][y][z];

		if (!nodeChild)
		{
			nodeChild = ENN_NEW OctreeNode;
			nodeChild->parent_ = node;

			const Vector3& nodeMin = node->getBoundBox().getMinimum();
			const Vector3& nodeMax = node->getBoundBox().getMaximum();

			Vector3 childMin, childMax;

			if ( x == 0 )
			{
				childMin.x = nodeMin.x;
				childMax.x = (nodeMin.x + nodeMax.x) * 0.5f;
			}
			else
			{
				childMin.x = (nodeMin.x + nodeMax.x) * 0.5f;
				childMax.x = nodeMax.x;
			}

			if ( y == 0 )
			{
				childMin.y = nodeMin.y;
				childMax.y = (nodeMin.y + nodeMax.y) * 0.5f;
			}
			else
			{
				childMin.y = (nodeMin.y + nodeMax.y) * 0.5f;
				childMax.y = nodeMax.y;
			}

			if ( z == 0 )
			{
				childMin.z = nodeMin.z;
				childMax.z = (nodeMin.z + nodeMax.z) * 0.5f;
			}
			else
			{
				childMin.z = (nodeMin.z + nodeMax.z) * 0.5f;
				childMax.z = nodeMax.z;
			}

			nodeChild->setBoundBox( childMin, childMax );
		}

		++depth;
		addObjectToNode(nodeChild, obj, depth);
	}
	else
	{
		node->addObject(obj);
	}
}

void Octree::removeObjectFromNode(ObjectType* obj)
{
	OctreeNode* node = obj->getOctreeNode();

	if (node)
	{
		node->removeObject(obj);
	}
}

}