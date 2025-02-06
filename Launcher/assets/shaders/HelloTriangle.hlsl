#pragma pack_matrix(column_major)
#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT) \

struct VSInput
{
    [[vk::location(0)]]float3 pos : POSITION;
    [[vk::location(1)]]float4 col : COLOR;
};
struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};
PSInput vs_main(VSInput vsInput)
{
    PSInput vsoutput;
    vsoutput.pos = float4(vsInput.pos, 1.0f);
    vsoutput.col = vsInput.col;
    return vsoutput;
}
float4 ps_main(PSInput psInput) : SV_TARGET0
{
    return psInput.col;
}