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
// Time: 2014/07/17
// File: general.fx
//

// transform
cbuffer Transforms
{
	float4x4 matProj				: packoffset(c0);
	float4x4 matWVP					: packoffset(c4);
	float4x4 matWV					: packoffset(c8);
};

#ifdef HAS_POINT_LITS
cbuffer PointLitParams
{
	float4 position		: packoffset(c0);
	float4 diffuse		: packoffset(c1);
};
#endif

#ifdef HAS_SPOT_LITS
cbuffer SpotLitParams
{
	float4 position			: packoffset(c0);
	float4 direction		: packoffset(c1);
	float4 diffuse			: packoffset(c2);
};
#endif

struct VS_INPUT
{
	float4 position				: POSITION;
	float3 normal				: NORMAL;
	float2 texcoord				: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position				: SV_POSITION;
	float3 ray_view				: TEXCOORD0;
};

Texture2D<float4> normal_texture : register(t0);
Texture2D<float> depth_texture : register(t1);

VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.position = mul(input.position, matWVP);
	float3 positionVS = mul(input.position, matWV).xyz;
	output.ray_view = float3(positionVS.xy / positionVS.z, 1.0);
	
	return output;
}

float3 PositionFromDepth( in float zBufferDepth, in float3 viewRay )
{
	// Convert to a linear depth value using the projection matrix
	float linearDepth = matProj[3][2] / (zBufferDepth - matProj[2][2]);
	return viewRay * linearDepth;
}

float4 getNormalViewShininess(float4 value)
{
	return float4(normalize(value.xyz * 2.0 - 1.0), value.w * 255.0);
}

float4 PSMAIN(in VS_OUTPUT input) : SV_TARGET0
{
	int3 sampleCoord = int3(input.position.xy, 0);
	
	float4 normalValue = normal_texture.Load(sampleCoord);
	float4 normalShiness = getNormalViewShininess(normalValue);
	float3 normal_wvs = normalShiness.rgb;
	float shiness = normalShiness.a;
	float zbufferValue = depth_texture.Load(sampleCoord).r;
	
	float3 position_wvs = PositionFromDepth(zbufferValue, input.ray_view);
	
	float3 totalLitDiff = float3(0.0, 0.0, 0.0);
    float3 totalLitSpec = float3(0.0, 0.0, 0.0);
	
	#ifdef HAS_POINT_LITS
		float4 posAtt = position;
        float3 lDiff  = diffuse.rgb;

        float3 vL = posAtt.xyz - position_wvs;
	    float  vL_len = length(vL);
	    vL = vL / vL_len;
		
		float att = cos(clamp(vL_len * posAtt.w, 0.0, M_PI)) * 0.5 + 0.5;
        lDiff *= att;
		
		totalLitDiff = max(0.0, dot(normal_wvs, vL)) * lDiff;
		
		if (shiness > 1e-3)
        {
            float3 vH = normalize(vL - normalize(position_wvs));
            totalLitSpec = pow(max(0.0, dot(normal_wvs, vH)), shiness) * lDiff;
        }
	#endif
	
	#ifdef HAS_SPOT_LITS
		float4 posAtt      = position;
        float4 spotDirCosI = direction;
        float4 lDiffCosIO  = diffuse;
		
		float3 vL = posAtt.xyz - position_wvs;
	    float  vL_len = length(vL);
	    vL = vL / vL_len;
		
		float att = cos(clamp(vL_len * posAtt.w, 0.0, M_PI)) * 0.5 + 0.5;
        att *= square( saturate( (dot(spotDirCosI.xyz, vL) - spotDirCosI.w) * lDiffCosIO.w ) );
        lDiffCosIO.rgb *= att;
		
		totalLitDiff = max(0.0, dot(normal_wvs, vL)) * lDiffCosIO.rgb;
		
		if ( shiness > 1e-3 )
        {
            float3 vH = normalize(vL - normalize(position_wvs));
            totalLitSpec = pow(max(0.0, dot(normal_wvs, vH)), shiness) * lDiffCosIO.rgb;
        }
	#endif
	
	
	float4 output;
	
	output.rgb = totalLitDiff;
	output.a = toLumin(totalLitSpec);
	
	return output;
}



