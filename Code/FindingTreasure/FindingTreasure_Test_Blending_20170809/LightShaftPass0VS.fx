cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);					// 카메라 기준
	matrix gmtxProjection : packoffset(c4);				// 원근 투영
};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);					// 복셀
};

struct VS_LIGHT_SHAFT_PASS0_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_LIGHT_SHAFT_PASS0_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

VS_LIGHT_SHAFT_PASS0_OUTPUT VSLightShaftPass0(VS_LIGHT_SHAFT_PASS0_INPUT input)
{
	VS_LIGHT_SHAFT_PASS0_OUTPUT output = (VS_LIGHT_SHAFT_PASS0_OUTPUT)0;
	matrix mtxWorldViewProjection = mul(gmtxWorld, gmtxView);
	mtxWorldViewProjection = mul(mtxWorldViewProjection, gmtxProjection);
	output.position = mul(float4(input.position, 1.0f), mtxWorldViewProjection);
	output.tex2dcoord = input.tex2dcoord;

	return(output);
}
