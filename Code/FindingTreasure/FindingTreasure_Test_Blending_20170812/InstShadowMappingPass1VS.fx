cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);					// 카메라 기준
	matrix gmtxProjection : packoffset(c4);				// 원근 투영
};

//cbuffer cbWorldMatrix : register(b1)
//{
//	matrix gmtxWorld : packoffset(c0);					// 복셀
//};

// 그림자 행렬 VxPxT
cbuffer cbShadowMatrix : register(b2)
{
	matrix gmtxShadow : packoffset(c0);						// 이때 P는 직교투영
}

struct VS_INST_SHADOW_MAPPING_PASS1_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
	float voxelType : INST_VOXEL_TYPE;
	column_major float4x4 mtxWorld : INST_WORLD;
	float4 mtrlDiffuse : INST_MTRL_DIFFUSE;
	float4 mtrlAmbient : INST_MTRL_AMBIENT;
	float4 mtrlSpecular: INST_MTRL_SPECULAR;
	float4 mtrlEmissive: INST_MTRL_EMISSIVE;
};

struct VS_INST_SHADOW_MAPPING_PASS1_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
	float4 shadowPosH : TEXCOORD1;
	float voxelType : INST_VOXEL_TYPE;
	float4 mtrlDiffuse : INST_MTRL_DIFFUSE;
	float4 mtrlAmbient : INST_MTRL_AMBIENT;
	float4 mtrlSpecular: INST_MTRL_SPECULAR;
	float4 mtrlEmissive: INST_MTRL_EMISSIVE;
};

VS_INST_SHADOW_MAPPING_PASS1_OUTPUT InstVSShadowMappingPass1(VS_INST_SHADOW_MAPPING_PASS1_INPUT input)
{
	VS_INST_SHADOW_MAPPING_PASS1_OUTPUT output = (VS_INST_SHADOW_MAPPING_PASS1_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)input.mtxWorld);
	output.positionW = mul(input.position, (float3x3)input.mtxWorld);
	output.positionW += float3(input.mtxWorld._41, input.mtxWorld._42, input.mtxWorld._43);
	matrix mtxWorldViewProjection = mul(input.mtxWorld, gmtxView);
	mtxWorldViewProjection = mul(mtxWorldViewProjection, gmtxProjection);
	output.position = mul(float4(input.position, 1.0f), mtxWorldViewProjection);
	output.tex2dcoord = input.tex2dcoord;
	output.shadowPosH = mul(float4(output.positionW, 1.0f), gmtxShadow);

	output.voxelType = input.voxelType;
	//재질
	output.mtrlDiffuse = input.mtrlDiffuse;
	output.mtrlAmbient = input.mtrlAmbient;
	output.mtrlSpecular = input.mtrlSpecular;
	output.mtrlEmissive = input.mtrlEmissive;

	return(output);
}
