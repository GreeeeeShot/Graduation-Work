#include "Light.fx"

cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

//������ ����ϴ� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

//������ ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	//������ǥ�迡�� ������ ��ġ�� ���� ���͸� ��Ÿ����.
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

//�ν��Ͻ��� �ϸ鼭 ������ ����ϴ� ��� ���� ���̴��� �Է��� ���� ����ü�̴�.
struct VS_INSTANCED_LIGHTING_COLOR_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4x4 mtxTransform : INSTANCEPOS;
};

//�ν��Ͻ��� �ϸ鼭 ������ ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_INSTANCED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	return output;
}

//������ ������ ����ϱ� ���� ������ ���� ���Ϳ� ������ ��ġ�� ����ϴ� ���� ���̴� �Լ��̴�.
VS_LIGHTING_COLOR_OUTPUT VSLightingColor(VS_LIGHTING_COLOR_INPUT input)
{
	VS_LIGHTING_COLOR_OUTPUT output = (VS_LIGHTING_COLOR_OUTPUT)0;
	//������ ������ ����ϱ� ���Ͽ� ������ǥ�迡�� ������ ��ġ�� ���� ���͸� ���Ѵ�.
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}

VS_INSTANCED_LIGHTING_COLOR_OUTPUT VSInstancedLightingColor(VS_INSTANCED_LIGHTING_COLOR_INPUT input)
{
	VS_INSTANCED_LIGHTING_COLOR_OUTPUT output = (VS_INSTANCED_LIGHTING_COLOR_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)input.mtxTransform);
	output.positionW = mul(float4(input.position, 1.0f), input.mtxTransform).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}
