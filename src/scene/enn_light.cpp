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
// File: enn_light.cpp

#include "enn_light.h"
#include "enn_root.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
Light::Light() :
m_lightType(LT_DIRECTIONAL),
m_position(Vector3::ZERO),
m_direction(Vector3::NEGATIVE_UNIT_Y), m_up(Vector3::NEGATIVE_UNIT_Z), m_right(Vector3::UNIT_X),
m_diffuse(Color::WHITE)
{
	m_dirty.set_flag(DIRTY_PROPERTY);
}

Light::~Light()
{

}

void Light::setPosition(const Vector3& pos)
{
	m_position = pos;
	_setPropertyDirty();
}

void Light::setDirection(const Vector3& direction)
{
	m_direction = direction;
	_setPropertyDirty();
}

void Light::setUp(const Vector3& up)
{
	m_up = up;
	_setPropertyDirty();
}

const Vector3& Light::getPosition() const
{
	const_cast<Light*>(this)->_update();
	return m_position;
}

const Vector3& Light::getDirection() const
{
	const_cast<Light*>(this)->_update();
	return m_direction;
}

const Vector3& Light::getUp() const
{
	const_cast<Light*>(this)->_update();
	return m_up;
}

const Vector3& Light::getRight() const
{
	const_cast<Light*>(this)->_update();
	return m_right;
}

void Light::setDiffuse(const Color& diffuse)
{
	m_diffuse = diffuse;
}

const Color& Light::getDiffuse() const
{
	return m_diffuse;
}

void Light::setTransform(const Matrix4& matWorld)
{
	setPosition(matWorld.getTrans());
	setDirection(-matWorld.getAxisZ());
	setUp(matWorld.getAxisY());
}

const Matrix4& Light::getWorldMatrix() const
{
	const_cast<Light*>(this)->_update();
	return m_matWorld;
}

const AxisAlignedBox& Light::getAABB() const
{
	const_cast<Light*>(this)->_update();
	return m_aabb;
}

bool Light::intersects(const AxisAlignedBox& aabb) const
{
	return getAABB().intersects(aabb);
}

void Light::_update()
{
	if (m_dirty.test_flag(DIRTY_PROPERTY))
	{
		_checkLightProp();
		_calcAABB();
		m_dirty.reset_flag(DIRTY_PROPERTY);
	}
}

void Light::_setPropertyDirty()
{
	m_dirty.set_flag(DIRTY_PROPERTY);
	fireAABBDirty();
}

void Light::_checkLightProp()
{
	m_direction.normalise();
	m_right = m_direction.crossProduct(m_up);
	m_right.normalise();
	m_up = m_right.crossProduct(m_direction);
	m_matWorld.makeTransform(m_position, Vector3::UNIT_SCALE, Quaternion(m_right, m_up, -m_direction));
}

//////////////////////////////////////////////////////////////////////////
DirectLight::DirectLight()
{

}

DirectLight::~DirectLight()
{

}

float DirectLight::squaredDistance(const Vector3& pos) const
{
	return 0;
}

void DirectLight::_calcAABB()
{
	m_aabb.setInfinite();
}

//////////////////////////////////////////////////////////////////////////
PointLight::PointLight()
{
	m_lightType = LT_POINT;
}

PointLight::~PointLight()
{

}

void PointLight::setRange(float range)
{
	m_range = range;
	_setPropertyDirty();
}

void PointLight::setFadeDist(float fadeDist)
{
	m_fadeDist = fadeDist;
	_setPropertyDirty();
}

float PointLight::getRange()      const
{
	const_cast<PointLight*>(this)->_update();
	return m_range;
}

float PointLight::getFadeDist()   const
{
	const_cast<PointLight*>(this)->_update();
	return m_fadeDist;
}

float PointLight::_getFadeScale() const
{
	return Math::PI * m_fadeDist / m_range;
}

float PointLight::squaredDistance(const Vector3& pos) const
{
	return m_position.squaredDistance(pos);
}

Mesh* PointLight::getVolume() const
{
	return g_mesh_mgr_->getPointLitMesh();
}

void PointLight::_checkLightProp()
{
	Light::_checkLightProp();

	if (m_range < 0)
		m_range = 0;

	if (m_fadeDist < 0)
		m_fadeDist = 0;

	m_matWorld.makeTransform(m_position, Vector3(m_range), Quaternion::IDENTITY);
}

