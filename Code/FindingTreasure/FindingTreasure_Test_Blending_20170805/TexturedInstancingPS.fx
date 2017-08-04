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

struct VS_TEXTURED_LIGHTING_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
	float voxelType : INST_VOXEL_TYPE;
	float4 mtrlDiffuse : INST_MTRL_DIFFUSE;
	float4 mtrlAmbient : INST_MTRL_AMBIENT;
	float4 mtrlSpecular: INST_MTRL_SPECULAR;
	float4 mtrlEmissive: INST_MTRL_EMISSIVE;
};

Texture2DArray gtxtTextureArray : register(t0);
//SamplerState gSamplerState : register(s0);

float4 InstPSTexturedLighting(VS_TEXTURED_LIGHTING_INSTANCING_OUTPUT input) : SV_Target
{
	float4 clipColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float3 uvw = float3(input.tex2dcoord, input.voxelType);

	clipColor = gtxtTextureArray.Sample(gSamplerState, uvw);
	clip(clipColor.a - 0.01f);

	input.normalW = normalize(input.normalW);
	float4 cIllumination = InstLighting(input.positionW, input.normalW, input.mtrlDiffuse, input.mtrlAmbient, input.mtrlSpecular, input.mtrlEmissive);
	float4 cColor = gtxtTextureArray.Sample(gSamplerState, uvw) * cIllumination;

	if (gFogEnable)
	{
		float fFogLerp = saturate((distance(gvCameraPosition.xyz, input.positionW) - gFogStart) / gFogRange);

		// 안개 색상과 조명된 색상을 섞는다.
		cColor = lerp(cColor, gFogColor, fFogLerp);
	}

	return(cColor);
}
