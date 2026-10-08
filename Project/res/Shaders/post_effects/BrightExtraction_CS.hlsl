Texture2D scene : register(t0);
RWTexture2D<float4> bloom : register(u0);

SamplerState samp : register(s0);

cbuffer BloomSettings : register(b0)
{
    float threshold;
    float bloomStrength;
};

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width, height;
    bloom.GetDimensions(width, height);

    if (DTid.x >= width || DTid.y >= height)
        return;

    float2 uv =
        (float2(DTid.xy) + 0.5f) /
        float2(width, height);

    float3 color = scene.SampleLevel(samp, uv, 0).rgb;

    // Rec. 709 luminance
    float luminance = dot(color, float3(0.2126, 0.7152, 0.0722));

    float knee = 0.5f;

    float soft = luminance - threshold + knee;
    soft = clamp(soft, 0.0f, 2.0f * knee);

    float mask = soft * soft / (4.0f * knee + 0.0001f);

    mask = max(mask, step(threshold, luminance));

    bloom[DTid.xy] = float4(color * mask, 1.0);
}