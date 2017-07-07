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
// Time: 2014/07/18
// File: ssao.fx
//

struct VS_INPUT
{
	float4 position				: POSITION;
	float3 normal				: NORMAL;
	float2 texcoord				: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position				: SV_Position;
	float2 texcoord				: TEXCOORD0;
	float3 view_ray				: TEXCOORD1;
};

cbuffer Transforms
{
	float4x4 invMatProj				: packoffset(c0);
	float4x4 matProj				: packoffset(c4);
	float4x4 invViewProj			: packoffset(c8);
};

VS_OUTPUT SSAO_VS(in VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.position = input.position;
	output.texcoord = input.texcoord;
	float3 view_ray = mul(input.position, invMatProj).xyz;
	output.view_ray = float3(view_ray.xy / (view_ray.z), 1.0);
	
	return output;
}

cbuffer SSAOParams
{
	float4	paramA					: packoffset(c0);
	float4	paramB					: packoffset(c1);
};

Texture2D<float4> normal_texture : register(t0);
Texture2D<float> depth_texture : register(t1);
Texture2D<float4> random_texture : register(t2);

sampler texPointSampler : register (s0);

float3 PositionFromDepth( in float zBufferDepth, in float3 viewRay )
{
	// Convert to a linear depth value using the projection matrix
	float linearDepth = (matProj[3][2] / ( zBufferDepth - (matProj[2][2] )));
	return viewRay * linearDepth / viewRay.z;
}

float3 PositionFromDepthEx( in float2 sampleCoord, in float3 viewRay )
{
	// Convert to a linear depth value using the projection matrix
	int3 coord = int3(sampleCoord.x * 1024,  sampleCoord.y * 768, 0);
	//float zBufferDepth = depth_texture.Sample(texPointSampler, sampleCoord).x;
	float zBufferDepth = depth_texture.Load(coord).r;
	float linearDepth = (matProj[3][2] / ( zBufferDepth - (matProj[2][2] )));
	return viewRay * linearDepth / viewRay.z;
}

float3 getNormalView(in float2 sampleCoord)
{
	float4 value = normal_texture.Sample(texPointSampler, sampleCoord);
	return normalize(value.xyz * 2.0 - 1.0);
}

float2 getUVPosByProj(float4 proj_pos)
{
	float2 coord = proj_pos.xy / proj_pos.w;
	coord = coord * float2(0.5, -0.5) + float2(0.5, 0.5);
	
	return coord;
}

static float2 poisson_disk[12] = {float2(0.016176185, 0.060370367),
float2(-0.13258250, -0.13258255),
float2(0.30185181, 0.080880985),
float2(-0.42259261, 0.11323310),
float2(0.39774796, -0.39774719),
float2(-0.17793840, 0.66407394),
float2(-0.21028902, -0.78481513),
float2(0.66291237, 0.66291285),
float2(-1.0262960, -0.27499604),
float2(1.1470373, -0.30734619),
float2(-0.92807919, 0.92807615),
float2(0.37205264, -1.3885182)};

float4 SSAO_PS(in VS_OUTPUT input) : SV_Target0
{
    float disk_radius = paramA.x;
	float disk_radius_sqr = paramA.y;
	
	float intensity = paramA.z;
	float power = paramA.w;
	
	float cEpsilon = paramB.x;
	
	float3 pos_view = PositionFromDepthEx(input.texcoord, input.view_ray);
	float3 normal_view = getNormalView(input.texcoord);
	
	float curSelfOccBeta = pos_view.z * paramB.y; // y = cSelfOccBeta
	float2 randAngle = random_texture.Sample(texPointSampler, input.texcoord * float2(256, 192)).xy * 2.0 - 1.0;
	float2x2 rotMat  = float2x2(randAngle.x, -randAngle.y, randAngle.y, randAngle.x);
	
	float ao = 0;
    float cnt = 0;
	
	for (int i = 0; i < 12; ++i)
	{
		float2 sampleOff = poisson_disk[i] * disk_radius;
		
		sampleOff.xy = mul(rotMat, sampleOff.xy);
		
		float3 samplePos = pos_view + float3(sampleOff, 0.0);
		float4 proj_pos = mul(float4(samplePos, 1.0), matProj);
		float2 sampleOffUV = getUVPosByProj(proj_pos);
		float3 sampleOffPos = PositionFromDepthEx(sampleOffUV, samplePos);
		float3 sampleVector = sampleOffPos - pos_view;
		
		float3 world_normal = mul(normal_view, (float3x3)invViewProj);
		float3 world_vector = mul(sampleVector, (float3x3)invViewProj);
		
		float dot_vn = dot(sampleVector, normal_view);
        float dot_vv = dot(sampleVector, sampleVector);
		
		bool is_valid = dot_vv < disk_radius_sqr;
		
		ao += is_valid ? max(0.0, dot_vn - curSelfOccBeta) / (dot_vv + cEpsilon) : 0;
		cnt += is_valid ? 1 : 0;
	}
	
	ao = cnt > 0.001 ? ao / cnt : 0.0;
	ao = max(0, 1.0 - 2 * 2 * ao / 8);
	
	ao = pow(ao, 8.0);
	float4 color;
	color.rgba = float4(ao, ao, ao, 1);
	return color;
}