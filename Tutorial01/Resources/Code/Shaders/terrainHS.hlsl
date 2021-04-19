struct HSIN {
	float4 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};
// Output patch constant data.
struct PatchOUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

cbuffer TessBuffer : register(b0) {
	float TessAmount;
	float3 padding;
};




#define NUM_CONTROL_POINTS 3

// Patch Constant Function
PatchOUT CalcHSPatchConstants(
	InputPatch<HSIN, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	PatchOUT output = (PatchOUT)0;
	output.EdgeTessFactor[0] = TessAmount;
	output.EdgeTessFactor[1] = TessAmount;
	output.EdgeTessFactor[2] = TessAmount;

	output.InsideTessFactor = TessAmount;

	return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HSIN main(
	InputPatch<HSIN, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HSIN output = (HSIN)0;

	output.Pos = ip[i].Pos;
	output.Norm = ip[i].Norm;
	output.Tex = ip[i].Tex;

	return output;
}
