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
// File: enn_water_object.cpp
//

#include "enn_water_object.h"
#include "enn_scene_node.h"

namespace enn
{

WaterObject::WaterObject()
: fresnel_bias_(0.3f)
, fresnel_power_(6.0f)
, specular_color_(1.0f, 1.0f, 1.0f)
, distort_start_dist_(0.1f)
, distort_end_dist_(20.0f)
, distort_full_depth_(3.5f)
{
	for (int i = 0; i < MAX_WAVES; ++i)
	{
		ripple_dir_[i] = Vector2(0.0f, 0.0f);
		ripple_speed_[i] = 0.0f;
		ripple_tex_scale_[i] = Vector2(0.0f, 0.0f);

		wave_dir_[i] = Vector2(0.0f, 0.0f);
		wave_speed_[i] = 0.0f;
		wave_magnitude_[i] = 0.0f;
	}

	for (int i = 0; i < MAX_FOAM; ++i)
	{
		foam_dir_[i] = Vector2(0.0f, 0.0f);
		foam_speed_[i] = 0.0f;
		foam_tex_scale_[i] = Vector2(0.0f, 0.0f);
		foam_opacity_[i] = 0.0f;
	}

	foam_dir_[0] = Vector2(1, 0);
	foam_dir_[1] = Vector2(0, 1);
	foam_tex_scale_[0] = Vector2(1, 1);
	foam_tex_scale_[1] = Vector2(3, 3);

	ripple_magnitude_[0] = 1.0f;
	ripple_magnitude_[1] = 1.0f;
	ripple_magnitude_[2] = 0.3f;

	initLakeTest();

	generate_vb_ = true;
}

WaterObject::~WaterObject()
{

}

bool WaterObject::init()
{
	initTextures();

	return true;
}

void WaterObject::setRippleTexName(const String& name)
{
	ripple_tex_name_ = name;
}

void WaterObject::setFoamTexName(const String& name)
{
	foam_tex_name_ = name;
}

void WaterObject::setCubeTexName(const String& name)
{
	cubemap_name_ = name;
}

void WaterObject::setDepthGradientTexName(const String& name)
{
	depth_gradient_tex_name_ = name;
}

void WaterObject::setAttachNode(SceneNode* node)
{
	attach_node_ = node;
}

void WaterObject::initTextures()
{
	ripple_tex_obj_ = ENN_NEW D3D11TextureObj();
	ripple_tex_obj_->loadFile(ripple_tex_name_, false);

	foam_tex_obj_ = ENN_NEW D3D11TextureObj();
	foam_tex_obj_->loadFile(foam_tex_name_);

	sky_map_obj_ = ENN_NEW D3D11TextureObj();
	sky_map_obj_->loadFile(cubemap_name_);

	depth_gradient_tex_obj_ = ENN_NEW D3D11TextureObj();
	depth_gradient_tex_obj_->loadFile(depth_gradient_tex_name_);
}

void WaterObject::initLakeTest()
{
	fresnel_bias_ = 0.3f;
	fresnel_power_ = 6;
	specular_power_ = 48;
	specular_color_.setRGB(1, 1, 1);

	// wave
	wave_dir_[0].setValue(0, 0.7f);
	wave_speed_[0] = 1;
	wave_magnitude_[0] = 0.8f;

	wave_dir_[1].setValue(0.707f, 0.707f);
	wave_speed_[1] = 1;
	wave_magnitude_[1] = 0.3f;

	wave_dir_[2].setValue(0.7f, 0.86f);
	wave_speed_[2] = 1;
	wave_magnitude_[2] = 0.3f;

	overall_wave_magnitude_ = 0.1f;

	ripple_dir_[0].setValue(0, 1);
	ripple_speed_[0] = 0.12f;
	ripple_tex_scale_[0].setValue(2.14f, 2.14f);
	ripple_magnitude_[0] = 0.3f;

	ripple_dir_[1].setValue(1, 0);
	ripple_speed_[1] = 0.11f;
	ripple_tex_scale_[1].setValue(4.25f, 4.2f);
	ripple_magnitude_[1] = 0.4f;

	ripple_dir_[2].setValue(1, -0.5f);
	ripple_speed_[2] = 0.004f;
	ripple_tex_scale_[2].setValue(50, 50);
	ripple_magnitude_[2] = 0.3f;

	overall_ripple_magnitude_ = 1;
	overall_foam_opacity_ = 2;
	foam_max_depth_ = 1;
	foam_ambient_lerp_ = 0.5f;
	foam_ripple_influence_ = 0.05f;

	reflectivity_ = 0.8f;
	depth_gradient_max_ = 50;

	distort_start_dist_ = 0.1f;
	distort_full_depth_ = 3.5f;
	distort_end_dist_ = 20;

	water_fog_data_.density = 0.2f;
	water_fog_data_.densityOffset = 0;
}

}