/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_axis_aligned_box.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_platform_headers.h"
#include "enn_axis_aligned_box.h"

namespace enn
{
	const AxisAlignedBox AxisAlignedBox::BOX_NULL;
	const AxisAlignedBox AxisAlignedBox::BOX_UNIT( Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f) );
	const AxisAlignedBox AxisAlignedBox::BOX_INFINITE(AxisAlignedBox::EXTENT_INFINITE);


	const AxisAlignedBox AxisAlignedBox::Invalid = AxisAlignedBox(FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
	const AxisAlignedBox AxisAlignedBox::Infinite = AxisAlignedBox(-FLT_MAX, -FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
	const AxisAlignedBox AxisAlignedBox::Zero = AxisAlignedBox(0, 0, 0, 0, 0, 0);
	const AxisAlignedBox AxisAlignedBox::Identiy = AxisAlignedBox(-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f);
	const AxisAlignedBox AxisAlignedBox::Default = AxisAlignedBox::Identiy;
}