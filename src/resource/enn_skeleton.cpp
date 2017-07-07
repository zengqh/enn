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
// Time: 2014/05/16
// File: enn_skeleton.cpp
//

#include "enn_skeleton.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
BoneNode::BoneNode(const String& name, int id) : id_(id)
{
	setName(name);
}

BoneNode::~BoneNode()
{

}

void BoneNode::setInitPos(const vec3f& pos)
{
	init_pos_ = pos;

	setPosition(pos);
}

void BoneNode::setInitRot(const Quaternion& rot)
{
	init_rot_ = rot;

	setOrientation(rot);
}

void BoneNode::setInitScale(const vec3f& sca)
{
	init_scale_ = sca;

	setScale(sca);
}

void BoneNode::resetSelf()
{
	setPosition(init_pos_);
	setOrientation(init_rot_);
	setScale(init_scale_);
}

void BoneNode::resetAll()
{
	resetSelf();

	ENN_FOR_EACH(NodeList, child_list_, it)
	{
		BoneNode* bn = static_cast<BoneNode*>(*it);
		bn->resetSelf();
	}
}

//////////////////////////////////////////////////////////////////////////

}