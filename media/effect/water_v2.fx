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
// File: water_v2.fx

struct VS_INPUT
{
	float4 pos		: POSITION;
};

struct VS_OUTPUT
{
	float4 hpos             : SV_POSITION;   
   
   // TexCoord 0 and 1 (xy,zw) for ripple texture lookup
   float4 rippleTexCoord01 : TEXCOORD0;   
   
   // xy is TexCoord 2 for ripple texture lookup 
   // z is the Worldspace unit distance/depth of this vertex/pixel
   // w is amount of the crestFoam ( more at crest of waves ).
   float4 rippleTexCoord2  : TEXCOORD1;
   
   // Screenspace vert position BEFORE wave transformation
   float4 posPreWave       : TEXCOORD2;
   
   // Screenspace vert position AFTER wave transformation
   float4 posPostWave      : TEXCOORD3;   
    
   // Objectspace vert position BEFORE wave transformation	
   // w coord is world space z position.
   float4 positionW        : TEXCOORD4;   
   
   float4 foamTexCoords    : TEXCOORD5;
   
   float3 ray_view		   : TEXCOORD6;
   
   float3x3 tangentMat     : TEXCOORD7;
};


cbuffer WaterVSParams
{
	float4x4 modelMat;
	float4x4 matWV;
	float4x4 modelviewProj;
	float4x4 matProj;
	float4   rippleMat[3];
	float4   eyePosWVS;       
	float4   waveDir[3];
	float4   waveData[3];
	float4   rippleDir[3];
	float4   rippleTexScale[3];                    
	float4   rippleSpeed;
	float4   foamDir;
	float4   foamTexScale;
	float4   foamSpeed;
	float4   elapsedTime;
};


