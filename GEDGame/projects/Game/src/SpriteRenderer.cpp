#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(const std::vector<std::wstring>& textureFilenames){
	m_textureFilenames = textureFilenames;
	m_pEffect = nullptr;
	m_spriteSRV;
	m_spriteCountMax = 0;
	m_pVertexBuffer = nullptr;
	m_pInputLayout = nullptr;
}

SpriteRenderer::~SpriteRenderer(){

	for (int i = 0; i < m_spriteSRV.size(); i++){
		SAFE_RELEASE(m_spriteSRV[i]);
	}

	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pInputLayout);
}

HRESULT SpriteRenderer::reloadShader(ID3D11Device* pdevice){


	return S_OK;
}

void SpriteRenderer::releaseShader(){

}

HRESULT SpriteRenderer::create(ID3D11Device* pdevice){
	return S_OK;
}

void SpriteRenderer::renderSprites(ID3D11DeviceContext* context, const std::vector<SpriteVertex>& sprites, const CFirstPersonCamera& camera){

}

void SpriteRenderer::destroy(){

}