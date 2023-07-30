#include "Object3D.hlsli"

struct Material {
    float32_t4 color;
    int32_t useTexture;
    float32_t4x4 uvTransform;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight {
    float32_t4 color;
    float32_t3 direction;
    float32_t intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
    PixelShaderOutput output;
    output.color = gMaterial.color;

    float32_t NdotL = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
    output.color.xyz *= gDirectionalLight.color.xyz * NdotL * gDirectionalLight.intensity;

    if (gMaterial.useTexture != 0) {
        float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
        float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
        output.color *= textureColor;
    }

    return output;
}