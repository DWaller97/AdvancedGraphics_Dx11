struct VSIN {
	float4 Position : SV_POSITION;
};

struct VSOUT {
	float4 Position : SV_POSITION;
};

VSOUT main(VSIN input)
{
	VSOUT output;
	output.Position = input.Position;
	return output;
}