struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 worldPos : WPOS;
    float4 color : COLOR;
};

float4 main(PixelShaderInput input) : SV_TARGET
{   
    return input.normal;
}