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
// Time: 2014/02/16
// File: enn_node_def.h

#pragma once


namespace enn
{

enum NodeDirtyFlags
{
	SELF_TRANSFORM_DIRTY = 0,
	PARENT_TRANSFORM_DIRTY,
	DF_WORLD_AABB,
	DF_OCTREE_POS,
	DF_LIGHT_INFO,
};

enum NodeFeatureFlags
{
	SHADOW_ENABLE_CAST = 0,
	SHADOW_ENABLE_RECEIVE
};

enum NodeTypeFlags
{
	NODE_SCENE = 0,
	NODE_RENDERABLE,
	NODE_CAMERA,
	NODE_LIGHT,
	NODE_AREA,
	NODE_MESH,
	NODE_WATER,
	NODE_ENV_PROBE,
	NODE_TERRAIN_SECTION,
};

}