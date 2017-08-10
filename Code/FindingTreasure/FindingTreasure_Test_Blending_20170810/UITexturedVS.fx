//cbuffer cbViewProjectionMatrix : register(b0)
//{
//	matrix gmtxView : packoffset(c0);
//	matrix gmtxProjection : packoffset(c4);
//};

//cbuffer cbWindowSize : register(b0)
//{
//	float gfWindowSizeW;
//	float gfWindowSizeH;
//};

cbuffer cbWorldMatrix : register(b1)
{
	matrix gmtxWorld : packoffset(c0);
};

struct VS_UI_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_UI_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

VS_UI_TEXTURED_OUTPUT VSUITextured(VS_UI_TEXTURED_INPUT input)
{
	VS_UI_TEXTURED_OUTPUT output = (VS_UI_TEXTURED_OUTPUT)0;
	output.position = float4(gmtxWorld._41 + input.position.x , gmtxWorld._42 + input.position.y, gmtxWorld._43, 1.0f);
	output.position.x *= (2.0f / 1280.0f);
	output.position.y *= (2.0f / 720.0f);
	output.position.z = 0.0f;
	output.tex2dcoord = input.tex2dcoord;

	return(output);
}
