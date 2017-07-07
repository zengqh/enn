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
// File: sky.fx
//

// ref skyx-0.4

struct VS_INPUT
{
	float4 position				: POSITION;
};

struct VS_OUTPUT
{
	float4 oPosition			: SV_Position;
	float2 oUV					: TEXCOORD0;
	float3 oRayleighColor		: TEXCOORD1;
	float3 oMieColor			: TEXCOORD2;
	float3 oDirection			: TEXCOORD3;
	float  oOpacity				: TEXCOORD4;
	float  oHeight				: TEXCOORD5;
};


// kr_params:
// (x)uKrESun // Kr * ESun ,
// (y)uKmESun; // Km * ESun, 
// (z)uKr4PI;  // Kr * 4 * PI, 
// (w)uKm4PI;  // Km * 4 * PI

// invWaveLength: 
// xyz invWaveLength, w InnerRadius

// atmosphere_params: 
// (x) uScale = 1 / (outerRadius - innerRadius)
// (y) uScaleDepth = // Where the average atmosphere density is found
// (z) uScaleOverScaleDepth = Scale / ScaleDepth
// (w) not used.
cbuffer SkyParams
{
	float4x4 		matWVP;
	float4 			uLightDir;
	float4			uCameraPos;
	float4			invWaveLength;		
	float4			kr_params;			
	float4			atmosphere_params;
};


cbuffer PsParams
{
	float4 ps_params;
};

float scale(float cos, float uScaleDepth)
{
	float x = 1.0 - cos;
	return uScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

VS_OUTPUT VSMAIN(in VS_INPUT input)
{
	VS_OUTPUT output;
	
	float uScale = atmosphere_params.x;
	float uScaleDepth = atmosphere_params.y;
	float uScaleOverScaleDepth = atmosphere_params.z;
	float3 uInvWaveLength = invWaveLength.xyz;
	float uInnerRadius = invWaveLength.w;
	float uKrESun = kr_params.x;
	float uKmESun = kr_params.y;
	float uKr4PI = kr_params.z;
	float uKm4PI = kr_params.w;
	
	
	output.oPosition = mul(input.position, matWVP);
	float3 NPos = input.position.xyz;
	NPos.y = max(0, NPos.y);
	NPos = normalize(NPos);
	
	float3 v3Pos = NPos;
	v3Pos.y += uInnerRadius;
	
	float3 v3Ray = v3Pos - uCameraPos.xyz;
	float fFar = length(v3Ray);
	v3Ray /= fFar;
	
	// Calculate the ray's starting position, then calculate its scattering offset
	float3 v3Start = uCameraPos.xyz;
	float fHeight = uCameraPos.y;
	float fStartAngle = dot(v3Ray, v3Start) / fHeight;
	
	// NOTE: fDepth is not pased as parameter(like a constant) to avoid the little precission issue (Apreciable)
	float fDepth = exp(uScaleOverScaleDepth * (uInnerRadius - uCameraPos.y));
	float fStartOffset = fDepth * scale(fStartAngle, uScaleDepth);
	
	// Init loop variables
	float fSampleLength = fFar / 4;
	float fScaledLength = fSampleLength * uScale;
	float3 v3SampleRay = v3Ray * fSampleLength;
	float3 v3SamplePoint = v3Start + v3SampleRay * 0.5f;
	
	float3 color = 0;
	for (int i = 0; i < 4; ++i)
	{
		float fHeight = length(v3SamplePoint);
		float fDepth = exp(uScaleOverScaleDepth * (uInnerRadius-fHeight));
		float fLightAngle = dot(uLightDir.xyz, v3SamplePoint) / fHeight;
		float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
		float fScatter = (fStartOffset + fDepth*(scale(fLightAngle, uScaleDepth) - scale(fCameraAngle, uScaleDepth)));
		float3 v3Attenuate = exp(-fScatter * (uInvWaveLength * uKr4PI + uKm4PI)); // <<< TODO
		
		// Accumulate color
		v3Attenuate *= (fDepth * fScaledLength);
		color += v3Attenuate;
		
		// Next sample point
		v3SamplePoint += v3SampleRay;
	}
	
	
	// Outputs
    output.oRayleighColor = color * (uInvWaveLength * uKrESun); // TODO <--- parameter
    output.oMieColor = color * uKmESun; // TODO <--- *uInvMieWaveLength
    output.oDirection = uCameraPos - v3Pos;
    output.oHeight = 1-input.position.y;
	
	return output;
}

float4 PSMAIN(in VS_OUTPUT input) : SV_Target
{
	float  uG = ps_params.x;
	float  uG2 = ps_params.y;
	float  uExposure = ps_params.z;
	
	float cos = dot(uLightDir.xyz, input.oDirection) / length(input.oDirection);
	float cos2 = cos*cos;
	
	float rayleighPhase = 0.75 * (1.0 + 0.5*cos2);
	
	float miePhase = 1.5f * ((1.0f - uG2) / (2.0f + uG2)) * (1.0f + cos2) / pow(1.0f + uG2 - 2.0f * uG * cos, 1.5f);
					 
	float4 oColor = float4((1 - exp(-uExposure * (rayleighPhase * input.oRayleighColor + miePhase * input.oMieColor))), 1);
	oColor.rgb = lerp(oColor.rgb, 0, (input.oHeight - 1));
	
	oColor.rgb = pow(oColor.rgb, 2.2);
	
	return oColor;
}