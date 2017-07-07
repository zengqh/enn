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
// Time: 2015/12/03
// File: enn_particle.h
//

#pragma once

#define PS_MAX_RENDER 4
#define PS_MAX_EMITTER 6
#define PS_MAX_MODIFIER 6
#define PS_PARTICLE_PARAM_SIZE 4

namespace enn
{

class PSEmitter;
struct Particle
{
	PSEmitter*	emitter;
	float3		position;
	float3		direction;
	Quaternion	rotation;
	float3		size;
	Color		color;
	Rectanglef	uvrect;
	float		speed;
	float		life;
	float		maxlife;
	Field<IntFloatUnion, PS_PARTICLE_PARAM_SIZE> param;
	void*		interal_data;
};

}