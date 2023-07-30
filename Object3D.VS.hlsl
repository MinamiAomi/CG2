#include "Object3D.hlsli"

struct Transformation {
    float32_t4x4 WVP;
    float32_t4x4 world;
};
ConstantBuffer<Transformation> gTransformation : register(b0);

struct VertexShaderInput {
    float32_t3 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    output.position = mul(float32_t4(input.position.xyz,1.0f), gTransformation.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)gTransformation.world));
    return output;
}