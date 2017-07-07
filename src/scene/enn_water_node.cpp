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
// Time: 2015/10/29
// File: enn_water_node.cpp
//

#include "enn_water_node.h"
#include "enn_water_block.h"
#include "enn_water_plane.h"

namespace enn
{

WaterNode::WaterNode() : water_obj_(0)
{
	type_flag_.set_flag(NODE_WATER);
}

WaterNode::~WaterNode()
{

}

void WaterNode::createWater(WaterType type)
{
	ENN_ASSERT(!water_obj_);

	switch (type)
	{
	case enn::Lake:
		water_obj_ = ENN_NEW WaterBlock();
		break;
	case enn::Ocean:
		water_obj_ = ENN_NEW WaterPlane();
		break;
	case enn::River:
		break;
	default:
		break;
	}

	water_obj_->setAttachNode(this);
}

const AxisAlignedBox& WaterNode::getObjectAABB() const
{
	return water_obj_->getObjectAABB();
}

}