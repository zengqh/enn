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
// Time: 2014/06/19
// File: hello_sample.cpp

#include "stdafx.h"
#include "hello_sample.h"

//////////////////////////////////////////////////////////////////////////
namespace enn
{

HelloSample::HelloSample()
{
	
}

HelloSample::~HelloSample()
{

}

bool HelloSample::onCreateScene()
{
	SceneGraph* sg = g_scene_graph_;

	AreaNode* area = sg->getRootNode();

	float h = 10000.0f;
	AxisAlignedBox aabb(-h, -h, -h, h, h, h);
	area->getArea()->initOctree(aabb, 8);

	// create material
	MaterialPtr mtrComm(g_mtr_mgr_->createMaterial("mtr_comm"));
	mtrComm->setNullCreator();
	mtrComm->setDiffuse(Color(0.6f, 0.6f, 0.6f, 1.0f));
	mtrComm->setSpecularShiness(Color(0.6f, 0.6f, 0.1f, 40.0f));
	mtrComm->useTexDiff()->setTexName("wall.jpg");
	mtrComm->enableLighting(true);

	MaterialPtr mtrDiff(g_mtr_mgr_->createMaterial("mtr_diff"));
	mtrDiff->setNullCreator();
	mtrDiff->setDiffuse(Color(0.88f, 0.78f, 0.6f, 1.0f));
	mtrDiff->useTexDiff()->setTexName("grass.jpg");
	mtrDiff->enableLighting(true);

	// create mesh
	MeshPtr meshPlane(g_mesh_mgr_->createPlane("mesh_plane", 100, 100, 3, 3));
	meshPlane->load(false);
	meshPlane->setMaterialName("mtr_diff");

	MeshPtr meshCube(g_mesh_mgr_->createCube("mesh_cube", 3.0f));
	meshCube->load(false);
	meshCube->setMaterialName("mtr_comm");

	MeshPtr meshSphere(g_mesh_mgr_->createSphere("mesh_sphere", 4.0f, 30));
	meshSphere->load(false);
	meshSphere->setMaterialName("mtr_comm");

	MeshPtr meshBoblamp(g_mesh_mgr_->createMesh("boblamp.mesh"));
	meshBoblamp->load(false);

	MeshPtr meshSponza(g_mesh_mgr_->createMesh("sponza.mesh"));
	meshSponza->load(false);

	// create mesh node
	MeshNode* objPlane = sg->createSceneNode<MeshNode>("objPlane");
	objPlane->setMesh("mesh_plane");
	objPlane->setPosition(0, -2, 0);
	objPlane->setScale(4, 1, 4);
	area->addChild(objPlane);

	MeshNode* objCube = sg->createSceneNode<MeshNode>("objCube");
	objCube->setMesh("mesh_cube");
	objCube->setPosition(-5, 0, 0);
	area->addChild(objCube);

	MeshNode* objSphere = sg->createSceneNode<MeshNode>("objSphere");
	objSphere->setMesh("mesh_sphere");
	objSphere->setPosition(5, 5, 0);
	area->addChild(objSphere);

	
	MeshNode* objBoblamp = sg->createSceneNode<MeshNode>("objBoblamp");
	objBoblamp->setMesh("boblamp.mesh");
	objBoblamp->setPosition(0, 0, 5);
	objBoblamp->setScale(0.1f, 0.1f, 0.1f);
	area->addChild(objBoblamp);

	MeshNode* objSponza = sg->createSceneNode<MeshNode>("objSponza");
	objSponza->setMesh("sponza.mesh");
	objSponza->setPosition(0, 0, 0);
	objSponza->setScale(0.1f, 0.1f, 0.1f);
	area->addChild(objSponza);

	LightNode* point_lit_node = sg->createSceneNode<LightNode>("point_lit0");
	point_lit_node->setLightType(LT_POINT);
	PointLight* point_lit0 = point_lit_node->getLightAs<PointLight>();
	point_lit0->setDiffuse(Color(0.8f, 0.1f, 0.13f));
	point_lit_node->setPosition(Vector3(0, 5, 0));
	point_lit0->setRange(20);
	point_lit0->setFadeDist(1);
	area->addChild(point_lit_node);

	LightNode* point_lit_node1 = sg->createSceneNode<LightNode>("point_lit1");
	point_lit_node1->setLightType(LT_POINT);
	PointLight* point_lit1 = point_lit_node1->getLightAs<PointLight>();
	point_lit1->setDiffuse(Color(0.8f, 0.8f, 0.83f));
	point_lit_node1->setPosition(Vector3(20, 20, 0));
	point_lit1->setRange(100);
	point_lit1->setFadeDist(1);
	area->addChild(point_lit_node1);

	LightNode* point_lit_node2 = sg->createSceneNode<LightNode>("point_lit2");
	point_lit_node2->setLightType(LT_POINT);
	PointLight* point_lit2 = point_lit_node2->getLightAs<PointLight>();
	point_lit2->setDiffuse(Color(0.0f, 0.8f, 0.9f));
	point_lit_node2->setPosition(Vector3(100, 20, 0));
	point_lit2->setRange(50);
	point_lit2->setFadeDist(1);
	area->addChild(point_lit_node2);

	LightNode* dir_lit_node = sg->createSceneNode<LightNode>("dir_lit1");
	dir_lit_node->setLightType(LT_DIRECTIONAL);
	DirectLight* dir_lit1 = dir_lit_node->getLightAs<DirectLight>();
	dir_lit1->setDiffuse(Color(0.1f, 0.15f, 0.03f));
	dir_lit_node->look_at(Vector3(0, 0, 0), Vector3(-1, -1, -1), Vector3::UNIT_Y);
	area->addChild(dir_lit_node);

	// camera
	camera_node_ = sg->createSceneNode<CameraNode>("main_cam");
	camera_node_->look_at(Vector3(0, 10.6f, 20), Vector3(0, 0, 0), Vector3::UNIT_Y);
	camera_node_->getCamera()->setPerspective(Math::toRadians(45.0f), 4.0f / 3.0f, 2.0f, 2000.0f);
	area->addChild(camera_node_);

	// load terrain
	g_environment_->loadTerrain("scene/test001.terrain");

	// create water
	//////////////////////////////////////////////////////////////////////////
	WaterNode* water_node = sg->createSceneNode<WaterNode>("lake");
	water_node->createWater(enn::Ocean);

	WaterPlane* lake = (WaterPlane*)water_node->getWaterObject();

	lake->setCubeTexName("texture/water/sky02.dds");
	lake->setDepthGradientTexName("texture/water/depthcolor_ramp.png");
	lake->setFoamTexName("texture/water/foam.dds");
	lake->setRippleTexName("texture/water/ripple.dds");

	lake->setWaterBound(100, 3);
	lake->init();

	water_node->setPosition(512, 20, 512);

	area->addChild(water_node);

	g_root_->addActiveCamera(camera_node_);

	return true;
}

void HelloSample::resize(int w, int h)
{
	SampleFrame::resize(w, h);

	camera_node_->getCamera()->setPerspective(Math::toRadians(45.0f), w / (float)h, 2.0f, 2000.0f);
}

void HelloSample::createSampleEventHandler()
{
	evt_handler_ = ENN_NEW SampleEventHandler();
	evt_handler_->init(camera_node_, 122.0f, 0.01f, (HWND)hwnd_);
}

bool HelloSample::onUpdate()
{
	MeshNode* objCube = static_cast<MeshNode*>(g_scene_graph_->getSceneNode("objCube"));
	MeshNode* objBob = static_cast<MeshNode*>(g_scene_graph_->getSceneNode("objBoblamp"));

	float elapsed_time = g_time_system_->getElapsedTime();

	objCube->yaw(Math::toRadians(elapsed_time * 100));
	objBob->yaw(Math::toRadians(elapsed_time * 70));

	return SampleFrame::onUpdate();
}

}