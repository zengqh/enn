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
// Time: 2014/12/21
// File: enn_environment.cpp
//

#include "enn_environment.h"

namespace enn
{
	
Environment* g_environment_ = 0;

Environment::Environment()
{
	g_environment_ = this;

	init();
}

Environment::~Environment()
{

}

Sky* Environment::getSky() const
{
	return sky_;
}

Terrain* Environment::getTerrain() const
{
	return terrain_;
}

void Environment::init()
{
	sky_ = ENN_NEW Sky();
}

void Environment::loadTerrain(const String& source)
{
	ENN_SAFE_DELETE(terrain_);

	terrain_ = ENN_NEW Terrain(source.c_str());
}

void Environment::destroy()
{

}

}