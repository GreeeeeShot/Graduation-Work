#include "Light.fx"

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

struct VS_LIGHT_SHAFT_PASS2_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

Texture2D gLightShaftMap : register(t0);
SamplerState gSamplerState : register(s0);

float4 PSLightShaftPass2(VS_LIGHT_SHAFT_PASS2_OUTPUT input) : SV_Target0
{
	float4 cColor;

	cColor = gLightShaftMap.Sample(gSampler, input.tex2dcoord);

	return cColor;
}
