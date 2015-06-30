struct MyVertex{
	float3 Pos				: POSITION;
	float Radius			: RADIUS;
	uint TexIndex			: TEXTUREINDEX;
};


matrix g_ViewProjection;


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
void DummyVS(MyVertex input, out float4 pos : SV_Position) {
	pos = float4(0, 0, 0.5, 1);
	pos = mul(pos, g_ViewProjection);
}
float4 DummyPS(float4 pos : SV_Position) : SV_Target0{
	return float4(1, 1, 0, 1);
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