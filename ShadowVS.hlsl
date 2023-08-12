struct VertexShaderInput {
    float32_t3 position : POSITION;
};

struct VertexShaderOutput {
    float32_t4 svPosition : SV_POSITION;
    float32_t3 worldPosition : TEXCOORD1;
};

struct SceneConstant {
    float32_t4x4 cameraViewProj;
    float32_t4x4 shadowViewProj;
    float32_t3   cameraWorldPos;
    float32_t sunLightIntensity;
    float32_t4 sunLightColor;
    float32_t3 sunLightDirection;
};
ConstantBuffer<SceneConstant> sceneConstant_ : register(b0);

struct InstanceConstant {
    float32_t4x4 worldMatrix;
};
ConstantBuffer<InstanceConstant> instanceConstant_ : register(b1);

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;

    float32_t4 worldPosition = mul(float32_t4(input.position, 1.0f), instanceConstant_.worldMatrix;

    output.svPosition = mul(worldPosition, sceneConstant_.cameraViewProj);
    output.worldPosition = worldPosition.xyz;

    return output;
}