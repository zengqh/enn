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
// File: enn_skeleton.h
//

#pragma once

#include "enn_node.h"
#include "enn_animation.h"

namespace enn
{

#define MAX_BONES_NUM		(60)
//////////////////////////////////////////////////////////////////////////
class BoneNode : public Node
{
public:
	BoneNode(const String& name, int id);
	~BoneNode();

	int getId() const { return id_; }

public:
	const vec3f& getInitPos() const { return init_pos_; }
	const Quaternion& getInitRot() const { return init_rot_; }
	const vec3f& getInitScale() const { return init_scale_; }

	void setInitPos(const vec3f& pos);
	void setInitRot(const Quaternion& rot);
	void setInitScale(const vec3f& sca);
	void resetSelf();
	void resetAll();

public:
	Matrix4& getOffsetMatrix() { return offset_matrix_; }
	const Matrix4& getOffsetMatrix() const { return offset_matrix_; }

protected:
	int					id_;

	vec3f				init_pos_;
	Quaternion			init_rot_;
	vec3f				init_scale_;

	Matrix4				offset_matrix_;
};

//////////////////////////////////////////////////////////////////////////
class Skeleton : public AllocatedObject
{
public:
	typedef vector<BoneNode*>::type              BoneNodeArray;
	typedef map<std::string, Animation*>::type   AnimationMap;
	typedef vector<int>::type                    IndexMap;
	typedef vector<IndexMap*>::type              IndicesMap;

public:
	Skeleton();
	~Skeleton();

public:
	BoneNode* createBoneNode(const String& name, int id);
	int       getBoneNodeCount() const;
	BoneNode* getBoneNode(int id);
	void      clearAllBoneNodes();

protected:

};

}