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
	/*********************************************************************************************************
	* MARKING SCHEME 2021
	* CATEGORY: TESSELLATION SHADERS WITH LEVEL OF DETAIL
	* DESCRIPTION: The tessellation factor is set here, all as the same value for the shader to input into 
	* the main function to generate tessellated geometry. This can be demonstrated by flying the
	* camera over the terrain as you spawn (since it's based on camera position, it doesn't account for
	* moved terrain objects.) You'll see the terrain geometry get more detailed as the camera moves 
	* down closer.
	* [Part 3]
	* *******************************************************************************************************/
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
