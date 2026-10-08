#include "Common.hlsli"

float CalcLuminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

float4 Sobel(int2 xy, InputTexture input)
{
    float4 c[3][3];
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int2 temp_xy = xy + int2(j - 1, i - 1);
            c[i][j] = input.inputBuffer[temp_xy];
        }
    }

    float4 Gx = -1.0f * c[0][0] - 2.0f * c[1][0] - 1.0f * c[2][0] + 1.0f * c[0][2] + 2.0f * c[1][2] + 1.0f * c[2][2];
    float4 Gy = -1.0f * c[2][0] - 2.0f * c[2][1] - 1.0f * c[2][1] + 1.0f * c[0][0] + 2.0f * c[0][1] + 1.0f * c[0][2];

    float4 mag = sqrt(Gx * Gx + Gy * Gy);

    mag = saturate(CalcLuminance(mag.rgb));

    return float4(mag.rgb, 1);
}