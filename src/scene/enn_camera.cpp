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
// Time: 2014/02/19
// File: enn_camera.cpp
//

#include "enn_camera.h"

namespace enn
{

Ray Camera::getScreenRay(float screenX, float screeny)
{
	Ray ret;
	getScreenRay(screenX, screeny, &ret);
	return ret;
}

void Camera::getScreenRay(float screenX, float screenY, Ray* outRay)
{
	Matrix4 inverseVP = (getProjMatrix() * getViewMatrix()).inverse();

	float nx = (2.0f * screenX) - 1.0f;
	float ny = 1.0f - (2.0f * screenY);
	Vector3 nearPoint(nx, ny, -1.f);

	// Use midPoint rather than far point to avoid issues with infinite projection
	Vector3 midPoint (nx, ny,  0.0f);

	// Get ray origin and ray target on near plane in world space
	Vector3 rayOrigin, rayTarget;

	rayOrigin = inverseVP * nearPoint;
	rayTarget = inverseVP * midPoint;

	Vector3 rayDirection = rayTarget - rayOrigin;
	rayDirection.normalise();

	outRay->setOrigin(rayOrigin);
	outRay->setDirection(rayDirection);
}

bool Camera::screenToWorldPositionClipGround(float screenX, float screenY, 
	Vector3& out_pos)
{
	Ray ray;
	getScreenRay(screenX, screenY, &ray);

	Plane plane;
	plane.redefine(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), 
		Vector3(0.0f, 0.0f, 1.0f));

	std::pair<bool, float> intersect_result = ray.intersects(plane);

	if (intersect_result.first)
	{
		out_pos = ray.getOrigin() + intersect_result.second * ray.getDirection();
		return true;
	}
	else
	{
		return false;
	}
}

Vector2 Camera::worldPositionToScreen(const Vector3& world_pos)
{
	Vector4 world_pos_4(world_pos, 1.0f);

	// clip space
	Vector4 clip_pos = getProjMatrix() * getViewMatrix() * world_pos_4;

	// normalized space
	Vector2 normalized_pos;
	normalized_pos.x = clip_pos.x / clip_pos.w;
	normalized_pos.y = clip_pos.y / clip_pos.w;

	Vector2 ret;

	ret.x = (normalized_pos.x + 1.0f) * 0.5f;
	ret.y = (1.0f - normalized_pos.y) * 0.5f;

	return ret;
}

}