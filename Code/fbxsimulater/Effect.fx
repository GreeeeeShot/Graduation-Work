/*float4 VS(float4 Pos : POSITION) : SV_POSITION
{
	//입력 파라메터인 정점 위치 벡터(Pos)를 그대로 반환한다. 정점에 대한 변환을 하지 않으므로 이 정점의 위치 벡터는 투영 좌표계로 표현된다.
	return Pos;
}*/

//정점-쉐이더의 출력을 위한 구조체이다.

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

/*정점-쉐이더이다. 정점의 위치 벡터를 월드 변환, 카메라 변환, 투영 변환을 순서대로 수행한다. 이제 삼각형의 각 정점은 y-축으로의 회전을 나타내는 행렬에 따라 변환한다. 그러므로 삼각형은 회전하게 된다.*/
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

// 정점-쉐이더 
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, gmtxWorld);
	output.position = mul(output.position, gmtxView);
	output.position = mul(output.position, gmtxProjection);
	output.color = input.color;
	//입력되는 정점의 색상을 그대로 출력한다. 
	return output;
}

// 픽셀-쉐이더
float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
//입력되는 정점의 색상을 그대로 출력한다. 
}