////////////////////////////////////////////////////////////////////////
VS_OUTPUT VSMAIN(in VS_INPUT IN)
{
	VS_OUTPUT OUT;
					   
	OUT.positionW = mul(IN.pos, modelMat);
	OUT.positionW.w = IN.pos.y;
	
	OUT.posPreWave = mul(IN.pos, modelviewProj);
	
	float2 undulatePos = mul(float4(IN.pos.x, 0, IN.pos.z, 1), modelMat).xz;
	float undulateAmt = 0.0;
	
	undulateAmt += waveData[0].y * sin( elapsedTime.x * waveData[0].x + 
                                       undulatePos.x * waveDir[0].x +
                                       undulatePos.y * waveDir[0].y );
   undulateAmt += waveData[1].y * sin( elapsedTime.x * waveData[1].x + 
                                       undulatePos.x * waveDir[1].x +
                                       undulatePos.y * waveDir[1].y );
   undulateAmt += waveData[2].y * sin( elapsedTime.x * waveData[2].x + 
                                       undulatePos.x * waveDir[2].x +
                                       undulatePos.y * waveDir[2].y );
									  
   
   OUT.rippleTexCoord2.w = undulateAmt / ( waveData[0].y + waveData[1].y + waveData[2].y );	
   OUT.rippleTexCoord2.w = saturate( OUT.rippleTexCoord2.w - 0.2 ) / 0.8;
   
   OUT.posPostWave = IN.pos;
   OUT.posPostWave.xyz += float3(0, 1, 0) * undulateAmt;
   
   OUT.posPostWave = mul( OUT.posPostWave, modelviewProj );
   
   // Setup the OUT position symantic variable
   OUT.hpos = OUT.posPostWave;
   
   // Save world space camera dist/depth of the outgoing pixel
   OUT.rippleTexCoord2.z = OUT.hpos.w;
   
   float2 txPos = undulatePos;
   
   // set up tex coordinates for the 3 interacting normal maps   
   OUT.rippleTexCoord01.xy = txPos * rippleTexScale[0].xy;
   OUT.rippleTexCoord01.xy += rippleDir[0].xy * elapsedTime.x * rippleSpeed.x;
            
   float2x2 texMat;   
   texMat[0][0] = rippleMat[0].x;
   texMat[0][1] = rippleMat[0].y;
   texMat[1][0] = rippleMat[0].z;
   texMat[1][1] = rippleMat[0].w;
   OUT.rippleTexCoord01.xy = mul( texMat, OUT.rippleTexCoord01.xy );      

   OUT.rippleTexCoord01.zw = txPos * rippleTexScale[1].xy;
   OUT.rippleTexCoord01.zw += rippleDir[1].xy * elapsedTime.x * rippleSpeed.y;
   
   texMat[0][0] = rippleMat[1].x;
   texMat[0][1] = rippleMat[1].y;
   texMat[1][0] = rippleMat[1].z;
   texMat[1][1] = rippleMat[1].w;
   OUT.rippleTexCoord01.zw = mul( texMat, OUT.rippleTexCoord01.zw );         

   OUT.rippleTexCoord2.xy = txPos * rippleTexScale[2].xy;
   OUT.rippleTexCoord2.xy += rippleDir[2].xy * elapsedTime.x * rippleSpeed.z; 

   texMat[0][0] = rippleMat[2].x;
   texMat[0][1] = rippleMat[2].y;
   texMat[1][0] = rippleMat[2].z;
   texMat[1][1] = rippleMat[2].w;
   OUT.rippleTexCoord2.xy = mul( texMat, OUT.rippleTexCoord2.xy );
   
   OUT.foamTexCoords.xy = txPos * foamTexScale.xy + foamDir.xy * foamSpeed.x * elapsedTime.x;
   OUT.foamTexCoords.zw = txPos * foamTexScale.zw + foamDir.zw * foamSpeed.y * elapsedTime.x;
   
   float3 binormal = float3( 0, 0, 1 );
   float3 tangent = float3( 1, 0, 0 );
   float3 normal;
   
   for ( int i = 0; i < 3; i++ )
   {
      tangent.y += waveDir[i].x * waveData[i].y * cos( waveDir[i].x * undulatePos.x + waveDir[i].y * undulatePos.y + elapsedTime.x * waveData[i].x );
	  binormal.y += waveDir[i].y * waveData[i].y * cos( waveDir[i].x * undulatePos.x + waveDir[i].y * undulatePos.y + elapsedTime.x * waveData[i].x );
   }
   
   normal = cross( binormal, tangent);
   
   OUT.tangentMat = float3x3( normalize( tangent ),
										normalize( binormal ),
										normalize( normal ) );
   
   float3 positionVS = mul(IN.pos, matWV).xyz;
   OUT.ray_view = float3(positionVS.xy / positionVS.z, 1.0);
   
   return OUT;
}


////////////////////////////////////////////////////////////////////////
Texture2D<float4> bumpMap : register(t0);
Texture2D<float4>     refractBuff : register(t1);
TextureCube  		skyMap :register(t2);
Texture2D<float4>      foamMap : register(t3);
Texture2D<float> 	depth_texture : register(t4);
Texture2D<float4> depthGradMap : register(t5);

SamplerState linearSampler : register(s0);

cbuffer WaterPSParams
{
	float4       specularParams;
	float4       baseColor;
	float4       miscParams;
	float4       farPlaneDist;
	float4       distortionParams;
	float4       ambientColor;
	float4       eyePosW;
	float4       rippleMagnitude;
	float4       depthGradMax;
	float4       inLightVec;
	float4	     sunColor;
	float4       reflectivity;
	float4       foamParams;
	float4       fogParams;
};


float3 PositionFromDepth( in float zBufferDepth, in float3 viewRay )
{
	// Convert to a linear depth value using the projection matrix
	float linearDepth = matProj[3][2] / (zBufferDepth - matProj[2][2]);
	return viewRay * linearDepth;
}

float4 tex2D(Texture2D<float4> tex, float2 tex_coord)
{
	return tex.Sample(linearSampler, tex_coord);
}

