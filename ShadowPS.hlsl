// RECIVE_SHADOWを定義することで影を受ける

// 頂点シェーダーの入力
struct PixelShaderInput {
    float32_t4 svPosition : SV_POSITION;
    float32_t3 worldPosition : TEXCOORD1;
};

// ピクセルシェーダーの出力
struct PixelShaderOutput {
    float32_t depth : SV_TARGET0;
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

PixelShaderOutput main(PixelShaderInput input) {
    PixelShaderOutput output;

    output.depth = input.svPosition.z;

    return output;
}