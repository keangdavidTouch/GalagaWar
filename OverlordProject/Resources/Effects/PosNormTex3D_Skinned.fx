//------------------------------------------------------------------------------------------------------
//   _____     _______ ____  _     ___  ____  ____    _____ _   _  ____ ___ _   _ _____   ______  ___ _ 
//  / _ \ \   / / ____|  _ \| |   / _ \|  _ \|  _ \  | ____| \ | |/ ___|_ _| \ | | ____| |  _ \ \/ / / |
// | | | \ \ / /|  _| | |_) | |  | | | | |_) | | | | |  _| |  \| | |  _ | ||  \| |  _|   | | | \  /| | |
// | |_| |\ V / | |___|  _ <| |__| |_| |  _ <| |_| | | |___| |\  | |_| || || |\  | |___  | |_| /  \| | |
//  \___/  \_/  |_____|_| \_\_____\___/|_| \_\____/  |_____|_| \_|\____|___|_| \_|_____| |____/_/\_\_|_|
//
// Overlord Engine v1.115
// Copyright Overlord Thomas Goussaert & Overlord Brecht Kets
// http://www.digitalartsandentertainment.com/
//------------------------------------------------------------------------------------------------------

float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float4 gTransparentReplaceColor = float4(0,0,0,0);
float4x4 gBones[70]; //Bones?


Texture2D gDiffuseMap;
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	//BlendWeights & BlendIndices?
    float4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHTS;
};

struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;

	float4 originalPosition = float4(input.pos, 1);
	float4 transformedPosition = 0;
	float3 transformedNormal = 0;
    
	//Skinning Magic...

    for (int i = 0; i < 4; i++)
    {
        float index = input.boneIndices[i];
        
        if (index > -1)
        {
            transformedPosition += mul(originalPosition, gBones[index] * input.boneWeights[i]);
            transformedNormal += normalize(mul(input.normal, (float3x3) gBones[index] * input.boneWeights[i]));
        }
    }

    //transformedPosition = mul(originalPosition, gBones[0]);
    //transformedNormal = normalize(mul(input.normal, (float3x3) gBones[0]));

    transformedPosition.w = 1;

	//Don't forget to change the output.pos & output.normal variables...
    output.pos = mul(transformedPosition, gWorldViewProj); //Non skinned position
    output.normal = normalize(mul(transformedNormal, (float3x3) gWorld)); //Non skinned normal

	output.texCoord = input.texCoord;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET{

	float4 diffuseColor = gDiffuseMap.Sample(samLinear,input.texCoord);

	//Enable to replace transparent pixel with specified color
	if(diffuseColor.r == 0 && diffuseColor.g == 0 &&  diffuseColor.b == 0 &&  diffuseColor.a == 0) {
		diffuseColor = gTransparentReplaceColor;
	}
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	diffuseColor.rgb *= diffuseStrength;

	return diffuseColor;
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(Solid);
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

