struct SpriteVertex{
	float3 Pos				: POSITION;
	float Radius			: RADIUS;
	uint TexIndex			: TEXTUREINDEX;
};

struct PSVertex{
	float4 Position :	SV_Position;
	float2 t		:	TEXCOORD;
};


matrix g_ViewProjection;
float3 camRight, camUp;

// Rasterizer states
RasterizerState rsCullNone {
	CullMode = None;
};

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


// ShaderCode
void DummyVS(in SpriteVertex input, out SpriteVertex output) {
	output = (SpriteVertex)0;
	//output.Pos = mul(input.Pos, g_ViewProjection);
	output.Pos = input.Pos;
}
float4 DummyPS(PSVertex pos) : SV_Target0{
	return float4(1, 1, 0, 1);
}

[maxvertexcount(4)]
void SpriteGS(point SpriteVertex vertex[1], inout TriangleStream<PSVertex> stream){

	vertex[0].Pos = mul(vertex[0].Pos, g_ViewProjection);

	PSVertex v;
	v.Position = float4(vertex[0].Pos.x - vertex[0].Radius, vertex[0].Pos.x + vertex[0].Radius, vertex[0].Pos.z, 1); // upper left
	v.t = float2(0, 0);
	stream.Append(v); // output first vertex
	v.Position = float4(vertex[0].Pos.x + vertex[0].Radius, vertex[0].Pos.x + vertex[0].Radius, vertex[0].Pos.z, 1); // upper right
	v.t = float2(0, 1);
	stream.Append(v); // output second vertex
	v.Position = float4(vertex[0].Pos.x - vertex[0].Radius, vertex[0].Pos.x - vertex[0].Radius, vertex[0].Pos.z, 1); // lower left
	v.t = float2(1, 0);
	stream.Append(v); // output third vertex
	v.Position = float4(vertex[0].Pos.x + vertex[0].Radius, vertex[0].Pos.x - vertex[0].Radius, vertex[0].Pos.z, 1); // lower right
	v.t = float2(1, 1);
	stream.Append(v); // output third vertex
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique11 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, DummyVS()));
		SetGeometryShader(CompileShader(gs_4_0, SpriteGS()));
		SetPixelShader(CompileShader(ps_4_0, DummyPS()));

		SetRasterizerState(rsCullNone);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}