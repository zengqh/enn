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
// Time: 2014/07/23
// File: enn_mesh_exporter.cpp

#include "enn_root.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

using namespace enn;

namespace enn
{

//////////////////////////////////////////////////////////////////////////
struct OutModel
{
	OutModel() :
	rootBone_(0),
	totalVertices_(0),
	totalIndices_(0)
	{

	}

	String outName_;
	aiNode* rootNode_;
	set<unsigned>::type meshIndices_;
	vector<aiMesh*>::type meshes_;
	vector<aiNode*>::type meshNodes_;
	vector<aiNode*>::type bones_;
	vector<aiAnimation*>::type animations_;
	vector<float>::type boneRadii_;
	vector<AxisAlignedBox>::type boneHitboxes_;
	aiNode* rootBone_;
	unsigned totalVertices_;
	unsigned totalIndices_;
};

//////////////////////////////////////////////////////////////////////////
void createEnn();
void destroyEnn();

void run(const vector<String>::type& arguments);

void ExportModel(const String& outName, bool animationOnly);
void ExportMaterials(set<String>::type& usedTextures);
void CollectMeshes(OutModel& model, aiNode* node);
void CollectBones(OutModel& model, bool animationOnly = false);
void CollectBonesFinal(vector<aiNode*>::type& dest, const set<aiNode*>::type& necessary, aiNode* node);
void CollectAnimations(OutModel* model = 0);
void BuildBoneCollisionInfo(OutModel& model);
void BuildAndSaveModel(OutModel& model);
void BuildAndSaveAnimations(OutModel* model = 0);

unsigned GetNumValidFaces(aiMesh* mesh);
VertexElementsType GetElementMask(aiMesh* mesh);

void GetPosRotScale(const aiMatrix4x4& transform, Vector3& pos, Quaternion& rot, Vector3& scale);
aiMatrix4x4 GetMeshBakingTransform(aiNode* meshNode, aiNode* modelRootNode);
aiMatrix4x4 GetDerivedTransform(aiNode* node, aiNode* rootNode);
aiMatrix4x4 GetDerivedTransform(aiMatrix4x4 transform, aiNode* node, aiNode* rootNode);

Vector3 ToVector3(const aiVector3D& vec);
Vector2 ToVector2(const aiVector2D& vec);
Quaternion ToQuaternion(const aiQuaternion& quat);
String SanitateAssetName(const String& name);
String FromAIString(const aiString& str);

void WriteShortIndices(unsigned short*& dest, aiMesh* mesh, unsigned index, unsigned offset);
void WriteLargeIndices(unsigned*& dest, aiMesh* mesh, unsigned index, unsigned offset);

void WriteVertex(float*& dest, aiMesh* mesh, unsigned index, VertexElementsType elementMask, AxisAlignedBox& box,
	const Matrix4& vertexTransform, const Matrix3& normalTransform,
	enn::vector<enn::vector<uchar>::type>::type blend_indices,
	enn::vector<enn::vector<float>::type>::type blend_weight);

void GetBlendData(OutModel& model, aiMesh* mesh, enn::vector<uint32>::type& bone_mappings,
	enn::vector<enn::vector<uchar>::type>::type& blend_indices,
	enn::vector<enn::vector<float>::type>::type& blend_weights);

unsigned GetBoneIndex(OutModel& model, const String& boneName);

void BuildAndSaveMaterial(aiMaterial* material, set<String>::type& usedTextures);
String GenerateMaterialName(aiMaterial* material);
String GetMeshMaterialName(aiMesh* mesh);
//////////////////////////////////////////////////////////////////////////
const aiScene* scene_ = 0;
aiNode* rootNode_ = 0;
String inputName_;
String resourcePath_;
String outPath_;
bool useSubdirs_ = true;
bool localIDs_ = false;
bool saveBinary_ = false;
bool createZone_ = true;
bool noAnimations_ = false;
bool noHierarchy_ = false;
bool noMaterials_ = false;
bool noTextures_ = false;
bool noMaterialDiffuseColor_ = false;
bool saveMaterialList_ = false;
bool includeNonSkinningBones_ = false;
bool verboseLog_ = false;
bool emissiveAO_ = false;
bool noOverwriteMaterial_ = false;
bool noOverwriteTexture_ = false;
bool noOverwriteNewerTexture_ = false;
bool checkUniqueModel_ = true;
vector<String>::type nonSkinningBoneIncludes_;
vector<String>::type nonSkinningBoneExcludes_;

set<aiAnimation*>::type allAnimations_;
vector<aiAnimation*>::type sceneAnimations_;

float defaultTicksPerSecond_ = 4800.0f;

//////////////////////////////////////////////////////////////////////////
void createEnn()
{
	RootSetting root_setting;

	root_setting.clear_clr_ = enn::Color::BLUE;
	root_setting.file_system_path_ = "F:/ge/enn/media";
	root_setting.num_threads_ = 2;
	root_setting.render_setting_.vsync_enable_ = false;
	root_setting.render_setting_.full_screen_ = false;
	root_setting.render_setting_.width_ = 0;
	root_setting.render_setting_.height_ = 0;
	root_setting.render_setting_.sample_count_ = 1;
	root_setting.render_setting_.sample_quality_ = 0;
	root_setting.render_setting_.window_handle_ = 0;

	ENN_NEW enn::Root();
	g_root_->init(root_setting);
	g_root_->start_console();
}

void destroyEnn()
{
	//g_root_->shutdown();
}

void run(const vector<String>::type& arguments)
{
	if (arguments.size() < 2)
	{
		ErrorExit(
			"Usage: AssetImporter <command> <input file> <output file> [options]\n"
			"See http://assimp.sourceforge.net/main_features_formats.html for input formats\n\n"
			"Commands:\n"
			"model      Output a model\n"
			"\n"
			"Options:\n"
			"-t          Generate tangents\n"
			);
	}

	createEnn();

	unsigned flags = aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_LimitBoneWeights |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FixInfacingNormals |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords |
		aiProcess_FindInstances |
		aiProcess_FlipUVs;

	String command = arguments[0];
	String inFile = arguments[1];
	String outFile = arguments[2];

	for (unsigned i = 3; i < arguments.size(); ++i)
	{
		if (arguments[i].size() > 1 && arguments[i][0] == '-')
		{
			String argument = arguments[i].substr(1);

			if (argument == "t")
			{
				flags |= aiProcess_CalcTangentSpace;
			}
		}
	}

	scene_ = aiImportFile(inFile.c_str(), flags);
	if (!scene_)
	{
		ErrorExit("Could not open or parse input file " + inFile);
	}

	rootNode_ = scene_->mRootNode;
	
	if (command == "model")
	{
		ExportModel(outFile, false);

		set<String>::type usedTextures;
		ExportMaterials(usedTextures);
	}

	destroyEnn();
}

void ExportModel(const String& outName, bool animationOnly)
{
	if (outName.empty())
	{
		ErrorExit("No output file defined");
	}
		

	OutModel model;
	model.rootNode_ = rootNode_;
	model.outName_ = outName;

	CollectMeshes(model, model.rootNode_);
	CollectBones(model, animationOnly);
	BuildBoneCollisionInfo(model);
	BuildAndSaveModel(model);
	if (!noAnimations_)
	{
		CollectAnimations(&model);
		BuildAndSaveAnimations(&model);

		// Save scene-global animations
		CollectAnimations();
		BuildAndSaveAnimations();
	}
}

void ExportMaterials(set<String>::type& usedTextures)
{
	for (unsigned i = 0; i < scene_->mNumMaterials; ++i)
	{
		BuildAndSaveMaterial(scene_->mMaterials[i], usedTextures);
	}
}

void CollectMeshes(OutModel& model, aiNode* node)
{
	for (unsigned i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene_->mMeshes[node->mMeshes[i]];
		for (unsigned j = 0; j < model.meshes_.size(); ++j)
		{
			if (mesh == model.meshes_[j])
			{
				PrintLine("Warning: same mesh found multiple times");
				break;
			}
		}

		model.meshIndices_.insert(node->mMeshes[i]);
		model.meshes_.push_back(mesh);
		model.meshNodes_.push_back(node);
		model.totalVertices_ += mesh->mNumVertices;
		model.totalIndices_ += GetNumValidFaces(mesh) * 3;
	}

	for (unsigned i = 0; i < node->mNumChildren; ++i)
		CollectMeshes(model, node->mChildren[i]);
}

void CollectBones(OutModel& model, bool animationOnly)
{
	// impl later
	/*
	HashSet<aiNode*> necessary;
	HashSet<aiNode*> rootNodes;

	for (unsigned i = 0; i < model.meshes_.Size(); ++i)
	{
		aiMesh* mesh = model.meshes_[i];
		aiNode* meshNode = model.meshNodes_[i];
		aiNode* meshParentNode = meshNode->mParent;
		aiNode* rootNode = 0;

		for (unsigned j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			String boneName(FromAIString(bone->mName));
			aiNode* boneNode = GetNode(boneName, scene_->mRootNode, true);
			if (!boneNode)
				ErrorExit("Could not find scene node for bone " + boneName);
			necessary.Insert(boneNode);
			rootNode = boneNode;

			for (;;)
			{
				boneNode = boneNode->mParent;
				if (!boneNode || ((boneNode == meshNode || boneNode == meshParentNode) && !animationOnly))
					break;
				rootNode = boneNode;
				necessary.Insert(boneNode);
			}

			if (rootNodes.Find(rootNode) == rootNodes.End())
				rootNodes.Insert(rootNode);
		}
	}

	// If we find multiple root nodes, try to remedy by using their parent instead
	if (rootNodes.Size() > 1)
	{
		aiNode* commonParent = (*rootNodes.Begin())->mParent;
		for (HashSet<aiNode*>::Iterator i = rootNodes.Begin(); i != rootNodes.End(); ++i)
		{
			if (*i != commonParent)
			{
				if (!commonParent || (*i)->mParent != commonParent)
					ErrorExit("Skeleton with multiple root nodes found, not supported");
			}
		}
		rootNodes.Clear();
		rootNodes.Insert(commonParent);
		necessary.Insert(commonParent);
	}

	if (rootNodes.Empty())
		return;

	model.rootBone_ = *rootNodes.Begin();
	CollectBonesFinal(model.bones_, necessary, model.rootBone_);
	// Initialize the bone collision info
	model.boneRadii_.Resize(model.bones_.Size());
	model.boneHitboxes_.Resize(model.bones_.Size());
	for (unsigned i = 0; i < model.bones_.Size(); ++i)
	{
		model.boneRadii_[i] = 0.0f;
		model.boneHitboxes_[i] = BoundingBox(0.0f, 0.0f);
	}
	*/
}

void CollectBonesFinal(vector<aiNode*>::type& dest, const set<aiNode*>::type& necessary, aiNode* node)
{
	// impl later
	/*
	bool includeBone = necessary.Find(node) != necessary.End();
	String boneName = FromAIString(node->mName);

	// Check include/exclude filters for non-skinned bones
	if (!includeBone && includeNonSkinningBones_)
	{
		// If no includes specified, include by default but check for excludes
		if (nonSkinningBoneIncludes_.Empty())
			includeBone = true;

		// Check against includes/excludes
		for (unsigned i = 0; i < nonSkinningBoneIncludes_.Size(); ++i)
		{
			if (boneName.Contains(nonSkinningBoneIncludes_[i], false))
			{
				includeBone = true;
				break;
			}
		}
		for (unsigned i = 0; i < nonSkinningBoneExcludes_.Size(); ++i)
		{
			if (boneName.Contains(nonSkinningBoneExcludes_[i], false))
			{
				includeBone = false;
				break;
			}
		}

		if (includeBone)
			PrintLine("Including non-skinning bone " + boneName);
	}

	if (includeBone)
		dest.Push(node);

	for (unsigned i = 0; i < node->mNumChildren; ++i)
		CollectBonesFinal(dest, necessary, node->mChildren[i]);
		*/
}

void CollectAnimations(OutModel* model)
{
	/*
	const aiScene* scene = scene_;
	for (unsigned i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* anim = scene->mAnimations[i];
		if (allAnimations_.Contains(anim))
			continue;

		if (model)
		{
			bool modelBoneFound = false;
			for (unsigned j = 0; j < anim->mNumChannels; ++j)
			{
				aiNodeAnim* channel = anim->mChannels[j];
				String channelName = FromAIString(channel->mNodeName);
				if (GetBoneIndex(*model, channelName) != M_MAX_UNSIGNED)
				{
					modelBoneFound = true;
					break;
				}
			}
			if (modelBoneFound)
			{
				model->animations_.Push(anim);
				allAnimations_.Insert(anim);
			}
		}
		else
		{
			sceneAnimations_.Push(anim);
			allAnimations_.Insert(anim);
		}
	}
	*/
	/// \todo Vertex morphs are ignored for now
}

void BuildBoneCollisionInfo(OutModel& model)
{
	/*
	for (unsigned i = 0; i < model.meshes_.Size(); ++i)
	{
		aiMesh* mesh = model.meshes_[i];
		for (unsigned j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			String boneName = FromAIString(bone->mName);
			unsigned boneIndex = GetBoneIndex(model, boneName);
			if (boneIndex == M_MAX_UNSIGNED)
				continue;
			for (unsigned k = 0; k < bone->mNumWeights; ++k)
			{
				float weight = bone->mWeights[k].mWeight;
				// Require skinning weight to be sufficiently large before vertex contributes to bone hitbox
				if (weight > 0.33f)
				{
					aiVector3D vertexBoneSpace = bone->mOffsetMatrix * mesh->mVertices[bone->mWeights[k].mVertexId];
					Vector3 vertex = ToVector3(vertexBoneSpace);
					float radius = vertex.Length();
					if (radius > model.boneRadii_[boneIndex])
						model.boneRadii_[boneIndex] = radius;
					model.boneHitboxes_[boneIndex].Merge(vertex);
				}
			}
		}
	}
	*/
}

void BuildAndSaveModel(OutModel& model)
{
	if (!model.rootNode_) ErrorExit("Null root node for model");
	String rootNodeName = model.rootNode_->mName.C_Str();
	if (!model.meshes_.size())
	{
		ErrorExit("No geometries found starting from node " + rootNodeName);
	}
	
	Mesh* enn_mesh = ENN_NEW Mesh();
	AxisAlignedBox box;

	PrintLine("Writing model " + rootNodeName);

	for (unsigned i = 0; i < model.meshes_.size(); ++i)
	{
		aiMesh* mesh = model.meshes_[i];
		VertexElementsType elementMask = GetElementMask(mesh);
		unsigned validFaces = GetNumValidFaces(mesh);
		if (!validFaces)
		{
			continue;
		}
			
		bool largeIndices = mesh->mNumVertices > 65535;

		SubMesh* sub_mesh = enn_mesh->createNewSubMesh(i);
		sub_mesh->setName(mesh->mName.data);
		sub_mesh->setMaterialName(GetMeshMaterialName(mesh));
		sub_mesh->setPrimType(TT_TRIANGLE_LIST);

		sub_mesh->setVertexType(elementMask);
		sub_mesh->setVertexCount(mesh->mNumVertices);

		if (largeIndices)
		{
			sub_mesh->setIdxType(IDX_32);
			sub_mesh->setIndexCount(validFaces * 3);
		}
		else
		{
			sub_mesh->setIdxType(IDX_16);
			sub_mesh->setIndexCount(validFaces * 3);
		}

		// Get the world transform of the mesh for baking into the vertices
		Matrix4 vertexTransform;
		Matrix3 normalTransform;
		Vector3 pos, scale;
		Quaternion rot;

		GetPosRotScale(GetMeshBakingTransform(model.meshNodes_[i], model.rootNode_), pos, rot, scale);
		vertexTransform.makeTransform(pos, scale, rot);
		rot.toRotationMatrix(normalTransform);

		uchar* vertexData = (uchar*)sub_mesh->getVertexData();
		uchar* indexData = (uchar*)sub_mesh->getIndexData();

		// Build the index data
		if (!largeIndices)
		{
			unsigned short* dest = (unsigned short*)indexData;
			for (unsigned j = 0; j < mesh->mNumFaces; ++j)
				WriteShortIndices(dest, mesh, j, 0);
		}
		else
		{
			unsigned* dest = (unsigned*)indexData;
			for (unsigned j = 0; j < mesh->mNumFaces; ++j)
				WriteLargeIndices(dest, mesh, j, 0);
		}

		// Build the vertex data
		// If there are bones, get blend data
		float* dest = (float*)vertexData;
		enn::vector<enn::vector<uchar>::type>::type blend_indices;
		enn::vector<enn::vector<float>::type>::type blend_weights;
		enn::vector<uint32>::type bone_mappings;

		if (model.bones_.size())
		{
			GetBlendData(model, mesh, bone_mappings,
				blend_indices, blend_weights);
		}

		for (unsigned v = 0; v < mesh->mNumVertices; ++v)
		{
			WriteVertex(dest, mesh, v, elementMask, box,
				vertexTransform, normalTransform, blend_indices, blend_weights);
		}
		
	}

	enn_mesh->updateAABB(true);
	g_mesh_mgr_->saveMesh(enn_mesh, model.outName_);
}

void BuildAndSaveAnimations(OutModel* model)
{
	/*
	const PODVector<aiAnimation*>& animations = model ? model->animations_ : sceneAnimations_;

	for (unsigned i = 0; i < animations.Size(); ++i)
	{
		aiAnimation* anim = animations[i];

		float duration = (float)anim->mDuration;
		String animName = FromAIString(anim->mName);
		String animOutName;

		if (animName.Empty())
			animName = "Anim" + String(i + 1);
		if (model)
			animOutName = GetPath(model->outName_) + GetFileName(model->outName_) + "_" + SanitateAssetName(animName) + ".ani";
		else
			animOutName = outPath_ + SanitateAssetName(animName) + ".ani";

		float ticksPerSecond = (float)anim->mTicksPerSecond;
		// If ticks per second not specified, it's probably a .X file. In this case use the default tick rate
		if (ticksPerSecond < M_EPSILON)
			ticksPerSecond = defaultTicksPerSecond_;
		float tickConversion = 1.0f / ticksPerSecond;

		// Find out the start time of animation from each channel's first keyframe for adjusting the keyframe times
		// to start from zero
		float startTime = duration;
		for (unsigned j = 0; j < anim->mNumChannels; ++j)
		{
			aiNodeAnim* channel = anim->mChannels[j];
			if (channel->mNumPositionKeys > 0)
				startTime = Min(startTime, (float)channel->mPositionKeys[0].mTime);
			if (channel->mNumRotationKeys > 0)
				startTime = Min(startTime, (float)channel->mRotationKeys[0].mTime);
			if (channel->mScalingKeys > 0)
				startTime = Min(startTime, (float)channel->mScalingKeys[0].mTime);
		}
		duration -= startTime;

		SharedPtr<Animation> outAnim(new Animation(context_));
		outAnim->SetAnimationName(animName);
		outAnim->SetLength(duration * tickConversion);

		PrintLine("Writing animation " + animName + " length " + String(outAnim->GetLength()));
		Vector<AnimationTrack> tracks;
		for (unsigned j = 0; j < anim->mNumChannels; ++j)
		{
			aiNodeAnim* channel = anim->mChannels[j];
			String channelName = FromAIString(channel->mNodeName);
			aiNode* boneNode = 0;
			bool isRootBone = false;

			if (model)
			{
				unsigned boneIndex = GetBoneIndex(*model, channelName);
				if (boneIndex == M_MAX_UNSIGNED)
				{
					PrintLine("Warning: skipping animation track " + channelName + " not found in model skeleton");
					continue;
				}
				boneNode = model->bones_[boneIndex];
				isRootBone = boneIndex == 0;
			}
			else
			{
				boneNode = GetNode(channelName, scene_->mRootNode);
				if (!boneNode)
				{
					PrintLine("Warning: skipping animation track " + channelName + " whose scene node was not found");
					continue;
				}
			}

			AnimationTrack track;
			track.name_ = channelName;
			track.nameHash_ = channelName;

			// Check which channels are used
			track.channelMask_ = 0;
			if (channel->mNumPositionKeys > 1)
				track.channelMask_ |= CHANNEL_POSITION;
			if (channel->mNumRotationKeys > 1)
				track.channelMask_ |= CHANNEL_ROTATION;
			if (channel->mNumScalingKeys > 1)
				track.channelMask_ |= CHANNEL_SCALE;
			// Check for redundant identity scale in all keyframes and remove in that case
			if (track.channelMask_ & CHANNEL_SCALE)
			{
				bool redundantScale = true;
				for (unsigned k = 0; k < channel->mNumScalingKeys; ++k)
				{
					float SCALE_EPSILON = 0.000001f;
					Vector3 scaleVec = ToVector3(channel->mScalingKeys[k].mValue);
					if (fabsf(scaleVec.x_ - 1.0f) >= SCALE_EPSILON || fabsf(scaleVec.y_ - 1.0f) >= SCALE_EPSILON ||
						fabsf(scaleVec.z_ - 1.0f) >= SCALE_EPSILON)
					{
						redundantScale = false;
						break;
					}
				}
				if (redundantScale)
					track.channelMask_ &= ~CHANNEL_SCALE;
			}

			if (!track.channelMask_)
				PrintLine("Warning: skipping animation track " + channelName + " with no keyframes");

			// Currently only same amount of keyframes is supported
			// Note: should also check the times of individual keyframes for match
			if ((channel->mNumPositionKeys > 1 && channel->mNumRotationKeys > 1 && channel->mNumPositionKeys != channel->mNumRotationKeys) ||
				(channel->mNumPositionKeys > 1 && channel->mNumScalingKeys > 1 && channel->mNumPositionKeys != channel->mNumScalingKeys) ||
				(channel->mNumRotationKeys > 1 && channel->mNumScalingKeys > 1 && channel->mNumRotationKeys != channel->mNumScalingKeys))
			{
				PrintLine("Warning: differing amounts of channel keyframes, skipping animation track " + channelName);
				continue;
			}

			unsigned keyFrames = channel->mNumPositionKeys;
			if (channel->mNumRotationKeys > keyFrames)
				keyFrames = channel->mNumRotationKeys;
			if (channel->mNumScalingKeys > keyFrames)
				keyFrames = channel->mNumScalingKeys;

			for (unsigned k = 0; k < keyFrames; ++k)
			{
				AnimationKeyFrame kf;
				kf.time_ = 0.0f;
				kf.position_ = Vector3::ZERO;
				kf.rotation_ = Quaternion::IDENTITY;
				kf.scale_ = Vector3::ONE;

				// Get time for the keyframe. Adjust with animation's start time
				if (track.channelMask_ & CHANNEL_POSITION && k < channel->mNumPositionKeys)
					kf.time_ = ((float)channel->mPositionKeys[k].mTime - startTime) * tickConversion;
				else if (track.channelMask_ & CHANNEL_ROTATION && k < channel->mNumRotationKeys)
					kf.time_ = ((float)channel->mRotationKeys[k].mTime - startTime) * tickConversion;
				else if (track.channelMask_ & CHANNEL_SCALE && k < channel->mNumScalingKeys)
					kf.time_ = ((float)channel->mScalingKeys[k].mTime - startTime) * tickConversion;

				// Make sure time stays positive
				kf.time_ = Max(kf.time_, 0.0f);

				// Start with the bone's base transform
				aiMatrix4x4 boneTransform = boneNode->mTransformation;
				aiVector3D pos, scale;
				aiQuaternion rot;
				boneTransform.Decompose(scale, rot, pos);
				// Then apply the active channels
				if (track.channelMask_ & CHANNEL_POSITION && k < channel->mNumPositionKeys)
					pos = channel->mPositionKeys[k].mValue;
				if (track.channelMask_ & CHANNEL_ROTATION && k < channel->mNumRotationKeys)
					rot = channel->mRotationKeys[k].mValue;
				if (track.channelMask_ & CHANNEL_SCALE && k < channel->mNumScalingKeys)
					scale = channel->mScalingKeys[k].mValue;

				// If root bone, transform with the model root node transform
				if (model && isRootBone)
				{
					aiMatrix4x4 transMat, scaleMat, rotMat;
					aiMatrix4x4::Translation(pos, transMat);
					aiMatrix4x4::Scaling(scale, scaleMat);
					rotMat = aiMatrix4x4(rot.GetMatrix());
					aiMatrix4x4 tform = transMat * rotMat * scaleMat;
					tform = GetDerivedTransform(tform, boneNode, model->rootNode_);
					tform.Decompose(scale, rot, pos);
				}

				if (track.channelMask_ & CHANNEL_POSITION)
					kf.position_ = ToVector3(pos);
				if (track.channelMask_ & CHANNEL_ROTATION)
					kf.rotation_ = ToQuaternion(rot);
				if (track.channelMask_ & CHANNEL_SCALE)
					kf.scale_ = ToVector3(scale);

				track.keyFrames_.Push(kf);
			}

			tracks.Push(track);
		}

		outAnim->SetTracks(tracks);

		File outFile(context_);
		if (!outFile.Open(animOutName, FILE_WRITE))
			ErrorExit("Could not open output file " + animOutName);
		outAnim->Save(outFile);
	}
	*/
}

unsigned GetNumValidFaces(aiMesh* mesh)
{
	unsigned ret = 0;

	for (unsigned j = 0; j < mesh->mNumFaces; ++j)
	{
		if (mesh->mFaces[j].mNumIndices == 3)
			++ret;
	}

	return ret;
}

VertexElementsType GetElementMask(aiMesh* mesh)
{
	VertexElementsType ves;

	ves.push_back(VE_POSITION);

	if (mesh->HasNormals())
	{
		ves.push_back(VE_NORMAL);
	}

	if (mesh->HasTangentsAndBitangents())
	{
		ves.push_back(VE_TANGENT);
	}

	if (mesh->GetNumUVChannels() > 0)
	{
		ves.push_back(VE_TEXCOORD1);
	}

	if (mesh->GetNumUVChannels() > 1)
	{
		ves.push_back(VE_TEXCOORD2);
	}

	/*
	if (mesh->HasBones())
	{
		ves.push_back(VE_BLENDWEIGHTS);
		ves.push_back(VE_BLENDINDICES);
	}
	*/

	return ves;
}

void GetPosRotScale(const aiMatrix4x4& transform, Vector3& pos, Quaternion& rot, Vector3& scale)
{
	aiVector3D aiPos;
	aiQuaternion aiRot;
	aiVector3D aiScale;
	transform.Decompose(aiScale, aiRot, aiPos);
	pos = ToVector3(aiPos);
	rot = ToQuaternion(aiRot);
	scale = ToVector3(aiScale);
}

aiMatrix4x4 GetMeshBakingTransform(aiNode* meshNode, aiNode* modelRootNode)
{
	if (meshNode == modelRootNode)
	{
		return aiMatrix4x4();
	}
	else
	{
		return GetDerivedTransform(meshNode, modelRootNode);
	}
}

aiMatrix4x4 GetDerivedTransform(aiNode* node, aiNode* rootNode)
{
	return GetDerivedTransform(node->mTransformation, node, rootNode);
}

aiMatrix4x4 GetDerivedTransform(aiMatrix4x4 transform, aiNode* node, aiNode* rootNode)
{
	// If basenode is defined, go only up to it in the parent chain
	while (node && node != rootNode)
	{
		node = node->mParent;
		if (node)
		{
			transform = node->mTransformation * transform;
		}
	}
	return transform;
}

Vector3 ToVector3(const aiVector3D& vec)
{
	return Vector3(vec.x, vec.y, vec.z);
}

Vector2 ToVector2(const aiVector2D& vec)
{
	return Vector2(vec.x, vec.y);
}

Quaternion ToQuaternion(const aiQuaternion& quat)
{
	return Quaternion(quat.w, quat.x, quat.y, quat.z);
}

String SanitateAssetName(const String& name)
{
	String fixedName = name;
	enn::replace_str(fixedName, "<", "");
	enn::replace_str(fixedName, ">", "");
	enn::replace_str(fixedName, "?", "");
	enn::replace_str(fixedName, "*", "");
	enn::replace_str(fixedName, ":", "");
	enn::replace_str(fixedName, "\"", "");
	enn::replace_str(fixedName, "/", "");
	enn::replace_str(fixedName, "\\", "");
	enn::replace_str(fixedName, "|", "");

	return fixedName;
}

String FromAIString(const aiString& str)
{
	return String(str.data);
}

void WriteShortIndices(unsigned short*& dest, aiMesh* mesh, unsigned index, unsigned offset)
{
	if (mesh->mFaces[index].mNumIndices == 3)
	{
		*dest++ = mesh->mFaces[index].mIndices[0] + offset;
		*dest++ = mesh->mFaces[index].mIndices[1] + offset;
		*dest++ = mesh->mFaces[index].mIndices[2] + offset;
	}
}

void WriteLargeIndices(unsigned*& dest, aiMesh* mesh, unsigned index, unsigned offset)
{
	if (mesh->mFaces[index].mNumIndices == 3)
	{
		*dest++ = mesh->mFaces[index].mIndices[0] + offset;
		*dest++ = mesh->mFaces[index].mIndices[1] + offset;
		*dest++ = mesh->mFaces[index].mIndices[2] + offset;
	}
}

void WriteVertex(float*& dest, aiMesh* mesh, unsigned index, const VertexElementsType elementMask, AxisAlignedBox& box,
	const Matrix4& vertexTransform, const Matrix3& normalTransform,
	enn::vector<enn::vector<uchar>::type>::type blend_indices,
	enn::vector<enn::vector<float>::type>::type blend_weight)
{
	Vector3 vertex = vertexTransform * ToVector3(mesh->mVertices[index]);
	box.merge(vertex);

	*dest++ = vertex.x;
	*dest++ = vertex.y;
	*dest++ = vertex.z;

	if (D3D11RenderDevice::hasElement(elementMask, VE_NORMAL))
	{
		Vector3 normal = normalTransform * ToVector3(mesh->mNormals[index]);
		*dest++ = normal.x;
		*dest++ = normal.y;
		*dest++ = normal.z;
	}

	if (D3D11RenderDevice::hasElement(elementMask, VE_TANGENT))
	{
		Vector3 tangent = normalTransform * ToVector3(mesh->mTangents[index]);
		Vector3 normal = normalTransform * ToVector3(mesh->mNormals[index]);
		Vector3 bitangent = normalTransform * ToVector3(mesh->mBitangents[index]);

		// Check handedness
		float w = 1.0f;
		if ((tangent.crossProduct(normal)).dotProduct(bitangent) < 0.5f)
		{
			w = -1.0f;
		}

		*dest++ = tangent.x;
		*dest++ = tangent.y;
		*dest++ = tangent.z;
		*dest++ = w;
	}

	if (D3D11RenderDevice::hasElement(elementMask, VE_TEXCOORD1))
	{
		Vector3 texCoord = ToVector3(mesh->mTextureCoords[0][index]);
		*dest++ = texCoord.x;
		*dest++ = texCoord.y;
	}

	if (D3D11RenderDevice::hasElement(elementMask, VE_TEXCOORD2))
	{
		Vector3 texCoord = ToVector3(mesh->mTextureCoords[1][index]);
		*dest++ = texCoord.x;
		*dest++ = texCoord.y;
	}

	if (D3D11RenderDevice::hasElement(elementMask, VE_BLENDINDICES))
	{
		for (int i = 0; i < 4; ++i)
		{
			if (i < (int)blend_weight[index].size())
			{
				*dest++ = blend_weight[index][i];
			}
			else
			{
				*dest++ = 0.0f;
			}
		}
	}

	if (D3D11RenderDevice::hasElement(elementMask, VE_BLENDINDICES))
	{
		uchar* destBytes = (uchar*)dest;
		++dest;
		for (uint32 i = 0; i < 4; ++i)
		{
			if (i < blend_indices[index].size())
			{
				*destBytes++ = blend_indices[index][i];
			}
			else
			{
				*destBytes++ = 0;
			}
		}
	}
}

void GetBlendData(OutModel& model, aiMesh* mesh, enn::vector<uint32>::type& bone_mappings,
	enn::vector<enn::vector<uchar>::type>::type& blend_indices,
	enn::vector<enn::vector<float>::type>::type& blend_weights)
{
	blend_indices.resize(mesh->mNumVertices);
	blend_weights.resize(mesh->mNumVertices);

	bone_mappings.clear();

	if (model.bones_.size() > MAX_BONES_NUM)
	{
		ENN_ASSERT(0);
	}
	else
	{
		for (uint32 i = 0; i < mesh->mNumBones; ++i)
		{
			aiBone* bone = mesh->mBones[i];
			String bone_name = bone->mName.data;

			uint32 global_idx = GetBoneIndex(model, bone_name);
			if (global_idx == Math::ENN_MAX_UNSIGNED)
			{
				ENN_ASSERT(0);
			}

			for (uint32 j = 0; j < bone->mNumWeights; ++j)
			{
				uint32 vertex = bone->mWeights[j].mVertexId;
				blend_indices[vertex].push_back(global_idx);
				blend_weights[vertex].push_back(bone->mWeights[j].mWeight);

				if (blend_weights[vertex].size() > 4)
				{
					ENN_ASSERT(0);
				}
			}
		}
	}
}

unsigned GetBoneIndex(OutModel& model, const String& boneName)
{
	for (unsigned i = 0; i < model.bones_.size(); ++i)
	{
		if (boneName == model.bones_[i]->mName.data)
			return i;
	}

	return -1;
}

void BuildAndSaveMaterial(aiMaterial* material, set<String>::type& usedTextures)
{
	aiString matNameStr;
	material->Get(AI_MATKEY_NAME, matNameStr);
	String matName = SanitateAssetName(FromAIString(matNameStr));
	if (trim_str(matName).empty())
	{
		matName = GenerateMaterialName(material);
	}

	matName += ".mtr";

	Material* enn_mtr = ENN_NEW Material();

	String diffuseTexName;
	String normalTexName;
	String specularTexName;
	String lightmapTexName;
	String emissiveTexName;
	Color diffuseColor = Color::WHITE;
	Color specularColor;
	Color emissiveColor = Color::BLACK;
	bool hasAlpha = false;
	bool twoSided = false;
	float specPower = 1.0f;

	aiString stringVal;
	float floatVal;
	int intVal;
	aiColor3D colorVal;

	if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), stringVal) == AI_SUCCESS)
	{
		diffuseTexName = stringVal.data;
	}
		
	if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), stringVal) == AI_SUCCESS)
	{
		normalTexName = stringVal.data;
	}
		
	if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), stringVal) == AI_SUCCESS)
	{
		specularTexName = stringVal.data;
	}
	
	if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_LIGHTMAP, 0), stringVal) == AI_SUCCESS)
	{
		lightmapTexName = stringVal.data;
	}
	
	if (material->Get(AI_MATKEY_TEXTURE(aiTextureType_EMISSIVE, 0), stringVal) == AI_SUCCESS)
	{
		emissiveTexName = stringVal.data;
	}

	// material color
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, colorVal) == AI_SUCCESS)
	{
		diffuseColor = Color(colorVal.r, colorVal.g, colorVal.b);

		enn_mtr->setDiffuse(diffuseColor);
	}

	if (material->Get(AI_MATKEY_COLOR_SPECULAR, colorVal) == AI_SUCCESS)
	{
		specularColor = Color(colorVal.r, colorVal.g, colorVal.b);

		specularColor.a = 0;

		enn_mtr->setSpecularShiness(specularColor);
	}

	if (material->Get(AI_MATKEY_COLOR_EMISSIVE, colorVal) == AI_SUCCESS)
	{
		emissiveColor = Color(colorVal.r, colorVal.g, colorVal.b);

		enn_mtr->setEmissive(emissiveColor);
	}

	if (material->Get(AI_MATKEY_SHININESS, floatVal) == AI_SUCCESS)
	{
		specPower = floatVal;
		specularColor.a = specPower;

		enn_mtr->setSpecularShiness(specularColor);
	}

	// set texture
	if (!diffuseTexName.empty())
	{
		TextureObjUnit* tu_diff = enn_mtr->useTexDiff();
		tu_diff->setTexNameNoLoading(diffuseTexName);

		usedTextures.insert(diffuseTexName);
	}

	if (!normalTexName.empty())
	{
		TextureObjUnit* tu_normal = enn_mtr->useTexNormal();
		tu_normal->setTexNameNoLoading(normalTexName);

		usedTextures.insert(normalTexName);
	}

	if (!specularTexName.empty())
	{
		TextureObjUnit* tu = enn_mtr->useTexSpecular();
		tu->setTexNameNoLoading(specularTexName);

		usedTextures.insert(specularTexName);
	}

	if (!emissiveTexName.empty())
	{
		TextureObjUnit* tu = enn_mtr->useTexEmissive();
		tu->setTexNameNoLoading(emissiveTexName);

		usedTextures.insert(emissiveTexName);
	}

	if (!lightmapTexName.empty())
	{
		TextureObjUnit* tu = enn_mtr->useTexDiff2();
		tu->setTexNameNoLoading(lightmapTexName);

		usedTextures.insert(lightmapTexName);
	}

	g_mtr_mgr_->save(enn_mtr, matName);
}

String GenerateMaterialName(aiMaterial* material)
{
	for (unsigned i = 0; i < scene_->mNumMaterials; ++i)
	{
		if (scene_->mMaterials[i] == material)
			return inputName_ + "_Material" + to_string(i);
	}

	// Should not go here
	return "";
}

String GetMeshMaterialName(aiMesh* mesh)
{
	aiMaterial* material = scene_->mMaterials[mesh->mMaterialIndex];
	aiString matNameStr;
	material->Get(AI_MATKEY_NAME, matNameStr);
	String matName = SanitateAssetName(FromAIString(matNameStr));

	if (trim_str(matName).empty())
	{
		matName = GenerateMaterialName(material);
	}

	return matName + ".mtr";
}

}

//////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	vector<String>::type arguments = ParseArguments(argc, argv);
	run(arguments);
	return 0;
}

