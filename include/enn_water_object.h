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
// Time: 2015/09/23
// File: enn_water_object.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_d3d11_render_device.h"



namespace enn
{

/// The aerial fog settings.
struct FogData
{
	float density;
	float densityOffset;
	float atmosphereHeight;
	Color color;

	FogData()
	{
		density = 0.0f;
		densityOffset = 0.0f;
		atmosphereHeight = 0.0f;
		color.setRGB(0.5f, 0.5f, 0.5f);
	}
};


/// The water fog settings.
struct WaterFogData
{
	float density;
	float densityOffset;
	float wetDepth;
	float wetDarkening;
	Color color;
	Plane plane;
	float depthGradMax;

	WaterFogData()
	{
		density = 0.0f;
		densityOffset = 0.0f;
		wetDepth = 0.0f;
		wetDarkening = 0.0f;
		color.setRGB(0.5f, 0.5f, 0.5f);
		depthGradMax = 0.0f;
	}
};

enum WaterType
{
	Lake,
	Ocean,
	River
};

class SceneNode;
class WaterObject : public AllocatedObject
{
	friend class OceanEffectTemplate;

public:
	enum consts
	{
		MAX_WAVES = 3,
		MAX_FOAM = 2,
		NUM_ANIM_FRAMES = 32
	};

	struct WaterVertex
	{
		Vector3 vertex;
	};

public:
	WaterObject();
	virtual ~WaterObject();

	virtual bool isUnderWater(const Vector3& pt) const { return false; }
	virtual bool init();

	void setRippleTexName(const String& name);
	void setFoamTexName(const String& name);
	void setCubeTexName(const String& name);
	void setDepthGradientTexName(const String& name);
	void setAttachNode(SceneNode* node);
	SceneNode* getSceneNode() const { return attach_node_; }

	virtual void render() {}

	WaterType getWaterType() const { return water_type_; }
	virtual const AxisAlignedBox& getObjectAABB() const = 0;

protected:
	virtual void setupVBIB() {}
	virtual void initTextures();
	void initLakeTest();

protected:
	WaterType			water_type_;
	SceneNode*			attach_node_;
	float				fresnel_bias_;
	float				fresnel_power_;
	float				specular_power_;
	Color				specular_color_;

	float				distort_start_dist_;
	float				distort_end_dist_;
	float				distort_full_depth_;

	Vector2				ripple_dir_[MAX_WAVES];
	float				ripple_speed_[MAX_WAVES];
	Vector2				ripple_tex_scale_[MAX_WAVES];
	float				ripple_magnitude_[MAX_WAVES];

	float				overall_ripple_magnitude_;

	Vector2				wave_dir_[MAX_WAVES];
	float				wave_speed_[MAX_WAVES];
	float				wave_magnitude_[MAX_WAVES];

	float				overall_wave_magnitude_;

	Vector2				foam_dir_[MAX_WAVES];
	float				foam_speed_[MAX_WAVES];
	Vector2				foam_tex_scale_[MAX_WAVES];
	float				foam_opacity_[MAX_WAVES];

	float				overall_foam_opacity_;
	float				foam_max_depth_;
	float				foam_ambient_lerp_;
	float				foam_ripple_influence_;

	Color				under_water_color_;

	String				ripple_tex_name_;
	String				foam_tex_name_;
	String				cubemap_name_;
	String				depth_gradient_tex_name_;

	WaterFogData		water_fog_data_;

	float				depth_gradient_max_;
	float				reflectivity_;
	bool				generate_vb_;

	D3D11TextureObj*	ripple_tex_obj_;
	D3D11TextureObj*	foam_tex_obj_;
	D3D11TextureObj*	sky_map_obj_;
	D3D11TextureObj*	depth_gradient_tex_obj_;
};

}