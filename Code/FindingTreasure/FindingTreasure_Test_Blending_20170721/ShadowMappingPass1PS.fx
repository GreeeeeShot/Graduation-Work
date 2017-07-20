#include "Light.fx"

SamplerComparisonState samShadow
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ComparisonFunc = LESS_EQUAL;
};

struct VS_SHADOW_MAPPING_PASS1_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tex2dcoord : TEXCOORD0;
	float4 shadowPosH : TEXCOORD1;
};

Texture2D gtxtTexture : register(t0);						
SamplerState gSamplerState : register(s0);

Texture2D gShadowMap : register(t1);				// pass0���� ���� �׸��� ��

float4 PSShadowMappingPass1(VS_SHADOW_MAPPING_PASS1_OUTPUT input) : SV_TARGET
{
	input.normalW = normalize(input.normalW);
	float shadow = CalcShadowFactor(samShadow, gShadowMap, input.shadowPosH);

	// ���� ���
	LIGHTEDCOLOR breakLighting;
	BreakLighting(input.positionW, input.normalW, breakLighting);

	breakLighting.m_cDiffuse  *= shadow;
	breakLighting.m_cSpecular *= shadow;

	float4 cIllumination = breakLighting.m_cAmbient + breakLighting.m_cDiffuse + breakLighting.m_cSpecular;
	cIllumination += (gcLightGlobalAmbient * gMaterial.m_cAmbient);

	//���� ������ ���İ��� ������ ��ǻ�� ������ ���İ����� �����Ѵ�.
	cIllumination.a = gMaterial.m_cDiffuse.a;

	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord) * cIllumination;

	if (gFogEnable)
	{
		float fFogLerp = saturate((distance(gvCameraPosition.xyz, input.positionW) - gFogStart) / gFogRange);

		// �Ȱ� ����� ����� ������ ���´�.
		cColor = lerp(cColor, gFogColor, fFogLerp);
	}

	return(cColor);
}
