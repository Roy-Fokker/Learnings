Texture2D textureObj;
SamplerState sampleState;

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float4 color;

	color = textureObj.Sample(sampleState, input.uv);

	return color;
}