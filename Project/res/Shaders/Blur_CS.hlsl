RWTexture2D<float4> backBufferUAV : register(u0);

Texture2D input : register(t0);

SamplerState samp : register(s0);

const static float offset[5] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f };
const static float weight[5] = {
  0.2270270270, 0.1945945946, 0.1216216216,
  0.0540540541, 0.0162162162
};

const static float blurScale = 4.0f;

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // Default calculations
    uint2 dimensions;
    backBufferUAV.GetDimensions(dimensions.x, dimensions.y);
    float2 uv = float2((float) DTid.x / (float) dimensions.x, (float) DTid.y / (float) dimensions.y);

    float2 direction = float2(1.0f / dimensions.x, 0.0f);       // Horizontal
    //float2 direction = float2(0.0f, 1.0f / dimensions.y);       // Vertical

    float3 fragmentColor = input.SampleLevel(samp, uv, 0).rgb * weight[0];

    float hstep = direction.x;
    float vstep = direction.y;

    for (int i = 1; i < 5; i++)
    {
        float2 offsetUV = float2(hstep, vstep) * offset[i] * blurScale;

        fragmentColor += 
            input.SampleLevel(samp, uv + offsetUV, 0).rgb * weight[i] + 
            input.SampleLevel(samp, uv - offsetUV, 0).rgb * weight[i];
    }

    backBufferUAV[DTid.xy] = float4(fragmentColor, 1.0f);
}
