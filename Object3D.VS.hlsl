#include "Object3D.hlsli"

struct Transformation {
	float32_t4x4 WVP;
};
ConstantBuffer<Transformation> gTransformation : register(b0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.position = mul(input.position, gTransformation.WVP);
	output.texcoord = input.texcoord;
	return output;
}