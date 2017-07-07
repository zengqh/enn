/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_frustum.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_math.h"

namespace enn
{

class Frustum : public AllocatedObject
{
public:
	enum ProjectionType
	{
		PT_ORTHOGRAPHIC,
		PT_PERSPECTIVE
	};

	enum FrustumPlane
	{
		PLANE_NEAR   = 0,
		PLANE_FAR    = 1,
		PLANE_LEFT   = 2,
		PLANE_RIGHT  = 3,
		PLANE_TOP    = 4,
		PLANE_BOTTOM = 5
	};

	enum FrustumCorner
	{
		CORNER_NEAR = 0,
		CORNER_FAR  = 4
	};

	enum Visibility
	{
		NONE,
		PARTIAL,
		FULL
	};

protected:
	static const uint32 DIRTY_VIEW       = 0;
	static const uint32 DIRTY_PROJECTION = 1;

public:
	Frustum();
	virtual ~Frustum();

public:
	void setPerspective( float fov, float aspect, float zNear, float zFar );
	void setPerspective2( float left, float top, float right, float bottom, float zNear, float zFar );
	void setPerspective2( float width, float height, float zNear, float zFar );

	void setOrtho( float left, float top, float right, float bottom, float zNear, float zFar );
	void setOrtho( float width, float height, float zNear, float zFar );

	void setTransform( const Vector3& eye, const Vector3& target, const Vector3& upDir );
	void setTransform( const Matrix4& matWorld );

	void setFov(float fov);
	void setAspect(float aspect);
	void setZNear(float n);
	void setZFar(float f);

public:
	ProjectionType getProjectionType() const { return m_projType; }

	float          getFov()            const { return m_fov; }
	float          getAspect()         const { return m_aspect; }

	float          getLeft()           const { return m_left; }
	float          getTop()            const { return m_top; }
	float          getRight()          const { return m_right; }
	float          getBottom()         const { return m_bottom; }

	float          getZNear()          const { return m_zNear; }
	float          getZFar()           const { return m_zFar; }

	const Matrix4& getWorldMatrix() const;
	const Matrix4& getViewMatrix() const;
	const Matrix4& getProjMatrix() const;
	const Matrix4& getViewProjMatrix() const;
	const Matrix4& getViewMatrixRD() const;
	const Matrix4& getProjMatrixRD() const;
	const Matrix4& getViewProjMatrixRD() const;

	const Vector3&          getEyePos() const;
	Vector3					getLookDir() const;
	Vector3					getUpDir() const;
	const Plane&            getPlane( FrustumPlane pla ) const;
	const Vector3*          getCorners() const;
	const AxisAlignedBox&   getAABB() const;

public:
	bool testVisibility( const AxisAlignedBox& bound ) const;
	Visibility testVisibilityEx( const AxisAlignedBox& bound ) const;

protected:
	void _setViewDirty();
	void _setProjectionDirty();
	void _update();
	void _calcPerspectiveParas();
	void _makePerspectiveProj();
	void _makeOrthoProj();
	void _makePlanes();
	void _makeCorners();

protected:
	// ͶӰ����
	ProjectionType m_projType;

	// ͶӰ����
	float m_fov;
	float m_aspect;

	float m_left;
	float m_top;
	float m_right;
	float m_bottom;

	float m_zNear;
	float m_zFar;

	// ����
	Matrix4 m_matWorld;
	Matrix4 m_matView;
	Matrix4 m_matProj;
	Matrix4 m_matViewProj;

	Matrix4 m_matViewRD; // ʵ���豸��view/proj������dx/gl�᲻ͬ
	Matrix4 m_matProjRD;
	Matrix4 m_matViewProjRD;

	// �۾�λ��(world�ռ�)
	Vector3 m_eyePos;

	// 8����(world�ռ�)
	Vector3 m_corners[8];

	// 6��ƽ��(world�ռ�)
	Plane m_planes[6];

	// ��Χ��(world�ռ�)
	AxisAlignedBox m_aabb;

	// �Ƿ���
	Flags32 m_dirty;
};

}