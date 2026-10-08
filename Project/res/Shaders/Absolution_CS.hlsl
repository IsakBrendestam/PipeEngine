RWTexture2D<float4> backBufferUAV : register(u0);

Texture2D input : register(t0);

SamplerState samp : register(s0);

float CalcLuminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

const static float4 filloutColor = float4(0.96f, 0.83f, 0.26f, 1.0f);

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // Default calculations
    uint2 dimensions;
    backBufferUAV.GetDimensions(dimensions.x, dimensions.y);
    float2 uv = float2((float) DTid.x / (float) dimensions.x, (float) DTid.y / (float) dimensions.y);

    const float4 origionalColor = backBufferUAV[DTid.xy];

    // Sobel Operator
    float4 c[3][3];
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int2 xy = DTid.xy + int2(j - 1, i - 1);
            c[i][j] = input[xy];
        }
    }

    float4 Gx = -1.0f * c[0][0] - 2.0f * c[1][0] - 1.0f * c[2][0] + 1.0f * c[0][2] + 2.0f * c[1][2] + 1.0f * c[2][2];
    float4 Gy = -1.0f * c[2][0] - 2.0f * c[2][1] - 1.0f * c[2][1] + 1.0f * c[0][0] + 2.0f * c[0][1] + 1.0f * c[0][2];

    float4 mag = sqrt(Gx * Gx + Gy * Gy);

    float lum = CalcLuminance(mag.rgb);
    float outline = saturate(lum);

    // Outline
    //if (outline > 0.5f) // TODO: remvoe if statement
        //backBufferUAV[DTid.xy] = float4(outline * filloutColor);

	// Fillout
	float4 sample = input.SampleLevel(samp, uv, 0);
    if (sample.r >= 0.01f || sample.g > 0.01f || sample.b > 0.01f) // TODO: remove if statement
        backBufferUAV[DTid.xy] = float4(origionalColor.rgb * (1-sample.a) + sample.rgb * sample.a, 1);
}
