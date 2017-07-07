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
// Time: 2015/12/09
// File: enn_ps_emitter_cylinder.cpp
//

#include "enn_ps_emitter_cylinder.h"

namespace enn
{

PSEmitterClinder::PSEmitterClinder()
{

}

PSEmitterClinder::~PSEmitterClinder()
{

}

void PSEmitterClinder::initParticle(Particle* p)
{
	PSEmitter::initParticle(p);

	float3 xAxis(1, 0, 0), yAxis(0, 1, 0), zAxis(0, 0, 1);

	float alpha = Math::rangeRandom(0.0f, 2 * Math::PI);

	float s = Math::rangeRandom(inner_scale_, 1.0f);
	float x = s * Math::cos(alpha);
	float z = s * Math::sin(alpha);
	float y = Math::rangeRandom(-1.0f, 1.0f);

	p->position = position_ + (x * xAxis + y * yAxis + z * zAxis) * extend_;
}

void PSEmitterClinder::setInnerScale(float inner)
{
	inner_scale_ = inner;
}

float PSEmitterClinder::getInnerScale() const
{
	return inner_scale_;
}

}