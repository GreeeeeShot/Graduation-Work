struct VS_SHADOW_MAPPING_PASS0_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

Texture2D gtxtTexture : register(t0);											// ������ �ؼ��̴�.
SamplerState gSamplerState : register(s0);										// ���÷�

void PSShadowMappingPass0(VS_SHADOW_MAPPING_PASS0_OUTPUT input)
{
	float4 cDiffuse = gtxtTexture.Sample(gSamplerState, input.tex2dcoord);		// ������ �ؽ��� �ؼ� �� ����

	clip(cDiffuse.a - 0.15f);													// ������ ���� �Ⱒ�Ѵ�.
}
