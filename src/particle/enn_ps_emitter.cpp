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
// Time: 2015/12/02
// File: enn_ps_emitter.cpp
//

#include "enn_ps_emitter.h"
#include "enn_particle.h"

namespace enn
{

PSEmitter::PSEmitter()
: render_(0)
, enabled_(true)
, position_(float3::ZERO)
, direction_(float3::UNIT_Y)
, common_direction_(float3::UNIT_X)
, min_color_(1, 1, 1)
, max_color_(1, 1, 1)
, angle_(0, 180)
, life_(2, 4)
, speed_(0.2f, 0.4f)
, dimension_(0.2f, 0.4f)
, uv_rect_(1, 1)
, uvrect_step_(1, 1)
, rate_(10)
, duration_time_(0)
, internal_time_(0)
, last_emit_time_(0)
, emit_count_(0)
{

}

PSEmitter::~PSEmitter()
{

}

void PSEmitter::init(PSRender* render)
{
	render_ = render;
}

PSRender* PSEmitter::getRender() const
{
	return render_;
}

void PSEmitter::setEnable(bool en)
{
	enabled_ = en;

	last_emit_time_ = 0;
	internal_time_ = 0;
	emit_count_ = 0;
}

bool PSEmitter::isEnable() const
{
	return enabled_;
}

void PSEmitter::setRate(float rate)
{
	rate_ = rate;
}

float PSEmitter::getRate() const
{
	return rate_;
}

void PSEmitter::setDurationTime(float time)
{
	duration_time_ = time;
	internal_time_ = 0;
	last_emit_time_ = 0;
}

float PSEmitter::getDurationTime() const
{
	return duration_time_;
}

void PSEmitter::setPosition(const float3& pos)
{
	position_ = pos;
}

const float3 PSEmitter::getPosition() const
{
	return position_;
}

void PSEmitter::setRotation(const Quaternion& rot)
{
	rotation_ = rot;

	direction_ = rotation_ * float3::UNIT_Y;
	common_direction_ = rotation_ * float3::UNIT_X;
}

void PSEmitter::setDirection(const float3& dir, const float3& comm_dir)
{
	direction_ = dir;
	common_direction_ = comm_dir;
}

const float3& PSEmitter::getDirection() const
{
	return direction_;
}

const float3& PSEmitter::getCommonDirection() const
{
	return common_direction_;
}

void PSEmitter::setColor(const Color& _min, const Color& _max)
{
	min_color_ = _min;
	max_color_ = _max;
}

const Color& PSEmitter::getMinColor() const
{
	return min_color_;
}

const Color& PSEmitter::getMaxColor() const
{
	return max_color_;
}

void PSEmitter::setAngle(const float2& v)
{
	angle_ = v;
}

const float2 PSEmitter::getAngle() const
{
	return angle_;
}

void PSEmitter::setLife(const float2& v)
{
	life_ = v;
}

const float2& PSEmitter::getLife() const
{
	return life_;
}

void PSEmitter::setSpeed(const float2& v)
{
	speed_ = v;
}

const float2& PSEmitter::getSpeed() const
{
	return speed_;
}

void PSEmitter::setDimension(const float2& v)
{
	dimension_ = v;
}

const float2& PSEmitter::getDimension() const
{
	return dimension_;
}

void PSEmitter::setSize(const float3& min_size, const float3& max_size)
{
	min_size_ = min_size;
	max_size_ = max_size;
}

const float3& PSEmitter::getMinSize() const
{
	return min_size_;
}

const float3& PSEmitter::getMaxSize() const
{
	return max_size_;
}

void PSEmitter::setRotation(const Quaternion& min_rot, const Quaternion& max_rot)
{
	min_rotation_ = min_rot;
	max_rotation_ = max_rot;
}

const Quaternion& PSEmitter::getMinRotation() const
{
	return min_rotation_;
}

const Quaternion& PSEmitter::getMaxRotation() const
{
	return max_rotation_;
}

void PSEmitter::setUVRect(const IntVector2& xy)
{
	uv_rect_.x_ = Math::maxVal(1, xy.x_);
	uv_rect_.y_ = Math::maxVal(1, xy.y_);

	uvrect_step_.x = 1.0f / uv_rect_.x_;
	uvrect_step_.y = 1.0f / uv_rect_.y_;
}

const IntVector2& PSEmitter::getUVRect() const
{
	return uv_rect_;
}

Rectanglef PSEmitter::getUVRect(int x, int y) const
{
	ENN_ASSERT(x < uv_rect_.x_ && y < uv_rect_.y_);

	Rectanglef rect(x * uvrect_step_.x,
		y * uvrect_step_.y, uvrect_step_.x, uvrect_step_.y);

	return rect;
}

Rectanglef PSEmitter::getUVRect(int index) const
{
	int y = index / uv_rect_.x_;
	int x = index % uv_rect_.x_;

	return getUVRect(x, y);
}

int PSEmitter::getUVRectCount() const
{
	return uv_rect_.x_ * uv_rect_.y_;
}

int PSEmitter::getEmitCount() const
{
	return emit_count_;
}

void PSEmitter::initParticle(Particle* p)
{
	p->emitter = this;
	p->color = randomColor();
	p->direction = randomDirection();
	p->uvrect = randomUVRect();
	p->size = randomSize();
	p->speed = randomSpeed();
	p->rotation = randomRotation();
	p->life = randomLife();
	p->maxlife = p->life;

	p->param[0].v_int = 0;
	p->param[1].v_int = 0;
	p->param[2].v_int = 0;
	p->param[3].v_int = 0;
}

bool PSEmitter::emit(float elapsed_time)
{
	emit_count_ = 0;
	internal_time_ += elapsed_time;

	if (duration_time_ != 0 && internal_time_ > duration_time_)
	{
		return false;
	}


	float time = internal_time_ - last_emit_time_;
	float emit_time = 1.0f / rate_;

	while (time > emit_time)
	{
		last_emit_time_ = internal_time_;
		emit_count_ += 1;
		time -= emit_time;
	}

	return emit_count_ > 0;
}

float3 PSEmitter::randomDirection()
{
	float rads = Math::rangeRandom(angle_.x, angle_.y);
	rads = Math::toRadians(rads);

	Quaternion quat;
	quat.fromAngleAxis(rads, common_direction_);

	float3 dir = quat * direction_;
	rads = Math::rangeRandom(0, Math::TWO_PI);
	quat.fromAngleAxis(rads, direction_);

	dir = quat * dir;


	return dir;
}

Quaternion PSEmitter::randomRotation()
{
	return Quaternion::nlerp(Math::unitRandom(), min_rotation_, max_rotation_, true);
}

float3 PSEmitter::randomSize()
{
	if (dimension_.x != 0 || dimension_.y != 0)
	{
		float d = Math::rangeRandom(dimension_.x, dimension_.y);

		return float3(d,d,d);
	}
	else
	{
		float x = Math::rangeRandom(min_size_.x, max_size_.x);
		float y = Math::rangeRandom(min_size_.y, max_size_.y);
		float z = Math::rangeRandom(min_size_.z, max_size_.z);

		return float3(x, y, z);
	}

}

Color PSEmitter::randomColor()
{
	Color color;

	color.r = Math::rangeRandom(min_color_.r, max_color_.r);
	color.g = Math::rangeRandom(min_color_.g, max_color_.g);
	color.b = Math::rangeRandom(min_color_.b, max_color_.b);
	color.a = Math::rangeRandom(min_color_.a, max_color_.a);

	return color;
}

Rectanglef PSEmitter::randomUVRect()
{
	int x = Math::rangeRandom(0, uv_rect_.x_ - 1);
	int y = Math::rangeRandom(0, uv_rect_.y_ - 1);

	return getUVRect(x, y);
}

float PSEmitter::randomSpeed()
{
	return Math::rangeRandom(speed_.x, speed_.y);
}

float PSEmitter::randomLife()
{
	if (life_.x != 0 || life_.y != 0)
	{
		return Math::rangeRandom(speed_.x, speed_.y);
	}
	else
	{
		return Math::ENN_MAX_FLOAT;
	}
}

}