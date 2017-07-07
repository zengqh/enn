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
// File: hdr.fx
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

cbuffer GodRayParams
{
	float4 gGodRayParam;
};

Texture2D<float4> sun_texture : register(t0);
Texture2D<float> depth_texture : register(t0);
sampler texPointSampler : register (s0);


VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.position = input.position;
	output.texcoord = input.texcoord;
	
	return output;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target0
{
	float4 color = float4(0, 0, 0, 0);
	
	float2 sunUV = gGodRayParam.xy;
	float uvStep = gGodRayParam.z;
	float2 dir = sunUV - input.texcoord;
	
	for (int i = 0; i < 32; ++i)
	{
		float2 uv = input.texcoord + dir * uvStep * i;
		float depth = depth_texture.Sample(texPointSampler, uv);
		float sunColor = sun_texture.Sample(texPointSampler, uv).r;
		
		if (depth > 0.999f)
		{
			color += sunColor;
		}
	}
	
	color /= 32;
	
	return color;
}

