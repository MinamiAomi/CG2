struct VertexShaderInput {
    float32_t3 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};

struct VertexShaderOutput {
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.position = float32_t4(input.position.xyz, 1.0f);
    output.texcoord = input.texcoord;
    return output;
}