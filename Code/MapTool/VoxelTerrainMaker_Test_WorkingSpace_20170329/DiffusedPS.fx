//정점-쉐이더의 출력을 위한 구조체이다.
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

// 픽셀-쉐이더
float4 DiffusedPS(VS_OUTPUT input) : SV_Target
{
	return input.color;
//입력되는 정점의 색상을 그대로 출력한다. 
}
