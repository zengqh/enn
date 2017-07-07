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
// File: enn_ps_emitter.h
//

#pragma once

#include "enn_math.h"

namespace enn
{

class PSRender;
class Particle;
class PSEmitter : public AllocatedObject
{
public:
	PSEmitter();
	virtual ~PSEmitter();

	virtual String getType() const = 0;

	void setName(const String& name) { name_ = name; }
	const String& getName() const { return name_; }
	

	virtual void init(PSRender* render);
	PSRender* getRender() const;

	void setEnable(bool en);
	bool isEnable() const;

	void setRate(float rate);
	float getRate() const;

	void setDurationTime(float time);
	float getDurationTime() const;

	void setPosition(const float3& pos);
	const float3 getPosition() const;

	void setRotation(const Quaternion& rot);
	void setDirection(const float3& dir, const float3& comm_dir);
	const float3& getDirection() const;
	const float3& getCommonDirection() const;

	void setColor(const Color& _min, const Color& _max);
	const Color& getMinColor() const;
	const Color& getMaxColor() const;

	void setAngle(const float2& v);
	const float2 getAngle() const;

	void setLife(const float2& v);
	const float2& getLife() const;

	void setSpeed(const float2& v);
	const float2& getSpeed() const;

	void setDimension(const float2& v);
	const float2& getDimension() const;

	void setSize(const float3& min_size, const float3& max_size);
	const float3& getMinSize() const;
	const float3& getMaxSize() const;

	void setRotation(const Quaternion& min_rot, const Quaternion& max_rot);
	const Quaternion& getMinRotation() const;
	const Quaternion& getMaxRotation() const;

	void setUVRect(const IntVector2& xy);
	const IntVector2& getUVRect() const;

	Rectanglef getUVRect(int x, int y) const;
	Rectanglef getUVRect(int index) const;

	int getUVRectCount() const;
	int getEmitCount() const;

	virtual void initParticle(Particle* p);
	bool emit(float elapsed_time);

	float3 randomDirection();
	Quaternion randomRotation();
	float3 randomSize();
	Color randomColor();
	Rectanglef randomUVRect();
	float randomSpeed();
	float randomLife();


protected:
	String			name_;
	bool			enabled_;
	float3			position_;
	Quaternion		rotation_;
	float3			direction_;
	float3			common_direction_;

	float			rate_;
	float			duration_time_;

	Color			min_color_, max_color_;
	float2			angle_;
	float2			life_;
	float2			speed_;
	float2			dimension_;
	float3			min_size_, max_size_;
	Quaternion		min_rotation_, max_rotation_;

	IntVector2		uv_rect_;

	/////////////////////////////////////////////////////////////
	PSRender*		render_;
	float2			uvrect_step_;
	float			internal_time_;
	float			last_emit_time_;
	int				emit_count_;
};

}
