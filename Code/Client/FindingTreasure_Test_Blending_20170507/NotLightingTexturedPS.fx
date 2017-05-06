struct VS_TEXTURED_NOT_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

Texture2D gtxtTexture : register(t0);
SamplerState gSamplerState : register(s0);

float4 PSTexturedNotLighting(VS_TEXTURED_NOT_LIGHTING_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord);

	return(cColor);
}
