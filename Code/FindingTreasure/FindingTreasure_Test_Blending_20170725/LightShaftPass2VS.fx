struct VS_LIGHT_SHAFT_PASS2_INPUT
{
	float3 position : POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_LIGHT_SHAFT_PASS2_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

VS_LIGHT_SHAFT_PASS2_OUTPUT VSLightShaftPass1(VS_LIGHT_SHAFT_PASS2_INPUT input)
{
	VS_LIGHT_SHAFT_PASS2_OUTPUT output = (VS_LIGHT_SHAFT_PASS2_OUTPUT)0;
	output.position = float4(input.position, 1.0f);
	output.tex2dcoord = input.tex2dcoord;

	return(output);
}