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
	float4 clipColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	clipColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord);
	clip(clipColor.a - 0.01f);

	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord) * cIllumination;

	if (gFogEnable)
	{
		float fFogLerp = saturate((distance(gvCameraPosition.xyz, input.positionW) - gFogStart) / gFogRange);

		// 안개 색상과 조명된 색상을 섞는다.
		cColor = lerp(cColor, gFogColor, fFogLerp);
	}

	return(cColor);
}
