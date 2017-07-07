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
// Time: 2014/06/20
// File: enn_light.h

#pragma once

#include "enn_scene_object.h"

namespace enn
{

enum LightType
{
	LT_DIRECTIONAL,
	LT_POINT,
	LT_SPOT,
	LT_MAX
};

class Mesh;
class Light : public SceneObject
{
	enum
	{
		DIRTY_PROPERTY
	};

public:
	Light();
	virtual ~Light();

public:
	LightType getLightType() const { return m_lightType; }

	void setPosition(const Vector3& pos);
	void setDirection(const Vector3& direction);
	void setUp(const Vector3& up);

	const Vector3& getPosition() const;
	const Vector3& getDirection() const;
	const Vector3& getUp() const;
	const Vector3& getRight() const;

	void setDiffuse(const Color& diffuse);
	const Color& getDiffuse() const;

public:
	void setTransform(const Matrix4& matWorld);

	const Matrix4& getWorldMatrix() const;
	const AxisAlignedBox& getAABB() const; // ·µ»Øworld¿Õ¼äaabb

	bool  intersects(const AxisAlignedBox& aabb) const;
	virtual float squaredDistance(const Vector3& pos) const = 0;
	virtual Mesh* getVolume() const { return 0; }

protected:
	void _update();
	void _setPropertyDirty();

	virtual void _checkLightProp();
	virtual void _calcAABB() = 0;

protected:
	LightType       m_lightType;

	Vector3         m_position;
	Vector3         m_direction;
	Vector3         m_up;
	Vector3         m_right;

	Color           m_diffuse;

	Matrix4         m_matWorld;
	AxisAlignedBox  m_aabb;

	Flags32         m_dirty;
};

//////////////////////////////////////////////////////////////////////////
class DirectLight : public Light
{
public:
	DirectLight();
	virtual ~DirectLight();

public:
	virtual float squaredDistance(const Vector3& pos) const;

protected:
	virtual void _calcAABB();
};

//////////////////////////////////////////////////////////////////////////
class PointLight : public Light
{
public:
	PointLight();
	virtual ~PointLight();

	void setRange(float range);
	void setFadeDist(float fadeDist);

	float getRange()      const;
	float getFadeDist()   const;
	float _getFadeScale() const;

public:
	virtual float squaredDistance(const Vector3& pos) const;
	virtual Mesh* getVolume() const;

protected:
	virtual void _checkLightProp();
	virtual void _calcAABB();

protected:
	float m_range;    // point/spot range 
	float m_fadeDist; // point/spot fade distance
	AxisAlignedBox m_volAABB;
};

//////////////////////////////////////////////////////////////////////////
class SpotLight : public PointLight
{
public:
	SpotLight();
	virtual ~SpotLight();

	void setInnerCone(float angle);
	void setOuterCone(float angle);

	float          getInnerCone()   const;
	float          getOuterCone()   const;
	const Vector2& _getSpotAngles() const;

public:
	virtual float squaredDistance(const Vector3& pos) const;
	virtual Mesh* getVolume() const;

protected:
	virtual void _checkLightProp();
	virtual void _calcAABB();

	void _makeSpotAABB(const Vector3& pos, const Vector3& xdir, const Vector3& ydir, const Vector3& zdir,
		float radius, float range, Vector3& centerSpot, AxisAlignedBox& aabb);

protected:
	float    m_innerCone;    // spot inner cone(half)
	float    m_outerCone;    // spot outer cone(half)
	float    m_outerRadius;  // spot outer radius

	Vector3  m_centerSpot;
	Vector2  m_spotAngles;   // x = cos(outerCone) y = 1 / (cos(innerCone) - cos(outerCone))
};

//////////////////////////////////////////////////////////////////////////
class LightFactory
{
public:
	static Light* createLight(LightType type);
};

//////////////////////////////////////////////////////////////////////////
class LightsInfo : public AllocatedObject
{
public:
	struct LightItem : public AllocatedObject
	{
		LightItem() : lit(0), dist(0.0f), in_sm(false)
		{

		}

		LightItem(Light* l, float d, bool is_in_sm = false)
			: lit(l), dist(d), in_sm(is_in_sm)
		{

		}

		Light*		lit;
		float		dist;
		bool		in_sm;
	};

	typedef enn::vector<LightItem>::type LightList;

protected:
	static bool sortByDist(const LightItem& lhs, const LightItem& rhs)
	{
		return lhs.dist < rhs.dist;
	}

public:
	LightsInfo();
	~LightsInfo();

public:
	void addLight(Light* lit, float dist, bool is_in_sm = false);
	void clear();
	void sort_n(int light_type, int max_n);

	int getLightsCount(int type) const;
	LightList* getLightList(int type) const;

protected:
	LightList*			lit_list_[LT_MAX];
};

}