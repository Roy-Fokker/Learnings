cbuffer frameBuffer : register(b0)
{
	matrix projection;
}

cbuffer transformBuffer : register(b1)
{
	matrix transform;
}

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	input.pos.w = 1.0f;

	output.pos = mul(input.pos, transform);
	output.pos = mul(output.pos, projection);

	output.uv = input.uv;
	
	return output;
}