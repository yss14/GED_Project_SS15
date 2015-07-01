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
/*void DummyVS(in SpriteVertex input, out SpriteVertex output) {
	output = (SpriteVertex)0;
	output.Pos = mul(input.Pos, g_ViewProjection);
	output.Pos = input.Pos;
}*/

void DummyVS(SpriteVertex input, out float4 pos : SV_Position) {
	pos = float4(0, 0, 0.5, 1);
	pos = mul(pos, g_ViewProjection);
}

float4 DummyPS(float4 pos : SV_Position) : SV_Target0{
	return float4(1, 1, 0, 1);
}

/*float4 DummyPS(PSVertex pos) : SV_Target0{
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}*/

[maxvertexcount(4)]
void SpriteGS(point SpriteVertex vertex[1], inout TriangleStream<PSVertex> stream){

	float radius = vertex[0].Radius;
	PSVertex v;

	v.Position = mul(float4(vertex[0].Pos - radius * camRight - radius * camUp, 1.0f), g_ViewProjection);
	v.t = float2(0.0f, 0.0f);
	stream.Append(v);

	v.Position = mul(float4(vertex[0].Pos + radius * camRight - radius * camUp, 1.0f), g_ViewProjection);
	v.t = float2(0.0f, 1.0f);
	stream.Append(v);

	v.Position = mul(float4(vertex[0].Pos - radius * camRight + radius * camUp, 1.0f), g_ViewProjection);
	v.t = float2(1.0f, 0.0f);
	stream.Append(v);

	v.Position = mul(float4(vertex[0].Pos + radius * camRight + radius * camUp, 1.0f), g_ViewProjection);
	v.t = float2(1.0f, 1.0f);
	stream.Append(v);
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique11 Render
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, DummyVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, DummyPS()));

		SetRasterizerState(rsCullNone);
		SetDepthStencilState(EnableDepth, 0);
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}