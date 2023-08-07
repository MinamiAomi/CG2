struct Constant {
    uint32_t width;
    uint32_t height;
    uint32_t cycle;
    float32_t time;
};
ConstantBuffer<Constant> gConstant : register(b0);

struct PixelShaderInput {
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    float32_t deltaU = 1.0f / 1280.0f;
    float32_t deltaV = 1.0f / 720.0f;

    PixelShaderOutput output;

    // ぼかし
    /*
    float32_t4 sumColor = gTexture.Sample(gSampler, input.texcoord);
    sumColor += gTexture.Sample(gSampler, float32_t2(input.texcoord.x + deltaU, input.texcoord.y         ));    // 右
    sumColor += gTexture.Sample(gSampler, float32_t2(input.texcoord.x + deltaU, input.texcoord.y + deltaV));    // 右上
    sumColor += gTexture.Sample(gSampler, float32_t2(input.texcoord.x         , input.texcoord.y + deltaV));    // 上
    sumColor += gTexture.Sample(gSampler, float32_t2(input.texcoord.x - deltaU, input.texcoord.y + deltaV));    // 左上
    sumColor += gTexture.Sample(gSampler, float32_t2(input.texcoord.x - deltaU, input.texcoord.y         ));    // 左
    sumColor += gTexture.Sample(gSampler, float32_t2(input.texcoord.x - deltaU, input.texcoord.y - deltaV));    // 左下
    sumColor += gTexture.Sample(gSampler, float32_t2(input.texcoord.x         , input.texcoord.y - deltaV));    // 下
    sumColor += gTexture.Sample(gSampler, float32_t2(input.texcoord.x + deltaU, input.texcoord.y - deltaV));    // 右下
    output.color = sumColor / 9.0f;
    */

    // RGBずらし
    /*
    float32_t4 rgbShiftColor = gTexture.Sample(gSampler, input.texcoord);
    rgbShiftColor.x = gTexture.Sample(gSampler, float32_t2(input.texcoord.x + deltaU * 5, input.texcoord.y)).x;
    rgbShiftColor.z = gTexture.Sample(gSampler, float32_t2(input.texcoord.x - deltaU * 5, input.texcoord.y)).z;
    output.color = rgbShiftColor;
    */

    // モノクロ化
    /*
    float32_t4 texColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t gamma = texColor.x * 0.2126f + texColor.y * 0.7152f + texColor.z * 0.0722f;
    output.color = float32_t4(gamma, gamma, gamma, texColor.w);
    */



    return output;
}