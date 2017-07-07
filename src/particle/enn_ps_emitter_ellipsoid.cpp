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
// File: enn_ps_emitter_ellipsoid.cpp
//

#include "enn_ps_emitter_ellipsoid.h"

namespace enn
{

PSEmitterEllipsoid::PSEmitterEllipsoid() : inner_scale_(0)
{

}

PSEmitterEllipsoid::~PSEmitterEllipsoid()
{

}

void PSEmitterEllipsoid::initParticle(Particle* p)
{
	PSEmitter::initParticle(p);

	float3 xAxis(1, 0, 0), yAxis(0, 1, 0), zAxis(0, 0, 1);

	float alpha = Math::rangeRandom(0.0f, 2 * Math::PI);
	float beta = Math::rangeRandom(0.0f, Math::PI);

	float s = Math::rangeRandom(inner_scale_, 1.0f);

	float sinbeta = Math::sin(beta);
	float x = s * Math::cos(alpha) * sinbeta;
	float y = s * Math::sin(alpha) * sinbeta;
	float z = s * Math::cos(beta);

	p->position = position_ + (x * xAxis + y * yAxis + z * zAxis) * extend_;
}

void PSEmitterEllipsoid::setInnerScale(float inner)
{
	inner_scale_ = inner;
}

float PSEmitterEllipsoid::getInnerScale() const
{
	return inner_scale_;
}

}