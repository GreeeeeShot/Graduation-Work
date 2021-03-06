cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);				// ���� ����
	matrix gmtxProjection : packoffset(c4);				// ���� ����
};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);				// ����
};

struct VS_SHADOW_MAPPING_PASS0_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_SHADOW_MAPPING_PASS0_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

VS_SHADOW_MAPPING_PASS0_OUTPUT VSShadowMappingPass0(VS_SHADOW_MAPPING_PASS0_INPUT input)
{
	VS_SHADOW_MAPPING_PASS0_OUTPUT output = (VS_SHADOW_MAPPING_PASS0_OUTPUT)0;
	matrix mtxWorldViewProjection = mul(gmtxWorld, gmtxView);
	mtxWorldViewProjection = mul(mtxWorldViewProjection, gmtxProjection);
	output.position = mul(float4(input.position, 1.0f), mtxWorldViewProjection);
	output.tex2dcoord = input.tex2dcoord;

	return(output);
}
