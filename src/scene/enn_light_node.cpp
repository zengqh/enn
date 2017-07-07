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
// Time: 2014/06/11
// File: enn_light_node.cpp

#include "enn_light_node.h"

namespace enn
{

LightNode::LightNode() : light_(0)
{
	type_flag_.set_flag(NODE_LIGHT);
}

LightNode::~LightNode()
{
	ENN_SAFE_DELETE(light_);
}

void LightNode::setLightType(LightType type)
{
	if (light_ && getLightType() != type)
	{
		ENN_SAFE_DELETE(light_);
	}

	if (!light_)
	{
		light_ = LightFactory::createLight(type);
		light_->setObjectListener(this);
	}
}

bool LightNode::intersects(const AxisAlignedBox& box) const
{
	const_cast<LightNode*>(this)->checkUpdateTransform();
	return light_->intersects(box);
}

float LightNode::squareDistance(const Vector3& pos) const
{
	const_cast<LightNode*>(this)->checkUpdateTransform();
	return light_->squaredDistance(pos);
}

bool LightNode::checkUpdateTransform()
{
	if (SceneNode::checkUpdateTransform())
	{
		light_->setTransform(derived_matrix_);
		return true;
	}

	return false;
}

void LightNode::checkUpdateWorldAABB()
{
	checkUpdateTransform();

	if (dirty_flag_.test_flag(DF_WORLD_AABB))
	{
		world_aabb_ = light_->getAABB();

		dirty_flag_.reset_flag(DF_WORLD_AABB);
	}
}

}