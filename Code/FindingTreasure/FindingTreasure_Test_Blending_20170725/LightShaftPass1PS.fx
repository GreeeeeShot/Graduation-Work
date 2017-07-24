#include "Light.fx"

const float uExposure = 0.5;
const float uDecayWeight = 0.95;
const float uShaftWeight = 0.8;

const int NUM_SAMPLES = 100;

// V * P(카메라) * T(텍스쳐)
cbuffer cbLightSourcePos : register(b0)
{
	float2 gfLightSourcePos;
};

struct VS_LIGHT_SHAFT_PASS1_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

Texture2D gLightMap : register(t0);
SamplerState gSamplerState : register(s0);

float4 PSLightShaftPass1(VS_LIGHT_SHAFT_PASS1_OUTPUT input) : SV_Target
{
	float2 stepTexPos = (gfLightSourcePos - input.tex2dcoord) / float(NUM_SAMPLES);
	float2 currTextPos = input.tex2dcoord;
	float illuminationDecay = 1.0;

	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	/*
	[unroll(100)]
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		currTextPos += stepTexPos;
		float4 sampleColor = gLightMap.Sample(gSamplerState, currTextPos);

		sampleColor *= illuminationDecay * uShaftWeight;

		cColor += sampleColor;

		illuminationDecay *= uDecayWeight;
	}*/
	return cColor = gLightMap.Sample(gSamplerState, currTextPos);//*= uExposure;
}