void PointLight::_calcAABB()
{
	Vector3 size(m_range, m_range, m_range);
	m_aabb.setExtents(m_position - size, m_position + size);
}

//////////////////////////////////////////////////////////////////////////
SpotLight::SpotLight()
{
	m_lightType = LT_SPOT;
}

SpotLight::~SpotLight()
{

}

void SpotLight::setInnerCone(float angle)
{
	m_innerCone = angle;
	_setPropertyDirty();
}

void SpotLight::setOuterCone(float angle)
{
	m_outerCone = angle;
	_setPropertyDirty();
}

float SpotLight::getInnerCone()   const
{
	const_cast<SpotLight*>(this)->_update();
	return m_innerCone;
}

float SpotLight::getOuterCone()   const
{
	const_cast<SpotLight*>(this)->_update();
	return m_outerCone;
}

const Vector2& SpotLight::_getSpotAngles() const
{
	return m_spotAngles;
}

float SpotLight::squaredDistance(const Vector3& pos) const
{
	return m_centerSpot.squaredDistance(pos);
}

Mesh* SpotLight::getVolume() const
{
	return g_mesh_mgr_->getSpotLitMesh();
}

void SpotLight::_checkLightProp()
{
	PointLight::_checkLightProp();

	m_outerCone = Math::clamp(m_outerCone, (float)0, Math::HALF_PI); // 由于是半角，最大90
	m_innerCone = Math::clamp(m_innerCone, (float)0, m_outerCone); // 内角不能超过外角
	m_outerRadius = m_range * Math::tan(m_outerCone); // 外圆的半径

	m_spotAngles.x = Math::cos(m_outerCone);
	m_spotAngles.y = 1.0f / (Math::cos(m_innerCone) - m_spotAngles.x);
}

void SpotLight::_calcAABB()
{
	// 聚光灯
	Vector3  zdir = -m_direction;
	Vector3& ydir = m_up;
	Vector3& xdir = m_right;

	_makeSpotAABB(m_position, xdir, ydir, zdir, m_outerRadius, m_range, m_centerSpot, m_aabb);
}

void SpotLight::_makeSpotAABB(const Vector3& pos, const Vector3& xdir, const Vector3& ydir, const Vector3& zdir,
	float radius, float range, Vector3& centerSpot, AxisAlignedBox& aabb)
{
	float halfRange = range * 0.5f;
	centerSpot = pos - zdir * halfRange;
	Vector3 halfSize(radius, radius, halfRange);

	Vector3 corners[8];
	ObjectAlignedBox::getCorners(centerSpot, xdir, ydir, zdir, halfSize, corners);

	aabb.setNull();
	aabb.merge(corners, 8, sizeof(Vector3));
}

//////////////////////////////////////////////////////////////////////////
Light* LightFactory::createLight(LightType type)
{
	switch (type)
	{
	case LT_DIRECTIONAL:
		return ENN_NEW DirectLight;

	case LT_POINT:
		return ENN_NEW PointLight;

	case LT_SPOT:
		return ENN_NEW SpotLight;

	default:
		ENN_ASSERT(0);
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
LightsInfo::LightsInfo()
{
	for (int i = 0; i < LT_MAX; ++i)
	{
		lit_list_[i] = 0;
	}
}

LightsInfo::~LightsInfo()
{
	for (int i = 0; i < LT_MAX; ++i)
	{
		ENN_SAFE_DELETE_T(lit_list_[i]);
	}
}

void LightsInfo::addLight(Light* lit, float dist, bool is_in_sm)
{
	LightList*& light_list = lit_list_[lit->getLightType()];

	if (!light_list)
	{
		light_list = ENN_NEW_T(LightList);
	}

	light_list->push_back(LightItem(lit, dist, is_in_sm));
}

void LightsInfo::clear()
{
	for (int i = 0; i < LT_MAX; ++i)
	{
		if (LightList* light_list = lit_list_[i])
		{
			light_list->clear();
		}
	}
}

void LightsInfo::sort_n(int light_type, int max_n)
{
	LightList* light_list = lit_list_[light_type];

	std::sort(light_list->begin(), light_list->end(), sortByDist);
}

int LightsInfo::getLightsCount(int type) const
{
	LightList* light_list = lit_list_[type];

	if (light_list)
	{
		return light_list->size();
	}

	return 0;
}

LightsInfo::LightList* LightsInfo::getLightList(int type) const
{
	return lit_list_[type];
}

}