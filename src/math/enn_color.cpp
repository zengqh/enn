/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_color.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_platform_headers.h"
#include "enn_color.h"
#include "enn_vector3.h"
#include "enn_vector4.h"

namespace enn
{
	const Color Color::ZERO  = Color( 0.0f, 0.0f, 0.0f, 0.0f );
	const Color Color::BLACK = Color( 0.0f, 0.0f, 0.0f, 1.0f );
	const Color Color::WHITE = Color( 1.0f, 1.0f, 1.0f, 1.0f );
	const Color Color::BLUE = Color( 0.0f, 0.0f, 1.0f, 1.0f );

	const float Color::COLOR_EPSILON = 1e-3f;

	uint32 Color::getAsRGBA() const
	{
		uint8 val8;
		uint32 val32 = 0;

		// Convert to 32bit pattern
		// (RGBA = 8888)

		// Red
		val8 = static_cast<uint8>(r * 255);
		val32 = val8 << 24;

		// Green
		val8 = static_cast<uint8>(g * 255);
		val32 += val8 << 16;

		// Blue
		val8 = static_cast<uint8>(b * 255);
		val32 += val8 << 8;

		// Alpha
		val8 = static_cast<uint8>(a * 255);
		val32 += val8;

		return val32;
	}

	uint32 Color::getAsARGB() const
	{
		uint8 val8;
		uint32 val32 = 0;

		// Convert to 32bit pattern
		// (ARGB = 8888)

		// Alpha
		val8 = static_cast<uint8>(a * 255);
		val32 = val8 << 24;

		// Red
		val8 = static_cast<uint8>(r * 255);
		val32 += val8 << 16;

		// Green
		val8 = static_cast<uint8>(g * 255);
		val32 += val8 << 8;

		// Blue
		val8 = static_cast<uint8>(b * 255);
		val32 += val8;

		return val32;
	}

	uint32 Color::getAsBGRA() const
	{
		uint8 val8;
		uint32 val32 = 0;

		// Convert to 32bit pattern
		// (ARGB = 8888)

		// Blue
		val8 = static_cast<uint8>(b * 255);
		val32 = val8 << 24;

		// Green
		val8 = static_cast<uint8>(g * 255);
		val32 += val8 << 16;

		// Red
		val8 = static_cast<uint8>(r * 255);
		val32 += val8 << 8;

		// Alpha
		val8 = static_cast<uint8>(a * 255);
		val32 += val8;

		return val32;
	}

	uint32 Color::getAsABGR() const
	{
		uint8 val8;
		uint32 val32 = 0;

		// Convert to 32bit pattern
		// (ABRG = 8888)

		// Alpha
		val8 = static_cast<uint8>(a * 255);
		val32 = val8 << 24;

		// Blue
		val8 = static_cast<uint8>(b * 255);
		val32 += val8 << 16;

		// Green
		val8 = static_cast<uint8>(g * 255);
		val32 += val8 << 8;

		// Red
		val8 = static_cast<uint8>(r * 255);
		val32 += val8;

		return val32;
	}

	Vector3 Color::getAsVector3() const
	{
		return Vector3(r, g, b);
	}

	Vector4 Color::getAsVector4() const
	{
		return Vector4(r, g, b, a);
	}

	void Color::setAsRGBA( uint32 val )
	{
		uint32 val32 = val;

		// Convert from 32bit pattern
		// (RGBA = 8888)

		// Red
		r = ((val32 >> 24) & 0xFF) / 255.0f;

		// Green
		g = ((val32 >> 16) & 0xFF) / 255.0f;

		// Blue
		b = ((val32 >> 8) & 0xFF) / 255.0f;

		// Alpha
		a = (val32 & 0xFF) / 255.0f;
	}

	void Color::setAsARGB( uint32 val )
	{
		uint32 val32 = val;

		// Convert from 32bit pattern
		// (ARGB = 8888)

		// Alpha
		a = ((val32 >> 24) & 0xFF) / 255.0f;

		// Red
		r = ((val32 >> 16) & 0xFF) / 255.0f;

		// Green
		g = ((val32 >> 8) & 0xFF) / 255.0f;

		// Blue
		b = (val32 & 0xFF) / 255.0f;
	}

	void Color::setAsBGRA( uint32 val )
	{
		uint32 val32 = val;

		// Convert from 32bit pattern
		// (ARGB = 8888)

		// Blue
		b = ((val32 >> 24) & 0xFF) / 255.0f;

		// Green
		g = ((val32 >> 16) & 0xFF) / 255.0f;

		// Red
		r = ((val32 >> 8) & 0xFF) / 255.0f;

		// Alpha
		a = (val32 & 0xFF) / 255.0f;
	}

	void Color::setAsABGR( uint32 val )
	{
		uint32 val32 = val;

		// Convert from 32bit pattern
		// (ABGR = 8888)

		// Alpha
		a = ((val32 >> 24) & 0xFF) / 255.0f;

		// Blue
		b = ((val32 >> 16) & 0xFF) / 255.0f;

		// Green
		g = ((val32 >> 8) & 0xFF) / 255.0f;

		// Red
		r = (val32 & 0xFF) / 255.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	const Color2 Color2::White = Color2(255, 255, 255, 255);
	const Color2 Color2::Red = Color2(255, 0, 0, 255);
	const Color2 Color2::Green = Color2(0, 255, 0, 255);
	const Color2 Color2::Blue = Color2(0, 0, 255, 255);
	const Color2 Color2::Black = Color2(0, 0, 0, 255);

	/* :) Color
	---------------------------------------------------------------------------------------
	@Remark:
	Color is 4 byte (clamp 0.0 - 1.0)
	---------------------------------------------------------------------------------------
	*/

	Color2::Color2()
	{
	}

	Color2::Color2(unsigned char c)
		: r(c), g(c), b(c), a(c)
	{
	}

	Color2::Color2(unsigned char c1, unsigned char c2, unsigned char c3)
		: r(c1), g(c2), b(c3), a(0xFF)
	{
	}

	Color2::Color2(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4)
		: r(c1), g(c2), b(c3), a(c4)
	{
	}

	Color2::Color2(const Color2 & c)
		: r(c.r), g(c.g), b(c.b), a(c.a)
	{
	}

	Color2::~Color2()
	{
	}


	Color2& Color2::operator =(const Color2 & c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;

		return *this;
	}


	bool Color2::operator ==(const Color2 & c) const
	{
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}

	bool Color2::operator !=(const Color2 & c) const
	{
		return r != c.r || g != c.g || b != c.b || a != c.a;
	}

	std::ostream & operator <<(std::ostream & os, const Color2 & c)
	{
		os << c.r << " " << c.g << " " << c.b << " " << c.a;
		return os;
	}
}