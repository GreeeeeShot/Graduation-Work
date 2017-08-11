#include "Light.fx"
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
struct VS_INST_LIGHT_SHAFT_PASS0_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
	float voxelType : INST_VOXEL_TYPE;
};


struct PS_SHADOW_AND_LIGHT_SHAFT_MAPPING_PASS1_OUTPUT
{
	float4 cToShadow : SV_Target0;
	//float4 cToLightShaft : SV_Target1;              // 멀티 렌더 타겟을 수행하기 위한 출력 구조체
};

Texture2DArray gtxtTextureArray : register(t0);
//SamplerState gSamplerState : register(s0);

float4 InstPSLightShaftPass0(VS_INST_LIGHT_SHAFT_PASS0_OUTPUT input) : SV_Target
{
	float4 output = (float4)0;

	float3 uvw = float3(input.tex2dcoord, input.voxelType);
	float4 cColor = gtxtTextureArray.Sample(gSamplerState, uvw);
	clip(cColor.a - 0.15f);

	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
