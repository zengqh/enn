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
// File: terrain.fx
//

struct VS_INPUT
{
	float2 xz		: POSITION;
	float3 normal	: NORMAL;
	float y			: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position		: SV_Position;
	float2 tcoord0		: TEXCOORD0;
	float4 tcoord12		: TEXCOORD1;
	float4 tcoord34		: TEXCOORD2;
	float3 T			: TEXCOORD4;
	float3 B			: TEXCOORD5;
	float3 N			: TEXCOORD6;
};

cbuffer TerrainParams
{
	float4x4    matView;
	float4x4    matWVP;
	float4		gTransform;
	float4		gUVParam;
	float4		gUVScale;
};

float4 CalcuPosition(float2 xz, float y)
{
	float4 r = float4(xz.x, y, xz.y, 1);

	r.xz += gTransform.xz;

	return r;
}

float2 CalcuDetailUV(float2 xz) // world space
{
	float2 uv = xz * gUVParam.zw;
	
	return uv;
}

float2 CalcuBlendUV(float2 xz)
{
	float2 uv = xz * gUVParam.xy;
	
	return uv;
}

VS_OUTPUT VSMAIN(VS_INPUT vin)
{
	VS_OUTPUT output;
	
	float4 position = CalcuPosition(vin.xz, vin.y);
	float3 normal = vin.normal.xyz;
	float2 detailUV = CalcuDetailUV(position.xz);
	
	output.position = mul(position, matWVP);
	output.tcoord0 = CalcuBlendUV(vin.xz);
	
	output.tcoord12.xy = detailUV * gUVScale.x;
	output.tcoord12.zw = detailUV * gUVScale.y;
	output.tcoord34.xy = detailUV * gUVScale.z;
	output.tcoord34.zw = detailUV * gUVScale.w;
	
	float3 T = float3(1, 0, 0);
	float3 B = float3(0, 0, 1);
	float3 N = normal;
	
	B = cross(T, N);
	T = cross(N, B);
	
	float3x3 matTBN = float3x3(T, B, N);
	matTBN = mul(matTBN, (float3x3)matView);
	
	output.T = matTBN[0];
	output.B = matTBN[1];
	output.N = matTBN[2];
	
	return output;
}


////////////////////////////////////////////////////////////////////////
Texture2D<float4> texBlend : register(t0);

#ifdef DETAIL_LEVEL1
Texture2D<float4> texDetail0 : register(t1);
#endif

#ifdef DETAIL_LEVEL2
Texture2D<float4> texDetail0 : register(t1);
Texture2D<float4> texDetail1 : register(t2);
#endif

#ifdef DETAIL_LEVEL3
Texture2D<float4> texDetail0 : register(t1);
Texture2D<float4> texDetail1 : register(t2);
Texture2D<float4> texDetail2 : register(t3);
#endif

#ifdef DETAIL_LEVEL4
Texture2D<float4> texDetail0 : register(t1);
Texture2D<float4> texDetail1 : register(t2);
Texture2D<float4> texDetail2 : register(t3);
Texture2D<float4> texDetail3 : register(t4);
#endif


SamplerState linearSampler : register (s0);


float4 PSMAIN(in VS_OUTPUT input) : SV_Target0
{
	float3 output = 0;
	
	float4 weight = texBlend.Sample(linearSampler, input.tcoord0);
#ifdef DETAIL_LEVEL1
	output += ( texDetail0.Sample(linearSampler, input.tcoord12.xy).rgb);
#endif

#ifdef DETAIL_LEVEL2
	output += ( texDetail0.Sample(linearSampler, input.tcoord12.xy).rgb) * weight.a;
	output += ( texDetail1.Sample(linearSampler, input.tcoord12.zw).rgb) * weight.r;
#endif
	
#ifdef DETAIL_LEVEL3
	output += ( texDetail0.Sample(linearSampler, input.tcoord12.xy).rgb) * weight.a;
	output += ( texDetail1.Sample(linearSampler, input.tcoord12.zw).rgb) * weight.r;
	output += ( texDetail2.Sample(linearSampler, input.tcoord34.xy).rgb) * weight.g;
#endif
	
#ifdef DETAIL_LEVEL4
	output += ( texDetail0.Sample(linearSampler, input.tcoord12.xy).rgb) * weight.a;
	output += ( texDetail1.Sample(linearSampler, input.tcoord12.zw).rgb) * weight.r;
	output += ( texDetail2.Sample(linearSampler, input.tcoord34.xy).rgb) * weight.g;
	output += ( texDetail3.Sample(linearSampler, input.tcoord34.zw).rgb) * weight.b;
#endif
	
	return float4(output, 1);
}