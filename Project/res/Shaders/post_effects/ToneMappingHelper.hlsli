
/****************************
        Reinhard
*****************************/

float3 ReinhardToneMap(float3 x)
{
    return x / (1.0f + x);
}

/****************************
Academy Color Encoding System
*****************************/

float3 ACESToneMap(float3 x)
{
    return clamp((x * (2.51f * x + 0.03f)) / (x * (2.43f * x + 0.59f) + 0.14f), 0.0f, 1.0f);
}

/****************************
        Uncharted2
*****************************/

float3 Uncharted2Partial(float3 x)
{
    const float A = 0.15f;
    const float B = 0.50f;
    const float C = 0.10f;
    const float D = 0.20f;
    const float E = 0.02f;
    const float F = 0.30f;

    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float3 Uncharted2ToneMap(float3 x)
{
    const float W = 11.2f;

    float3 curr = Uncharted2Partial(x);
    float3 whiteScale = 1.0f / Uncharted2Partial(W);

    return curr * whiteScale;
}

/****************************
       GammaCorrection
*****************************/

#define DEFAULT_GAMMA_FACTOR 2.2f

inline float4 InvGammaCorrection(float4 value, float gFactor)
{
    return pow(value, gFactor);
}

inline float4 GammaCorrection(float4 value, float gFactor)
{
    return pow(value, 1.0f / gFactor);
}

inline float3 GammaCorrection(float3 value, float gFactor)
{
    return pow(abs(value), 1.0f / gFactor);
}