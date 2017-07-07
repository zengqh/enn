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

cbuffer TonemapParams
{
	// w = gExposure
	// xyz = gBloomWeight
	float4 tone_map_params;
};


Texture2D<float4> lum_texture : register(t0);
Texture2D<float4> scene_texture : register(t0);
Texture2D<float4> lum_bright0 : register(t0);
Texture2D<float4> lum_bright1 : register(t0);
Texture2D<float4> lum_bright2 : register(t0);

sampler texPointSampler : register (s0);

const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;
const float W = 11.2;

float3 Uncharted2Tonemap(float3 x)
{
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}


VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.position = input.position;
	output.texcoord = input.texcoord;
	
	return output;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target0
{
	float gExposure = tone_map_params.w;
	float3 gBloomWeight = tone_map_params.xyz;
	
	float adaptedLum = lum_texture.Sample(texPointSampler, float2(0.5, 0.5)).r;
	
	float3 texColor = scene_texture.Sample(texPointSampler, input.texcoord).rgb;
	float3 texBright0 = lum_bright0.Sample(texPointSampler, input.texcoord).rgb * gBloomWeight.x;
	float3 texBright1 = lum_bright1.Sample(texPointSampler, input.texcoord).rgb * gBloomWeight.y;
	float3 texBright2 = lum_bright2.Sample(texPointSampler, input.texcoord).rgb * gBloomWeight.z;
	
	float3 texBright = texBright0 + texBright1 + texBright2;
	
	texColor += texBright;
	
	texColor *= gExposure / (adaptedLum + 0.001f);
	float3 curr = Uncharted2Tonemap(texColor);
	
	float3 whiteScale = 1.0f / Uncharted2Tonemap(W);
	float3 color = curr*whiteScale;
	
	return float4(color, 1);
}

