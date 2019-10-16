//=============================================================================
//// Shader uses position and texture
//=============================================================================

float Size = 16;
float SizeRoot = 4;

Texture2D gLUT;
Texture2D gTexture;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
};

/// Create Rasterizer State (Backface culling) 

RasterizerState BackCulling
{
    CullMode = BACK;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
   float3 Position : POSITION0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	//Only needs float2 position as input.
	PS_INPUT output = (PS_INPUT)0;

	output.Position = float4(input.Position.xyz, 1);
	output.TexCoord = input.Position.xy * 0.5f + 0.5f;
	output.TexCoord.y = 1 - output.TexCoord.y;
	
	return output;
}

//PIXEL SHADER
//------------
float4 PSApplyLUT(PS_INPUT input): SV_Target
{
	float4 baseTexture = gTexture.Load(int3(input.Position.xy, 0));

	//Manual trilinear interpolation

	float red = baseTexture.r * (Size - 1);

	float redinterpol = frac(red);

	float green = baseTexture.g * (Size - 1);
	float greeninterpol = frac(green);

	float blue = baseTexture.b * (Size - 1);
	float blueinterpol = frac(blue);

	//Blue base value

	float row = trunc(blue / SizeRoot);
	float col = trunc(blue % SizeRoot);

	float2 blueBaseTable = float2(trunc(col * Size), trunc(row * Size));

	float4 b0r1g0;
	float4 b0r0g1;
	float4 b0r1g1;
	float4 b1r0g0;
	float4 b1r1g0;
	float4 b1r0g1;
	float4 b1r1g1;

	//Red 0 and 1, Green 0

	float4 b0r0g0 = gLUT.Load(int3(blueBaseTable.x + red, blueBaseTable.y + green, 0));

	[branch]
	if (red < Size - 1)
		b0r1g0 = gLUT.Load(int3(blueBaseTable.x + red + 1, blueBaseTable.y + green, 0));
	else
		b0r1g0 = b0r0g0;

	// Green 1

	[branch]
	if (green < Size - 1)
	{
		//Red 0 and 1

		b0r0g1 = gLUT.Load(int3(blueBaseTable.x + red, blueBaseTable.y + green + 1, 0));

		[branch]
		if (red < Size - 1)
			b0r1g1 = gLUT.Load(int3(blueBaseTable.x + red + 1, blueBaseTable.y + green + 1, 0));
		else
			b0r1g1 = b0r0g1;
	}
	else
	{
		b0r0g1 = b0r0g0;
		b0r1g1 = b0r0g1;
	}

	[branch]
	if (blue < Size - 1)
	{
		blue += 1;
		row = trunc(blue / SizeRoot);
		col = trunc(blue % SizeRoot);

		blueBaseTable = float2(trunc(col * Size), trunc(row * Size));

		b1r0g0 = gLUT.Load(int3(blueBaseTable.x + red, blueBaseTable.y + green, 0));

		[branch]
		if (red < Size - 1)
			b1r1g0 = gLUT.Load(int3(blueBaseTable.x + red + 1, blueBaseTable.y + green, 0));
		else
			b1r1g0 = b0r0g0;

		// Green 1

		[branch]
		if (green < Size - 1)
		{
			//Red 0 and 1

			b1r0g1 = gLUT.Load(int3(blueBaseTable.x + red, blueBaseTable.y + green + 1, 0));

			[branch]
			if (red < Size - 1)
				b1r1g1 = gLUT.Load(int3(blueBaseTable.x + red + 1, blueBaseTable.y + green + 1, 0));
			else
				b1r1g1 = b0r0g1;
		}
		else
		{
			b1r0g1 = b0r0g0;
			b1r1g1 = b0r0g1;
		}
	}
	else
	{
		b1r0g0 = b0r0g0;
		b1r1g0 = b0r1g0;
		b1r0g1 = b0r0g0;
		b1r1g1 = b0r1g1;
	}

	float4 result = lerp(lerp(b0r0g0, b0r1g0, redinterpol), lerp(b0r0g1, b0r1g1, redinterpol), greeninterpol);
	float4 result2 = lerp(lerp(b1r0g0, b1r1g0, redinterpol), lerp(b1r0g1, b1r1g1, redinterpol), greeninterpol);

	result = lerp(result, result2, blueinterpol);

	return result;
}


//TECHNIQUE
//---------
technique10 ApplyLUT
{
    pass P0
    {          
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PSApplyLUT() ) );
    }
}

