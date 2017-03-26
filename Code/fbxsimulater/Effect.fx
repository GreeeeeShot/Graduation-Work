/*float4 VS(float4 Pos : POSITION) : SV_POSITION
{
	//�Է� �Ķ������ ���� ��ġ ����(Pos)�� �״�� ��ȯ�Ѵ�. ������ ���� ��ȯ�� ���� �����Ƿ� �� ������ ��ġ ���ʹ� ���� ��ǥ��� ǥ���ȴ�.
	return Pos;
}*/

//����-���̴��� ����� ���� ����ü�̴�.

cbuffer cbColor : register(b0)
{
	float4	gcColor : packoffset(c0);
};

cbuffer cbViewProjectionMatrix : register(b0)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

/*����-���̴��̴�. ������ ��ġ ���͸� ���� ��ȯ, ī�޶� ��ȯ, ���� ��ȯ�� ������� �����Ѵ�. ���� �ﰢ���� �� ������ y-�������� ȸ���� ��Ÿ���� ��Ŀ� ���� ��ȯ�Ѵ�. �׷��Ƿ� �ﰢ���� ȸ���ϰ� �ȴ�.*/
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

// ����-���̴� 
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	//�ԷµǴ� ������ ������ �״�� ����Ѵ�. 
	return output;
}

// �ȼ�-���̴�
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
//�ԷµǴ� ������ ������ �״�� ����Ѵ�. 
}