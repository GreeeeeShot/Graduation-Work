#include "Light.fx"

struct VS_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord) * cIllumination;

	return(cColor);
}
