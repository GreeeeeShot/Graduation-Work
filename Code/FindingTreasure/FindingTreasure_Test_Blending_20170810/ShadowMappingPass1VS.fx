cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);					// 카메라 기준
	matrix gmtxProjection : packoffset(c4);				// 원근 투영
};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);					// 복셀
};

// 그림자 행렬 VxPxT
cbuffer cbShadowMatrix : register(b2)			
{
	matrix gmtxShadow : packoffset(c0);						// 이때 P는 직교투영
}

struct VS_SHADOW_MAPPING_PASS1_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_SHADOW_MAPPING_PASS1_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
	float4 shadowPosH : TEXCOORD1;
};

VS_SHADOW_MAPPING_PASS1_OUTPUT VSShadowMappingPass1(VS_SHADOW_MAPPING_PASS1_INPUT input)
{
	VS_SHADOW_MAPPING_PASS1_OUTPUT output = (VS_SHADOW_MAPPING_PASS1_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.positionW = mul(input.position, (float3x3)gmtxWorld);
	output.positionW += float3(gmtxWorld._41, gmtxWorld._42, gmtxWorld._43);
	matrix mtxWorldViewProjection = mul(gmtxWorld, gmtxView);
	mtxWorldViewProjection = mul(mtxWorldViewProjection, gmtxProjection);
	output.position = mul(float4(input.position, 1.0f), mtxWorldViewProjection);
	output.tex2dcoord = input.tex2dcoord;
	output.shadowPosH = mul(float4(output.positionW, 1.0f), gmtxShadow);

	return(output);
}
