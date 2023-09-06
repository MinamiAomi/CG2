#include "Object3D.hlsli"

struct GeometryShaderOutput {
    float32_t4 position : SV_POSITION;
    uint32_t RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(2 * 3)]
void main(
    triangle VertexShaderOutput input[3],
    inout TriangleStream<GeometryShaderOutput> output
)
{
    for (uint32_t i = 0; i < 2; i++) {
        GeometryShaderOutput element;
        element.RTIndex = i;
        for (uint32_t j = 0; j < 3; j++)
        {
            element.position = input[j].position;
            output.Append(element);
        }
    }
}
