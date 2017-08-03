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

struct VS_INST_SHADOW_MAPPING_PASS0_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
	float voxelType : INST_VOXEL_TYPE;
};

Texture2DArray gtxtTextureArray : register(t0);										// ������ �ؼ��̴�.
//SamplerState gSamplerState : register(s0);										// ���÷�

void InstPSShadowMappingPass0(VS_SHADOW_MAPPING_PASS0_OUTPUT input)
{
	float3 uvw = float3(input.tex2dcoord, input.voxelType);
	float4 cDiffuse = gtxtTextureArray.Sample(gSamplerState, uvw);		// ������ �ؽ��� �ؼ� �� ����

	clip(cDiffuse.a - 0.15f);													// ������ ���� �Ⱒ�Ѵ�.
}
