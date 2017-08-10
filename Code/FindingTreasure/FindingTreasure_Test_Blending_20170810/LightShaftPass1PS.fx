#include "Light.fx"

const int NUM_SAMPLES = 1;

SamplerState gSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;

	ComparisonFunc = NEVER;
	MinLOD = 0;
	MaxLOD = 0;
};

// V * P(카메라) * T(텍스쳐)
cbuffer cbLightSourcePos : register(b3)
{
	float3 gfLightSourcePos;
	float pad;
};

struct VS_LIGHT_SHAFT_PASS1_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

Texture2D gLightMap : register(t0);
//SamplerState gSamplerState : register(s0);

float4 PSLightShaftPass1(VS_LIGHT_SHAFT_PASS1_OUTPUT input) : SV_Target0
{
	if(gfLightSourcePos.z < 0 || gfLightSourcePos.z > 1) clip(-1);
	float2 stepTexPos = (gfLightSourcePos.xy - input.tex2dcoord.xy) / float(100);
	float2 currTextPos = input.tex2dcoord;
	float illuminationDecay = 0.9;
	float uExposure = 0.065;
	float uDecayWeight = 0.85;
	float uShaftWeight = 0.9;

	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 sampleColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	/**/
	[unroll]
	for (int i = 0; i < 100; i++)
	{
		sampleColor = gLightMap.Sample(gSampler, currTextPos);

		sampleColor *= illuminationDecay * uShaftWeight;

		cColor += sampleColor;
		currTextPos += stepTexPos;
		illuminationDecay *= uDecayWeight;
	}
	
	cColor *= uExposure;

	cColor.a = 1.0f;
	return cColor;// gLightMap.Sample(gSampler, currTextPos); // cColor;
}
