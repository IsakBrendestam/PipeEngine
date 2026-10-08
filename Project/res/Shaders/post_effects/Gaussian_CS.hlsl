
Texture2D inputTexture: register(t0);
RWTexture2D<float4> outputTexture: register(u0);

static const float weights3[10] =
{
    0.39905028,
    0.24203623,
    0.05400558,
    0.00443305,
    0,
    0,
    0,
    0,
    0,
    0
};

static const float weights5[10] =
{
    0.22702703,
    0.19459459,
    0.12162162,
    0.05405405,
    0.01621622,
    0.00316228,
    0,
    0,
    0,
    0
};

static const float weights7[10] =
{
    0.15957691,
    0.14730806,
    0.10777865,
    0.07427257,
    0.03645783,
    0.01621622,
    0.00687262,
    0.00284900,
    0,
    0
};

static const float weights9[10] =
{
    0.13298076,
    0.12581549,
    0.10663900,
    0.08628024,
    0.06168930,
    0.04018631,
    0.02386418,
    0.01285158,
    0.00622915,
    0.00268472
};


cbuffer BlurConstants: register(b0)
{
    int direction;
    int radius;
    int2 padding;
}


[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint2 pixel = DTid.xy;

    if (pixel.x >= 1920 || pixel.y >= 1080)
        return;

    uint2 dir;
    if (direction == 0)
        dir = float2(1.0, 0.0);
    else
        dir = float2(0.0, 1.0);

    float weights[10];
    if (radius == 3)
        weights = weights3;
    else if (radius == 5)
        weights = weights5;
    else if (radius == 7)
        weights = weights7;
    else if (radius == 9)
        weights = weights9;

    float3 color = inputTexture[pixel].rgb * weights[0];

    for (int i = 1; i < radius; i++)
    {
        int2 offset = int2(dir * i);

        color += inputTexture[pixel + offset].rgb * weights[i];
        color += inputTexture[pixel - offset].rgb * weights[i];
    }

    outputTexture[pixel] = float4(color, 1.0);
}