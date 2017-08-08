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

struct VS_LIGHT_SHAFT_PASS0_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};


struct PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT
{
	float4 cToShadow : SV_Target0;
	//float4 cToLightShaft : SV_Target1;              // 멀티 렌더 타겟을 수행하기 위한 출력 구조체
};

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

float4 PSLightShaftPass0(VS_LIGHT_SHAFT_PASS0_OUTPUT input) : SV_Target
{
	PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT output = (PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT)0;

	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord);
	clip(cColor.a - 0.15f);

	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
