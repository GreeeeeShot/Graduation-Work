#include "Light.fx"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

//������ ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	//������ǥ�迡�� ������ ��ġ�� ���� ���͸� ��Ÿ����.
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

//�ν��Ͻ��� �ϸ鼭 ������ ����ϴ� ��� ���� ���̴��� ����� ���� ����ü�̴�.
struct VS_INSTANCED_LIGHTING_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
}

//�� �ȼ��� ���Ͽ� ������ ������ �ݿ��� ������ ����ϱ� ���� �ȼ� ���̴� �Լ��̴�.
float4 PSLightingColor(VS_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);

	return(cIllumination);
}

float4 PSInstancedLightingColor(VS_INSTANCED_LIGHTING_COLOR_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);

	return(cIllumination);
}
