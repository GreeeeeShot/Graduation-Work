cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);					// 카메라 기준
	matrix gmtxProjection : packoffset(c4);				// 원근 투영
};

struct VS_INST_LIGHT_SHAFT_PASS0_INPUT
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

struct VS_INST_LIGHT_SHAFT_PASS0_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
	float voxelType : INST_VOXEL_TYPE;
};

VS_INST_LIGHT_SHAFT_PASS0_OUTPUT InstVSLightShaftPass0(VS_INST_LIGHT_SHAFT_PASS0_INPUT input)
{
	VS_INST_LIGHT_SHAFT_PASS0_OUTPUT output = (VS_INST_LIGHT_SHAFT_PASS0_OUTPUT)0;
	matrix mtxWorldViewProjection = mul(input.mtxWorld, gmtxView);
	mtxWorldViewProjection = mul(mtxWorldViewProjection, gmtxProjection);
	output.position = mul(float4(input.position, 1.0f), mtxWorldViewProjection);
	output.tex2dcoord = input.tex2dcoord;

	output.voxelType = input.voxelType;

	return(output);
}
