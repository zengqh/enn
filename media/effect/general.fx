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
// File: general.fx
//

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// vertex shader

/////////////////////////////////////////////////////////////////////
// cbuffers

// transform
cbuffer Transforms
{
	float4x4 matWorld		: packoffset(c0);
    float4x4 matWVP			: packoffset(c4);
	float4x4 matUV			: packoffset(c8);
};


// skeleton
#ifdef HAS_SKELETON
cbuffer Skeleton
{
	float4x4 matBones[MAX_BONES_NUM]	: packoffset(c0);
};
#endif

// shadow map
#ifdef HAS_SHADOW

// global massive shadow map
#ifdef HAS_PSSM
cbuffer PSSMShadowProj
{
	float4x4 matShadowProj[4]	: packoffset(0);
};
#endif

#endif

/////////////////////////////////////////////////////////////////////
// input and output


// output
struct VS_OUTPUT
{
	float4 position				: SV_Position;

#ifdef HAS_UV1
	float2 tex0					: TEXCOORD0;
#endif

#ifdef HAS_UV2
	float2 tex1					: TEXCOORD1;
#endif

	float4 pos_ws				: TEXCOORD2;
	float3 normal_ws			: TEXCOORD3;
	
#ifdef HAS_NORMALMAP
	float3 tangent_ws			: TEXCOORD4;
	float3 bitangent_ws			: TEXCOORD5;
#endif

#ifdef HAS_SHADOW
#ifdef HAS_PSSM
	float4 pos_shadow_proj[3]	: TEXCOORD6;
#endif
#endif
};


