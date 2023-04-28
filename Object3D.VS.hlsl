struct TransformationMatrix {
	float32_t4x4 WVP;
};
ConstantBuffer<TransformationMatrix> g_transformationMatrix : register(b0);

struct VertexShaderOutput {
	float32_t4 position : SV_POSITION;
};

struct VertexShaderInput {
	float32_t4 position : POSITION;
};

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	output.position = mul(input.position, g_transformationMatrix.WVP);
	return output;
}