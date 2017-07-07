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
// Time: 2014/03/14
// File: enn_material_manager.cpp
//

#include "enn_material_manager.h"

namespace enn
{

MaterialManager* g_mtr_mgr_ = 0;
//////////////////////////////////////////////////////////////////////////
MaterialResAutoCreator::MaterialResAutoCreator()
{
	m_basePath = "Material";
}

MaterialResAutoCreator::~MaterialResAutoCreator()
{

}

bool MaterialResAutoCreator::buildResource(Resource* res)
{
	MemoryBuffer* buff = static_cast<MemoryBuffer*>(res->getUserData());

	Material* mtr = static_cast<Material*>(res);

	return mtr->load(*buff);
}

bool MaterialResAutoCreator::prepareResourceImpl(Resource* res, MemoryBuffer* buff)
{
	ENN_ASSERT(buff);

	if (!buff)
	{
		return false;
	}

	res->setUserData(buff);

	return true;
}

//////////////////////////////////////////////////////////////////////////
MaterialManager::MaterialManager()
{
	g_mtr_mgr_ = this;

	ResourceManager::init();
	setAutoCreator(ENN_NEW MaterialResAutoCreator());
}

MaterialManager::~MaterialManager()
{

}

void MaterialManager::init()
{

}

Resource* MaterialManager::createImpl()
{
	Resource* res = ENN_NEW Material;
	res->setManager(this);

	return res;
}

void MaterialManager::save(Material* mtr, const String& name)
{
	const String& root_path = g_file_system_->getRootPath();

	String mtr_full_name = root_path + "/Material/" + name;

	File mtr_file(mtr_full_name, FILE_WRITE);

	mtr->save(mtr_file);
}

}