/////////////////////////////////////////////////////////////////////
VS_OUTPUT VSMAIN(COMM_VS_INPUT vin)
{
	VS_OUTPUT output;
	
	float4 vPos_ = vin.position;
	float3 vNormal_ = vin.normal;
	
#ifdef HAS_SKELETON
	float4x3 skinning = 0;
	
	for (int i = 0; i < 4; ++i)
	{
		skinning += matBones[vin.bone_indices[i]] * vin.bone_weights[i];
	}
	
	vPos_.xyz = mul(vPos_, skinning);
	vNormal_ = mul(vNormal_, (float3x3)skinning);
#endif

	output.pos_ws = mul(vPos_, matWorld);
	output.normal_ws = mul(vNormal_, (float3x3)matWorld);
	
#ifdef HAS_NORMALMAP
	output.tangent_ws = mul(vin.tangent.xyz, (float3x3)matWorld);
	output.bitangent_ws = normalize( cross( output.normal_ws, output.tangent_ws ) ) * vin.tangent.w;
#endif

#ifdef HAS_UV1
#ifdef HAS_TEXANI
	output.tex0 = mul(float3(vin.tex0.x, vin.tex0.y, 1.0), matUV).xy;
#else
	output.tex0 = vin.tex0;
#endif
#endif
	
#ifdef HAS_SHADOW
	#ifdef HAS_PSSM
		output.pos_shadow_proj[0] = mul(vPosWorld, matShadowProj[0]);
		output.pos_shadow_proj[1] = mul(vPosWorld, matShadowProj[1]);
		output.pos_shadow_proj[2] = mul(vPosWorld, matShadowProj[2]);
		
		float4 vPosSP3 = mul(vPosWorld, matShadowProj[3]);
		output.pos_shadow_proj[0].w = vPosSP3.x;
		output.pos_shadow_proj[1].w = vPosSP3.y;
		output.pos_shadow_proj[2].w = vPosSP3.z;
		
		output.pos_ws.w = vProjPos.w;
	#endif
#endif

	output.position = mul(vPos_, matWVP);
	
	return output;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// pixel shder
struct PS_OUTPUT
{
	float4 color : SV_Target;
};

cbuffer MtrParams
{
	float3 mtr_diffuse			: packoffset(c0);
	float4 mtr_spec				: packoffset(c1);

	float3 mtr_emissive			: packoffset(c2);

	float4 tex_clr				: packoffset(c3);

	float2 fog_param			: packoffset(c4);
	
	float mtr_alpha				: packoffset(c4.z);
	float alpha_test_ref		: packoffset(c4.w);
	
	float3 fog_color			: packoffset(c5);
};

#ifdef HAS_LIGHTING

#ifdef HAS_DIR_LITS
cbuffer DirLightParams
{
	float3 dir_light_dir		: packoffset(c0);
	float3 dir_light_diff		: packoffset(c1);
};
#endif

#ifdef HAS_SHADOW
cbuffer ShadowParams
{
	float3 shadowInfo			: packoffset(c0);
	float2 shadowFade			: packoffset(c1);
	float3 splitZView			: packoffset(c2);
	float4 poissonDisk			: packoffset(c3);
};
#endif

#endif

cbuffer GeometryParams
{
	float3 global_amb			: packoffset(c0);
	float3 camPos				: packoffset(c1);
};


/////////////////////////////////////////////////////////////////////
// Sampler and texture

#ifdef HAS_DIFFUSEMAP
Texture2D<float4> texDiff : register(t0);
sampler texDiffSampler : register (s0);			
#endif

#ifdef HAS_DIFFUSE2MAP
Texture2D<float4> texDiff2 : register(t1);
sampler texDiff2Sampler : register (s1);			
#endif

#ifdef HAS_NORMALMAP
Texture2D<float4> texNormal : register(t2);
sampler texNormalSampler : register (s2);			
#endif

#ifdef HAS_SEPCULARMAP
Texture2D<float4> texSpecular : register(t3);
sampler texSpecularSampler : register (s3);			
#endif

#ifdef HAS_EMISSIVEMAP
Texture2D<float4> texEmissive : register(t4);
sampler texEmissiveSampler : register (s4);			
#endif

#ifdef HAS_AO
Texture2D<float4> mapAO : register(t5);
sampler mapAOSampler : register (s5);			
#endif

#ifdef HAS_DEFER_LIT
Texture2D<float4> lightBuffer : register(t6);		
#endif


PS_OUTPUT PSMAIN(in VS_OUTPUT input)
{
	PS_OUTPUT output;
	
	int3 sample_screen_coord = int3(input.position.xy, 0);
	
#ifdef HAS_COLORMAP
	float4 finalClr = PSBufferMisc.tex_clr;
#else
	float4 finalClr = float4(1.0, 1.0, 1.0, 1.0);
#endif

#ifdef HAS_DIFFUSEMAP
	float4 texDiffClr = texDiff.Sample(texDiffSampler, input.tex0);
	finalClr.a *= texDiffClr.a;
#else
	float4 texDiffClr = float4(1.0, 1.0, 1.0, 1.0);
#endif

	finalClr.a *= mtr_alpha;
	
#ifdef HAS_ALPHA_TEST
	if (finalClr.a < alpha_test_ref)
	{
		discard;
	}
#endif

#ifdef HAS_LIGHTING
	float3 totLitClrDiff = float3(0.0, 0.0, 0.0);
    float3 totLitClrSpec = float3(0.0, 0.0, 0.0);
    
    #ifdef HAS_NORMALMAP
		float3x3 tangentFrameVS = float3x3( normalize( input.tangent_ws ),
										normalize( input.bitangent_ws ),
										normalize( input.normal_ws ) );
										
		float3 normalTS = normalize( texNormal.Sample( texNormalSampler, input.tex0 ).rgb * 2.0f - 1.0f );
	
	float3 vN = mul( normalTS, tangentFrameVS );
    #else
		float3 vN = normalize(input.normal_ws);
    #endif
    
    float3 vE = normalize(camPos - input.pos_ws.xyz);
    
    #ifdef HAS_DIR_LITS
		#ifdef HAS_SHADOW
			#ifdef HAS_PSSM
				
			#else
			
			#endif
		#else
			float lightIllum = 1.0;
		#endif
		
		float3 vL = dir_light_dir;
		float df = max(0.0, dot(vN, vL));
		
		totLitClrDiff = df * lightIllum * dir_light_diff;
		
		#ifdef HAS_SPECULAR
			float3 vH = normalize(vL + vE);
			float sf = max(0.0, dot(vN, vH));
			
			sf = pow(sf, mtr_spec.a);
			totLitClrSpec += sf * dir_light_diff;
		#endif
    #endif	// end of HAS_DIR_LITS
    
    #ifdef HAS_DEFER_LIT
		float4 lit_buffer_color = lightBuffer.Load(sample_screen_coord);
		
		totLitClrDiff += lit_buffer_color.rgb;
		#ifdef HAS_SPECULAR
			totLitClrSpec += lit_buffer_color.rgb * (lit_buffer_color.a / (toLumin(lit_buffer_color.rgb) + 0.02));
		#endif
    #endif
    
    totLitClrDiff = global_amb + totLitClrDiff * mtr_diffuse;
    
    #ifdef HAS_SPECULAR
		totLitClrSpec = totLitClrSpec * mtr_spec.rgb;
	#endif
	
	#ifdef HAS_SPECULARMAP
		totLitClrSpec *= texSpecular.Sample(texSpecularSampler, input.tex0).rgb;
	#endif
	
#else
	float3 totLitClrDiff = float3(1.0, 1.0, 1.0);
    float3 totLitClrSpec = float3(0.0, 0.0, 0.0);
#endif

	float3 totReflectClr = totLitClrDiff * texDiffClr.rgb + totLitClrSpec;
	
	#ifdef HAS_DEFER_LIT
		#ifdef HAS_AO
			FLOAT ao = texture2DProj(mapAO, vLitBufTex).a;
			totReflectClr.rgb *= ao;
		#endif
	#endif
	
	#ifdef HAS_EMISSIVE
		#ifdef HAS_EMISSIVE_MAP
			totReflectClr += mtr_emis * texEmissive.Sample(texEmissiveSampler, input.tex0).rgb;
		#else
			totReflectClr += mtr_emis;
		#endif
	#endif
	
	finalClr.rgb *= totReflectClr;
	
	// light map
	#ifdef HAS_DIFFUSE2MAP
		finalClr.rgb *= texDiff2.Sample(texDiff2Sampler, input.tex1).rgb;
	#endif
	
	#ifdef HAS_FOG
		float camDist = length(camPos - input.pos_ws.xyz);
		float fogP = clamp((fogParam.y - camDist) / fogParam.x,0.0, 1.0);
		finalClr.rgb = lerp(fogColor, finalClr.rgb, fogP);
	#endif
	
	output.color = finalClr;
	
	return output;
}