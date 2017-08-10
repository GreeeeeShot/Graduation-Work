struct VS_SHADOW_MAPPING_PASS0_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

Texture2D gtxtTexture : register(t0);											// 복셀의 텍셀이다.
SamplerState gSamplerState : register(s0);										// 샘플러

void PSShadowMappingPass0(VS_SHADOW_MAPPING_PASS0_OUTPUT input)
{
	float4 cDiffuse = gtxtTexture.Sample(gSamplerState, input.tex2dcoord);		// 복셀의 텍스쳐 텍셀 값 추출

	clip(cDiffuse.a - 0.15f);													// 투명한 것은 기각한다.
}
