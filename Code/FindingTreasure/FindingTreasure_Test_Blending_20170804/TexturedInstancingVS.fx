cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

/*cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};*/

struct VS_TEXTURED_LIGHTING_INSTANCING_INPUT
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

VS_TEXTURED_LIGHTING_INSTANCING_OUTPUT InstVSTexturedLight(VS_TEXTURED_LIGHTING_INSTANCING_INPUT input)
{
	VS_TEXTURED_LIGHTING_INSTANCING_OUTPUT output = (VS_TEXTURED_LIGHTING_INSTANCING_OUTPUT)0;


	output.normalW = mul(input.normal, (float3x3)input.mtxWorld);

	output.positionW = mul(input.position, (float3x3)input.mtxWorld);
	output.positionW += float3(input.mtxWorld._41, input.mtxWorld._42, input.mtxWorld._43);

	matrix mtxWorldViewProjection = mul(input.mtxWorld, gmtxView);
	mtxWorldViewProjection = mul(mtxWorldViewProjection, gmtxProjection);
	output.position = mul(float4(input.position, 1.0f), mtxWorldViewProjection);

	output.tex2dcoord = input.tex2dcoord;
	
	//재질
	output.mtrlDiffuse = input.mtrlDiffuse;
	output.mtrlAmbient = input.mtrlAmbient;
	output.mtrlSpecular = input.mtrlSpecular;
	output.mtrlEmissive = input.mtrlEmissive;
	
	// 복셀 타입
	output.voxelType = input.voxelType;
	return(output);
}
