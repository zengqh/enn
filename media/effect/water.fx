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
// File: water.fx

struct VS_INPUT
{
	float4 pos		: POSITION;
};

struct VS_OUTPUT
{
	float4 pos				 : SV_Position;
	float3 normal			 : NORMAL;
	float3 worldPos			 : TEXCOORD1;
	float4 projPos			 : TEXCOORD2;
	float3 viewPos			 : TEXCOORD3;
};

cbuffer WaterVSParams
{
	float4x4	matWVP;
	float4x4	matWV;
	float4x4	matProj;
	float4		gHeight;
};

cbuffer WaterPSParams
{
	float3 cameraPos;
	
	float4 gWorldCornerLeftTop;
	float4 gWorldCornerRightDir;
	float4 gWorldCornerDownDir;

	float4 gViewCornerLeftTop;
	float4 gViewCornerRightDir;
	float4 gViewCornerDownDir;
	
	float4 gDeepColor;
	float4 gRefractionDist;
	float4 gSunDir;
	float4 gSunColor;
	float4 gSunLightParam;
	float4 gNoiseScale;
	float4 gNoiseUVParam;
	float4 gNormalUVParam[4];
	float4 gNormalWeight;
	float4 gInvScreenSize;
	float4 gFarClip;
};

uniform sampler2D pointSampler : register (s0);
uniform sampler2D linearSampler : register (s1);

Texture2D<float4> texWave : register(t0);
Texture2D<float> texDepth : register(t1);
Texture2D<float4> texRefr : register(t2);
Texture2D<float4> texRefl : register(t3);
Texture2D<float4> texFresnel : register(t4);

VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;
	
	float4 worldPos = input.pos;
	worldPos.y = gHeight.x;
	
	output.pos = mul(worldPos, matWVP);
	output.normal = float3(0, 1, 0);
	output.worldPos = worldPos;
	output.projPos = output.pos;
	output.viewPos = mul(worldPos, matWV);
	
	return output;
}


float LinearDepth(float2 uv)
{
	float r = pointSampler.Sample(uv, texDepth).r;
	float linearDepth = matProj[3][2] / (zBufferDepth - matProj[2][2]);
	
	return linearDepth;
}

float ViewDepth(float2 uv)
{
	return LinearDepth(uv) * gFarClip.x;
}

float3 CalcuNoise(float3 pos)
{
	float2 uv0 = pos.xz * gNoiseUVParam.xy + gNoiseUVParam.zw;
	float3 noise = linearSampler.Sample(texWave, uv0).rgb - 0.5;
	return gNoiseScale * noise;
}

float3 CalcuNormal(float3 pos, float3 normal, float3 noise)
{
	float2 uv0 = pos.xz * gNormalUVParam[0].xy + gNormalUVParam[0].zw + noise.xy;
	float2 uv1 = pos.xz * gNormalUVParam[1].xy + gNormalUVParam[1].zw + noise.xy;
	float2 uv2 = pos.xz * gNormalUVParam[2].xy + gNormalUVParam[2].zw + noise.xy;
	float2 uv3 = pos.xz * gNormalUVParam[3].xy + gNormalUVParam[3].zw + noise.xy;
	
	float nrm0 = (linearSampler.Sample(gTex_Normal0, uv0).rgb * 2 - 1) * gNormalWeight.x;
	float nrm1 = (linearSampler.Sample(gTex_Normal0, uv1).rgb * 2 - 1) * gNormalWeight.y;
	float nrm2 = (linearSampler.Sample(gTex_Normal1, uv2).rgb * 2 - 1) * gNormalWeight.z;
	float nrm3 = (linearSampler.Sample(gTex_Normal1, uv3).rgb * 2 - 1) * gNormalWeight.w;
	
	float3 nrm = nrm0 + nrm1 + nrm2 + nrm3;
	
	normal = normalize(normal);
	normal.xz += nrm.xy;
	return normalize(normal);
}

float CalcuDeepWeight(float2 projUV, float3 wpos, float3 vpos)
{
	float diff0, diff1;
	
	float d = LinearDepth(projUV);
	
	{
		float4 p = gWorldCornerLeftTop + gWorldCornerRightDir * projUV.x + gWorldCornerDownDir * projUV.y;
		float3 pos = gCameraPos + p.xyz * d;
		diff0 = wpos.y - pos.y;
	}
	
	{
		float4 p = gViewCornerLeftTop + gViewCornerRightDir * projUV.x + gViewCornerDownDir * projUV.y;
		float3 pos = p.xyz * d;
		diff1 = length(pos) - length(vpos);
	}
	
	float k0 = max(0, diff0 / gRefractionDist);
	float k1 = max(0, diff1 / gRefractionDist);
	
	float k = (k0 + k1) * 0.5;
	float kv = min(1, ((length(vpos) - 100) / (1000 - 100));
	k = min(1, k);
	k = lerp(k, 1, kv);
	
	return saturate(k);
}

float CalcuSmoothFactor(float2 projUV, float3 vpos)
{
	float d1 = vpos.z;
	float d2 = ViewDepth(projUV);
	
	float s = saturate((d2 - d1) / 10);
	float k = saturate((d2 - 200) / (1000 - 200));
	
	s = lerp(s, 1, k);
	return s;
}

float2 CalcuNoiseIncUV(float2 projUV, float2 incUV, float3 vpos)
{
	float d1 = vpos.z;
	float d2 = ViewDepth(projUV + incUV);
	
	return incUV * (d2 > d1);
}

float CalcuFresnel(float3 normal, float3 eyeDir)
{
	float dp = dot(normal, eyeDir);
	return pointSampler.Sample(texFresnel, float2(dp, 0)).r;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target
{
	float2 projUV = input.projPos.xy / input.projPos.w * float2(0.5, -0.5) + gInvScreenSize.xy * 0.5f;
	float3 noise = CalcuNoise(input.worldPos);
	float3 eyeDir = normalize(gCameraPos - input.worldPos);
	float3 normal = CalcuNormal(input.worldPos, input.normal, noise);
	
	float wgt = CalcuDeepWeight(projUV, input.worldPos, input.viewPos);
	float smooth = CalcuSmoothFactor(projUV, input.vpos);
	
	float2 rNoiseUV = CalcuNoiseIncUV(projUV, normal.xz * 0.04, input.viewPos);
	float4 refrColor = linearSampler.Sample(texRefr, projUV + rNoiseUV * smooth);
	float4 reflColor = linearSampler.Sample(texRefl, projUV + normal.xz * 0.065 * smooth);
	float fresnel = CalcuFresnel(normal, eyeDir);
	
	refrColor = lerp(refrColor, gDeepColor, wgt);
	float4 col = lerp(refrColor, reflColor, fresnel * smooth);
	
	col.rgb += CalcuSunColor(normal, eyeDir) * smooth;
	col.a = 0;
	
	return col;
}