#include "Light.fx"

struct VS_TEXTURED_NOT_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

float4 PSTexturedNotLighting(VS_TEXTURED_NOT_LIGHTING_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord);

	if (gFogEnable)
	{
		float fFogLerp = saturate((distance(gvCameraPosition.xyz, input.positionW) - gFogStart) / gFogRange);

		// 안개 색상과 조명된 색상을 섞는다.
		cColor = lerp(cColor, gFogColor, fFogLerp);
	}
	return(cColor);
}
