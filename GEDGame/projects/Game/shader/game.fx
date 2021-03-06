//--------------------------------------------------------------------------------------
// Shader resources
//--------------------------------------------------------------------------------------

Texture2D   g_Diffuse; // Material albedo for diffuse lighting

Buffer<float> g_HeightMap;
Texture2D g_NormalMap;


//--------------------------------------------------------------------------------------
// Constant buffers
//--------------------------------------------------------------------------------------

cbuffer cbConstant
{
    float4  g_LightDir; // Object space
	int g_TerrainRes;
};

cbuffer cbChangesEveryFrame
{
    matrix  g_World;
    matrix  g_WorldViewProjection;
    float   g_Time;
	matrix g_WorldNormals;
};

cbuffer cbUserChanges
{
};


//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------

struct PosNorTex
{
    float4 Pos : SV_POSITION;
    float4 Nor : NORMAL;
    float2 Tex : TEXCOORD;
};

struct PosTexLi
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
    float   Li : LIGHT_INTENSITY;
	float3 normal: NORMAL;
};

struct PosTex{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Samplers
//--------------------------------------------------------------------------------------

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samLinearClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

//--------------------------------------------------------------------------------------
// Rasterizer states
//--------------------------------------------------------------------------------------

RasterizerState rsDefault {
};

RasterizerState rsCullFront {
    CullMode = Front;
};

RasterizerState rsCullBack {
    CullMode = Back;
};

RasterizerState rsCullNone {
	CullMode = None; 
};

RasterizerState rsLineAA {
	CullMode = None; 
	AntialiasedLineEnable = true;
};


//--------------------------------------------------------------------------------------
// DepthStates
//--------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};


//--------------------------------------------------------------------------------------
// Shaders
//--------------------------------------------------------------------------------------

PosTexLi SimpleVS(PosNorTex Input) {
    PosTexLi output = (PosTexLi) 0;

    // Transform position from object space to homogenious clip space
    output.Pos = mul(Input.Pos, g_WorldViewProjection);

    // Pass trough normal and texture coordinates
    output.Tex = Input.Tex;

    // Calculate light intensity
    output.normal = normalize(mul(Input.Nor, g_World).xyz); // Assume orthogonal world matrix
    output.Li = saturate(dot(output.normal, g_LightDir.xyz));
        
    return output;
}

PosTex TerrainVS(uint VertexID : SV_VertexID){
	PosTex output = (PosTex)0;

	int x = VertexID % g_TerrainRes;
	int z = VertexID / g_TerrainRes;

	//Calc
	output.Pos.x = x;
	output.Pos.z = z;
	output.Pos.y = g_HeightMap[VertexID];

	//Translate
	output.Pos.x = output.Pos.x - g_TerrainRes / 2;
	output.Pos.z = output.Pos.z - g_TerrainRes / 2;
	output.Pos.y = output.Pos.y - 0.5;

	//For matrix operations
	output.Pos.w = 1;

	output.Tex.x = (float)x / (g_TerrainRes - 1.0);
	output.Tex.y = (float)z / (g_TerrainRes - 1.0);

	// Transform position from object space to homogenious clip space
	output.Pos = mul(output.Pos, g_WorldViewProjection);


	return output;
}

float4 SimplePS(PosTexLi Input) : SV_Target0 {
    // Perform lighting in object space, so that we can use the input normal "as it is"
    //float4 matDiffuse = g_Diffuse.Sample(samAnisotropic, Input.Tex);
    float4 matDiffuse = g_Diffuse.Sample(samLinearClamp, Input.Tex);
    return float4(matDiffuse.rgb * Input.Li, 1);
	//return float4(Input.normal, 1);
}

float4 TerrainPS(PosTex input):SV_Target0{
	float3 n = float3(0, 0, 0);
	n.xz = g_NormalMap.Sample(samAnisotropic, input.Tex).xy;
	n.x = n.x * 2 - 1;
	n.z = n.z * 2 - 1;

	//Fixing division by 0
	float xzSqr;
	xzSqr = n.x * n.x + n.z * n.z;
	if (xzSqr > 0.995f){
		n.y = 0.1f;
	}
	else{
		n.y = sqrt(1.0 - xzSqr);
	}


	//n.y = sqrt(1 - n.x*n.x - n.z*n.z);
	n = normalize(mul(float4(n,0), g_WorldNormals).xyz);

	float3 matDiffuse = g_Diffuse.Sample(samLinearClamp, input.Tex).xyz;
	
	float i = saturate(dot(n, g_LightDir.xyz));

	//Simulate Ambient Lighting
	/*if (i < 0.2){
		i = 0.2;
	}*/

	return float4(matDiffuse.rgb * i, 1);
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique11 Render
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, TerrainVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TerrainPS()));
        
        SetRasterizerState(rsCullNone);
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}