float4 tex2D(Texture2D<float> tex, float2 tex_coord)
{
	return tex.Sample(linearSampler, tex_coord);
}

float4 tex1D(Texture1D<float4> tex, float tex_coord)
{
	return tex.Sample(linearSampler, tex_coord);
}

float4 texCUBE(TextureCube tex, float3 tex_coord)
{
	return tex.Sample(linearSampler, tex_coord);
}

float computeSceneFog( float dist, float fogDensity, float fogDensityOffset )
{
   float f = dist - fogDensityOffset;
   return exp( -fogDensity * f );
}

float fresnel(float NdotV, float bias, float power)
{
   return bias + (1.0-bias)*pow(abs(1.0 - max(NdotV, 0)), power);
}

float2 toTexCoord(float4 h)
{
	float2 tex_coord;
	tex_coord.x = (h.x / h.w + 1.0) * 0.5;
	tex_coord.y = (1.0 - h.y / h.w) * 0.5;
	
	return tex_coord;
}

#define PIXEL_DEPTH		IN.rippleTexCoord2.z	
#define FOAM_OPACITY     foamParams[0]

// fogParams
#define FOG_DENSITY        fogParams[0]
#define FOG_DENSITY_OFFSET fogParams[1]

#define FRESNEL_BIAS       miscParams[0]
#define FRESNEL_POWER      miscParams[1]

#define SPEC_POWER         specularParams[3]
#define SPEC_COLOR         specularParams.xyz

#define FOAM_OPACITY       		foamParams[0]
#define FOAM_MAX_DEPTH     		foamParams[1]
#define FOAM_AMBIENT_LERP  		foamParams[2]
#define FOAM_RIPPLE_INFLUENCE 	foamParams[3]

