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
// File: common.fxh
//

// input
struct COMM_VS_INPUT
{
	float4 position				: POSITION;
	float3 normal				: NORMAL;
	
#ifdef HAS_NORMALMAP
	float4 tangent				: TANGENT;
#endif
	
#ifdef HAS_UV1
	float2 tex0					: TEXCOORD0;
#endif

#ifdef HAS_UV2
	float2 tex2					: TEXCOORD1;
#endif

#ifdef HAS_SKELETON
	uint4 bone_indices			: BLENDINDICES0;
	float4 bone_weights 		: BLENDWEIGHT0;
#endif
};

float toLumin( float3 clr )
{
    // Y = 0.2126 R + 0.7152 G + 0.0722 B
    return dot(float3(0.2126, 0.7152, 0.0722), clr);
}

float square(float v)
{
	return v * v;
}

/// Calculate fog based on a start and end positions in worldSpace.
float computeSceneFog(  float3 startPos,
                        float3 endPos,
                        float fogDensity,
                        float fogDensityOffset,
                        float fogHeightFalloff )
{      
   float f = length( startPos - endPos ) - fogDensityOffset;
   float h = 1.0 - ( endPos.z * fogHeightFalloff );  
   return exp( -fogDensity * f * h );  
}

#define M_PI 3.14159