#include "Light.fx"

struct VS_LIGHT_SHAFT_PASS2_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

Texture2D gLightShaftMap : register(t0);
SamplerState gSamplerState : register(s0);

float4 PSLightShaftPass2(VS_LIGHT_SHAFT_PASS2_OUTPUT input) : SV_Target
{
	float4 cColor;

	cColor = gLightShaftMap.Sample(gSamplerState, input.tex2dcoord);

	return cColor;
}
