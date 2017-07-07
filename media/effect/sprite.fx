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
// File: sprite.fx
//

cbuffer TexViewportInfo : register(b0)
{
	float4 TexAndViewportSize : packoffset(c0);
};

Texture2D	SpriteTexture : register(t0);
SamplerState SpriteSampler : register(s0);

struct VS_INPUT
{
	float2 position				: POSITION;
	float2 texcoord				: TEXCOORD0;
	float4 source_rect			: TEXCOORD2;
};

struct VS_OUTPUT
{
	float4 position				: SV_Position;
	float2 texcoord				: TEXCOORD0;
};

VS_OUTPUT SpriteVS(in VS_INPUT input)
{
	float4 positionSS = float4(input.position * input.source_rect.zw, 0.0, 1.0);
	positionSS.xy += input.source_rect.xy;
	
	float4 positionDS = positionSS;
	
	// Scale by the viewport size, flip Y, then rescale to device coordinates
	positionDS.xy /= TexAndViewportSize.zw;
    positionDS.xy = positionDS * 2.0 - 1.0;
    positionDS.y *= -1.0;
	
	VS_OUTPUT output;
    output.position = positionDS;
    output.texcoord = input.texcoord;
	
    return output;
}

float4 SpritePS(in VS_OUTPUT input) : SV_Target
{
	int3 sampleCoord = int3(input.position.xy, 0);
    float4 texColor = SpriteTexture.Load(sampleCoord);
    return texColor;
}