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
// File: ssao_filter.fx
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
};

VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.position = input.position;
	output.texcoord = input.texcoord;
	
	return output;
}

Texture2D<float4> texAO : register(t0);
Texture2D<float> texDepth : register(t1);

sampler texAOSampler : register (s0);
sampler texPointSampler : register(s1);

cbuffer filterSampleOffsets
{
	float4 filterOffset[9]			: packoffset(c0);
}

cbuffer photometricExponent
{
	float4 phe;
}

cbuffer Transforms
{
	float4x4 matProj	: packoffset(c0);
}

float getLinearDepth(in float2 uv)
{
	float zBufferDepth = texDepth.Sample(texPointSampler, uv);
	float linearDepth = matProj[3][2] / ( zBufferDepth - (matProj[2][2] ));
	
	return linearDepth;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target0
{
	float ao = texAO.Sample(texAOSampler, input.texcoord).r;
	float linearDepth = getLinearDepth(input.texcoord).r;
	float total_ao = 0.0;
	float total_weight = 0.0;
	
	for (int i = 0; i < 9; ++i)
	{
		float2 sampleCoord = input.texcoord + filterOffset[i].xy;
		float sampleDepth = getLinearDepth(sampleCoord);
		
		float weight = filterOffset[i].z / pow((1 + abs((linearDepth - sampleDepth))), phe.x);
		
		total_ao += texAO.Sample(texAOSampler, sampleCoord).r;
		total_weight += weight;
	}
	
	
	float final_ao = total_ao / total_weight;
	float4 f_color;
	
	f_color.rgba = float4(final_ao, final_ao, final_ao, 1.0);
	
	return f_color;
}

