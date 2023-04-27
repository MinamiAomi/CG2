struct Material {
    float32_t4 color;
};
ConstantBuffer<Material> g_material : register(b0);
struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main() {
    PixelShaderOutput output;
    output.color = g_material.color;
    return output;
}