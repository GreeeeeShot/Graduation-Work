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

SamplerState gSamplerState
{
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;

	ComparisonFunc = NEVER;
	MinLOD = 0;
	MaxLOD = 0;
};

struct VS_INST_SHADOW_MAPPING_PASS1_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tex2dcoord : TEXCOORD0;
	float4 shadowPosH : TEXCOORD1;
	float voxelType : INST_VOXEL_TYPE;
	float4 mtrlDiffuse : INST_MTRL_DIFFUSE;
	float4 mtrlAmbient : INST_MTRL_AMBIENT;
	float4 mtrlSpecular: INST_MTRL_SPECULAR;
	float4 mtrlEmissive: INST_MTRL_EMISSIVE;
};

struct PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT
{
	float4 cToShadow : SV_Target0;
};

Texture2D gtxtTexture : register(t0);
//SamplerState gSamplerState : register(s0);

Texture2D gShadowMap : register(t1);				// pass0에서 만든 그림자 맵
SamplerState gShadowMapSampler : register(s1);



PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT InstPSShadowMappingPass1(VS_INST_SHADOW_MAPPING_PASS1_OUTPUT input)
{
	VS_INST_SHADOW_MAPPING_PASS1_OUTPUT output = (VS_INST_SHADOW_MAPPING_PASS1_OUTPUT)0;
	input.normalW = normalize(input.normalW);
	float shadow = CalcShadowFactor(gShadowMapSampler, gShadowMap, input.shadowPosH);
	//shadow = gShadowMap.Sample(gSamplerState, input.shadowPosH.xy / input.shadowPosH.w).r < input.shadowPosH.z/ input.shadowPosH.w ? 0.36f: 1.0f;

	// 조명 계산
	LIGHTEDCOLOR breakLighting;
	InstBreakLighting(input.positionW, input.normalW, input.mtrlDiffuse, input.mtrlAmbient, input.mtrlSpecular, input.mtrlEmissive, breakLighting);

	breakLighting.m_cDiffuse *= shadow;
	breakLighting.m_cSpecular *= shadow;

	float4 cIllumination = (breakLighting.m_cAmbient + breakLighting.m_cDiffuse + breakLighting.m_cSpecular);// * shadow;
	cIllumination += (gcLightGlobalAmbient * input.mtrlAmbient);

	//최종 색상의 알파값은 재질의 디퓨즈 색상의 알파값으로 설정한다.
	cIllumination.a = input.mtrlDiffuse.a;

	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord) * cIllumination;//;

	clip(cColor.a - 0.15f);

	if (gFogEnable)
	{
		float fFogLerp = saturate((distance(gvCameraPosition.xyz, input.positionW) - gFogStart) / gFogRange);

		// 안개 색상과 조명된 색상을 섞는다.
		cColor = lerp(cColor, gFogColor, fFogLerp);
	}

	output.cToShadow = cColor;
	//output.cToLightShaft = float4(0.0f, 0.0f, 0.0f, cIllumination.a);



	return output;
}