float4 PSMAIN(in VS_OUTPUT IN) : SV_Target
{
	int3 sampleCoord = int3(IN.hpos.xy, 0);
	float zbufferValue = depth_texture.Load(sampleCoord).r;
	float3 position_wvs = PositionFromDepth(zbufferValue, IN.ray_view);
	float linear_depth = position_wvs.z;
	
	float3 bumpNorm = ( tex2D( bumpMap, IN.rippleTexCoord01.xy ).rbg * 2.0 - 1.0 ) * rippleMagnitude.x;
	bumpNorm       += ( tex2D( bumpMap, IN.rippleTexCoord01.zw ).rbg * 2.0 - 1.0 ) * rippleMagnitude.y;      
    bumpNorm       += ( tex2D( bumpMap, IN.rippleTexCoord2.xy ).rbg * 2.0 - 1.0 ) * rippleMagnitude.z;
	
	bumpNorm = normalize( bumpNorm );
    bumpNorm = lerp( bumpNorm, float3(0,1,0), 1.0 - rippleMagnitude.w );
	//bumpNorm = mul( bumpNorm, IN.tangentMat );

	float4 temp = IN.posPreWave;
	float4 temp2 = IN.posPostWave;
	temp.xy += bumpNorm.xy;
	temp2.xy += bumpNorm.xy;
	float2 reflectCoord = toTexCoord(temp);
	float2 refractCoord = toTexCoord(temp2);
	
	float4 fakeColor = float4(ambientColor.rgb,1);
	float3 eyeVec = IN.positionW.xyz - eyePosW.xyz;
	eyeVec = normalize( eyeVec );
	float3 reflectionVec = reflect( normalize(eyeVec), bumpNorm );
	
	// Use fakeColor for ripple-normals that are angled towards the camera   
   eyeVec = -eyeVec;
   eyeVec = normalize( eyeVec );
   float ang = saturate( dot( eyeVec, bumpNorm ) );   
   float fakeColorAmt = ang; 
   
#ifndef UNDERWATER
	float2 foamRippleOffset = bumpNorm.xy * FOAM_RIPPLE_INFLUENCE;
   IN.foamTexCoords.xy += foamRippleOffset; 
   IN.foamTexCoords.zw += foamRippleOffset;
   
   float4 foamColor = tex2D( foamMap, IN.foamTexCoords.xy );   
   foamColor += tex2D( foamMap, IN.foamTexCoords.zw ); 
   foamColor = saturate( foamColor );
   
   // Modulate foam color by ambient color
   // so we don't have glowing white foam at night.
   foamColor.rgb = lerp( foamColor.rgb, ambientColor.rgb, FOAM_AMBIENT_LERP );
   
   float delta = linear_depth - PIXEL_DEPTH;
   delta = max(delta, 0);
   
   float foamDelta = saturate( delta / FOAM_MAX_DEPTH );      
   float foamAmt = 1 - pow( foamDelta, 2 );
   foamAmt *= FOAM_OPACITY * foamColor.a;
   
   foamColor.rgb *= FOAM_OPACITY * foamAmt * foamColor.a;
   
   fakeColor = ( texCUBE(skyMap , reflectionVec ) );
   fakeColor.a = 1;
   
   float4 reflectColor = fakeColor;
   
   // Get refract color
   float4 refractColor = tex2D( refractBuff, refractCoord );
   
   // We darken the refraction color a bit to make underwater 
   // elements look wet.  We fade out this darkening near the
   // surface in order to not have hard water edges.
   // @param WET_DEPTH The depth in world units at which full darkening will be recieved.
   // @param WET_COLOR_FACTOR The refract color is scaled down by this amount when at WET_DEPTH
   // refractColor.rgb *= 1.0f - ( saturate( delta / WET_DEPTH ) * WET_COLOR_FACTOR );
   
   // Add Water fog/haze.
   float fogDelta = delta - FOG_DENSITY_OFFSET;

   if ( fogDelta < 0.0 )
      fogDelta = 0.0;     
   float fogAmt = 1.0 - saturate( exp( -FOG_DENSITY * fogDelta )  );  
   

   
   // Calculate the water "base" color based on depth.
   float depthDelta = saturate( delta / depthGradMax.x );
   float4 waterBaseColor = baseColor * tex2D( depthGradMap, float2(depthDelta, 0.5));
   
   // Modulate baseColor by the ambientColor.
   waterBaseColor *= float4( ambientColor.rgb, 1 );
   
   // calc "diffuse" color by lerping from the water color
   // to refraction image based on the water clarity.   
   float4 diffuseColor = lerp( refractColor, waterBaseColor, fogAmt );
   
   // fresnel calculation   
   float fresnelTerm = fresnel( ang, FRESNEL_BIAS, FRESNEL_POWER );	
   
   // Scale the frensel strength by fog amount
   // so that parts that are very clear get very little reflection.
   fresnelTerm *= fogAmt;
   fresnelTerm *= reflectivity.x;
   
   // Combine the diffuse color and reflection image via the
   // fresnel term and set out output color.
   float4 OUT = lerp( diffuseColor, reflectColor, fresnelTerm );
   float3 lightVec = inLightVec.xyz;

   // Get some specular reflection.
   float3 newbump = normalize( bumpNorm );
   
   float3 halfAng = normalize( eyeVec + -lightVec );
   float specular = saturate( dot( newbump, halfAng ) );
   specular = pow( specular, SPEC_POWER );   
   
   specular *= saturate( delta / 2 );
   OUT.rgb = OUT.rgb + ( SPEC_COLOR * specular.xxx );      
#else
	float4 refractColor = tex2D( refractBuff, refractCoord );
	float4 OUT = refractColor;  
#endif



#ifndef UNDERWATER

   OUT.rgb = OUT.rgb + foamColor.rgb;

   /*
   float factor = computeSceneFog( eyePosW, 
                                   IN.positionW.xyz, 
                                   IN.positionW.w,
                                   fogData.x,
                                   fogData.y,
                                   fogData.z );

   OUT.rgb = lerp( OUT.rgb, fogColor.rgb, 1.0 - saturate( factor ) );
	*/
#endif


	OUT.a = 1.0;
	
	return OUT;
}