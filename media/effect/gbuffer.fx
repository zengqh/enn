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
// File: gbuffer.fx
//

/////////////////////////////////////////////////////////////////////
// cbuffers

// transform
cbuffer Transforms
{
	float4x4 matWV			: packoffset(c0);
    float4x4 matWVP			: packoffset(c4);
};

// input and output

// output
struct VS_OUTPUT
{
	float4 position				: SV_Position;
	
#ifdef HAS_UV1
	float2 texCoord				: TEXCOORD0;
#endif

	float3 normalVS				: TEXCOORD1;
	
#ifdef HAS_NORMALMAP
	float3 TangentVS			: TEXCOORD2;
	float3 BitangentVS			: TEXCOORD3;
#endif
};

cbuffer MtrParams
{
	float4 mtrShiness			: packoffset(c0);
};

#ifdef HAS_NORMALMAP
Texture2D<float4> texNormal : register(t1);
sampler texNormalSampler : register (s1);
#endif

/////////////////////////////////////////////////////////////////////
VS_OUTPUT VSMAIN(COMM_VS_INPUT vin)
{
	VS_OUTPUT output;
	
	output.position = mul(vin.position ,matWVP);
	float3 normalVS = mul(vin.normal, (float3x3)matWV);
	
#ifdef HAS_NORMALMAP
	float3 tangentVS = normalize( mul( vin.tangent.xyz, (float3x3)matWV ) );
	float3 bitangentVS = normalize( cross( normalVS, tangentVS ) ) * vin.tangent.w;

	output.TangentVS = tangentVS;
	output.BitangentVS = bitangentVS;
#endif

	output.normalVS = normalVS;
	
#ifdef HAS_UV1
	output.texCoord = vin.tex0;
#endif
	
	return output;
}

/////////////////////////////////////////////////////////////////////
struct PS_OUTPUT
{
	float4 color : SV_Target0;
};

Texture2D<float4> texNormalFit : register(t0);
sampler texNormalFitSampler : register (s0);

// coming from khaos by master mao
float3 compressUnsignedNormalToNormalsBuffer( float3 vNormal )
{
	vNormal = normalize(vNormal);
	float3 vNormalUns = abs(vNormal);
	float maxNAbs = max(vNormalUns.z, max(vNormalUns.x, vNormalUns.y));
  
	float2 vTexCoord = vNormalUns.z<maxNAbs?(vNormalUns.y<maxNAbs?vNormalUns.yz:vNormalUns.xz):vNormalUns.xy;
	vTexCoord = vTexCoord.x < vTexCoord.y ? vTexCoord.yx : vTexCoord.xy;
	vTexCoord.y /= vTexCoord.x;
	
	float fFittingScale = texNormalFit.Sample(texNormalFitSampler, vTexCoord).a;
	vNormal *= fFittingScale / maxNAbs;

	return mad( vNormal, 0.5, 0.5 );
}

PS_OUTPUT PSMAIN(in VS_OUTPUT input)
{
	PS_OUTPUT output;
	
	float3 normalVS = input.normalVS;
#ifdef HAS_NORMALMAP
	float3x3 tangentFrameVS = float3x3( normalize( input.TangentVS ),
										normalize( input.BitangentVS ),
										normalize( input.normalVS ) );
										
	float3 normalTS = normalize( texNormal.Sample( texNormalSampler, input.texCoord ).rgb * 2.0f - 1.0f );
	
	normalVS = mul( normalTS, tangentFrameVS );
#endif
	
	output.color.rgb = compressUnsignedNormalToNormalsBuffer(normalVS);
	output.color.a = mtrShiness.a;
	
	return output;
}