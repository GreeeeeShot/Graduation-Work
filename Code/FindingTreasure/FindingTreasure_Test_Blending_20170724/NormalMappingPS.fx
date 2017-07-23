#include "Light.fx"

struct VS_TEXTURED_NORMAL_MAPPING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tex2dcoord : TEXCOORD0;
	float3 tbasis : TBASIS;
};

// Texture
Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

// NormalMap
Texture2D gnormTexture : register(t1);
SamplerState gNormState : register(s1);

float4 PSNormalMapping(VS_TEXTURED_NORMAL_MAPPING_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float3 vUNormBasedTex = gnormTexture.Sample(gNormState, float2(input.tex2dcoord.x*1.0f + input.tex2dcoord.z*0.03f, input.tex2dcoord.y*1.0f * 0.5f));
	vUNormBasedTex += gnormTexture.Sample(gNormState, float2(input.tex2dcoord.x*0.8f- input.tex2dcoord.z*0.02f, input.tex2dcoord.y * 0.8f));
	vUNormBasedTex += gnormTexture.Sample(gNormState, float2(input.tex2dcoord.x*0.6f - input.tex2dcoord.z*0.01f, input.tex2dcoord.y * 0.6f));
	vUNormBasedTex = normalize(vUNormBasedTex);
	//vUNormBasedTex += gnormTexture.Sample(gNormState, float2(input.tex2dcoord.y*1.5f * 0.5f, input.tex2dcoord.x*1.5f + input.tex2dcoord.z*0.20f));
	//vUNormBasedTex.yz = vUNormBasedTex.zy;
	input.normalW = ConvertNormalBasedOnTBN(vUNormBasedTex, input.normalW, input.tbasis);
	float4 cIllumination = Lighting(input.positionW, input.normalW) * 1.2f;
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord) * cIllumination;

	if (gFogEnable)
	{
		float fFogLerp = saturate((distance(gvCameraPosition.xyz, input.positionW) - gFogStart) / gFogRange);

		// 안개 색상과 조명된 색상을 섞는다.
		cColor = lerp(cColor, gFogColor, fFogLerp);
	}

	return(cColor);
}
