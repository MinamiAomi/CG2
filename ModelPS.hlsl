// RECIVE_SHADOWを定義することで影を受ける

// 頂点シェーダーの入力
struct PixelShaderInput {
    float32_t4 svPosition : SV_POSITION;
    float32_t3 normal : NORMAL0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 worldPosition : TEXCOORD1;
#ifndef RECIVE_SHADOW
    float32_t3 shadowCoord : TEXCOORD2;
#endif
};

// ピクセルシェーダーの出力
struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

// シーン全体で共通の定数
struct SceneConstant {
    float32_t4x4 cameraViewProj;
    float32_t4x4 shadowViewProj;
    float32_t3 cameraWorldPos;
    float32_t sunLightIntensity;
    float32_t4 sunLightColor;
    float32_t3 sunLightDirection;
};
ConstantBuffer<SceneConstant> sceneConstant_ : register(b0);

// マテリアル定数
struct MaterialConstant {
    float32_t3 baseColor;
    float32_t metalness;
    float32_t roughness;
};
ConstantBuffer<MaterialConstant> materialConstant_ : register(b2);

// テクスチャ
Texture2D<float32_t4> baseColorTexture_ : register(t0);
SamplerState defaultSampler_ : register(s0);

#ifndef RECIVE_SHADOW
// 影テクスチャ
Texture2D<float32_t> shadowTexture_ : register(t1);
SamplerState shadowSampler_ : register(s1);
#endif

#define PI 3.14159265

float32_t Pow5(float32_t x) {
    float xSquare = x * x;
    return xSquare * xSquare * x;
}

float32_t SchickFresnel(float32_t f0, float32_t f90, float32_t cosine) {
    return lerp(f0, f90, Pow5(1.0f - cosine));
}

float32_t3 SchickFresnel(float32_t3 f0, float32_t3 f90, float32_t cosine) {
    return lerp(f0, f90, Pow5(1.0f - cosine));
}

PixelShaderOutput main(PixelShaderInput input) {
    PixelShaderOutput output;
    output.color = float32_t4(1.0f, 1.0f, 1.0f, 1.0f);


    float32_t NdotL = dot(normalize(input.normal), -sceneConstant_.sunLightDirection);
    float32_t cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float32_t3 shadeColor = sceneConstant_.sunLightColor.xyz * cos * sceneConstant_.sunLightIntensity;

    float32_t3 baseColor = baseColorTexture_.Sample(defaultSampler_, input.texcoord).xyz;

    output.color.xyz = baseColor * shaderColor;

    return output;
}