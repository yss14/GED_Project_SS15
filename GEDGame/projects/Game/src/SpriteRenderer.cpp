#include "SpriteRenderer.h"
#include "DXUT.h"
#include "d3dx11effect.h"
#include "SDKmisc.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

SpriteRenderer::SpriteRenderer(const std::vector<std::wstring>& textureFilenames){
	m_textureFilenames = textureFilenames;
	m_pEffect = nullptr;
	m_spriteSRV;
	m_spriteCountMax = 0;
	m_pVertexBuffer = nullptr;
	m_pInputLayout = nullptr;
}

SpriteRenderer::~SpriteRenderer(){

}

HRESULT SpriteRenderer::reloadShader(ID3D11Device* pdevice){

	HRESULT hr;
	WCHAR path[MAX_PATH];
	// Find and load the rendering effect
	V_RETURN(DXUTFindDXSDKMediaFileCch(path, MAX_PATH, L"shader\\SpriteRenderer.fxo"));
	std::ifstream is(path, std::ios_base::binary);
	is.seekg(0, std::ios_base::end);
	std::streampos pos = is.tellg();
	is.seekg(0, std::ios_base::beg);
	std::vector<char> effectBuffer((unsigned int)pos);
	is.read(&effectBuffer[0], pos);
	is.close();
	V_RETURN(D3DX11CreateEffectFromMemory((const void*)&effectBuffer[0], effectBuffer.size(), 0, pdevice, &m_pEffect));
	assert(m_pEffect->IsValid());

	// Obtain the effect technique
	SAFE_GET_TECHNIQUE(m_pEffect, "Render", technique);
	
	// Obtain the effect pass
	SAFE_GET_PASS(technique, "P0", pass0);
	return S_OK;
}

void SpriteRenderer::releaseShader(){
	SAFE_RELEASE(m_pEffect);
}

HRESULT SpriteRenderer::create(ID3D11Device* pdevice){

	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(SpriteVertex) * 1024;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Create the vertex buffer.
	hr = pdevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);

	// Define the input layout
	const D3D11_INPUT_ELEMENT_DESC layout[] = // http://msdn.microsoft.com/en-us/library/bb205117%28v=vs.85%29.aspx
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "RADIUS", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTUREINDEX", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	SAFE_GET_MATRIX(m_pEffect, "g_ViewProjection", g_vProjection);
	SAFE_GET_VECTOR(m_pEffect, "camRight", g_camRight);
	SAFE_GET_VECTOR(m_pEffect, "camUp", g_camUp);

	// Create the input layout
	D3DX11_PASS_DESC pd;
	V_RETURN(pass0->GetDesc(&pd));
	V_RETURN(pdevice->CreateInputLayout(layout, numElements, pd.pIAInputSignature,
		pd.IAInputSignatureSize, &m_pInputLayout));

	return hr;
}

HRESULT SpriteRenderer::renderSprites(ID3D11DeviceContext* context, const std::vector<SpriteVertex>& sprites, const CFirstPersonCamera& camera){
	HRESULT hr;

	D3D11_BOX box;
	box.left = 0; box.right = sprites.size() * sizeof(SpriteVertex);
	box.top = 0; box.bottom = 1;
	box.front = 0; box.back = 1;
	
	context->UpdateSubresource(this->m_pVertexBuffer, 0, &box, &sprites[0], 0, 0);
	unsigned int strides[] = { sizeof(SpriteVertex), }, offsets[] = { 0, };
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, strides, offsets);
	context->IASetInputLayout(m_pInputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	DirectX::XMMATRIX viewProjection;
	viewProjection = camera.GetViewMatrix() * camera.GetProjMatrix();
	V_RETURN(g_vProjection->SetMatrix((float*)&viewProjection));
	V_RETURN(g_camRight->SetFloatVector((float*)&camera.GetWorldRight()));
	V_RETURN(g_camUp->SetFloatVector((float*)&camera.GetWorldUp()));

	V_RETURN(pass0->Apply(0, context));
	context->Draw(1, 0);

	return hr;
}

void SpriteRenderer::destroy(){
	for (int i = 0; i < m_spriteSRV.size(); i++){
		SAFE_RELEASE(m_spriteSRV[i]);
	}

	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pInputLayout);
}