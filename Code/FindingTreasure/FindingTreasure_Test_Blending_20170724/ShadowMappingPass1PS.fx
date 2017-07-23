#include "Light.fx"
/*
SamplerComparisonState samShadow
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ComparisonFunc = LESS;
};*/

struct VS_SHADOW_MAPPING_PASS1_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tex2dcoord : TEXCOORD0;
	float4 shadowPosH : TEXCOORD1;
};

struct PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT		
{
	float4 cToShadow : SV_Target0;
	float4 cToLightShaft : SV_Target1;
};

Texture2D gtxtTexture : register(t0);						
SamplerState gSamplerState : register(s0);

Texture2D gShadowMap : register(t1);				// pass0에서 만든 그림자 맵
SamplerState gShadowMapSampler : register(s1);



PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT PSShadowMappingPass1(VS_SHADOW_MAPPING_PASS1_OUTPUT input)
{
	PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT output = (PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT)0;
	input.normalW = normalize(input.normalW);
	float shadow = CalcShadowFactor(gShadowMapSampler, gShadowMap, input.shadowPosH);
	//shadow = gShadowMap.Sample(gSamplerState, input.shadowPosH.xy / input.shadowPosH.w).r < input.shadowPosH.z/ input.shadowPosH.w ? 0.36f: 1.0f;

	// 조명 계산
	LIGHTEDCOLOR breakLighting;
	BreakLighting(input.positionW, input.normalW, breakLighting);

	breakLighting.m_cDiffuse *= shadow;
	breakLighting.m_cSpecular *= shadow;

	float4 cIllumination = (breakLighting.m_cAmbient + breakLighting.m_cDiffuse + breakLighting.m_cSpecular);// * shadow;
	cIllumination += (gcLightGlobalAmbient * gMaterial.m_cAmbient);

	//최종 색상의 알파값은 재질의 디퓨즈 색상의 알파값으로 설정한다.
	cIllumination.a = gMaterial.m_cDiffuse.a;

	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord) * cIllumination;//;

	clip(cColor.a - 0.15f);
	
	if (gFogEnable)
	{
		float fFogLerp = saturate((distance(gvCameraPosition.xyz, input.positionW) - gFogStart) / gFogRange);

		// 안개 색상과 조명된 색상을 섞는다.
		cColor = lerp(cColor, gFogColor, fFogLerp);
	}

	output.cToShadow = cColor;

	// 라이트 셰프트를 위한 것.
	if (cColor.g < 1.0)
		output.cToLightShaft = float4(0.0f, 0.0f, 0.0f, 0.0f);
	else
		output.cToLightShaft = cColor;

	
	return output;
}
