//����-���̴��� ����� ���� ����ü�̴�.
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

// �ȼ�-���̴�
float4 DiffusedPS(VS_OUTPUT input) : SV_Target
{
	return input.color;
//�ԷµǴ� ������ ������ �״�� ����Ѵ�. 
}
