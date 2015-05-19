#pragma once
#include "DXUT.h"
#include "d3dx11effect.h"
#include "ConfigParser.h"
#include "SimpleImage.h"
#include <iostream>

struct SimpleVertex{
	DirectX::XMFLOAT4 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;

	SimpleVertex(){
		Pos.x = Pos.y = Pos.z = Pos.w = Normal.x = Normal.y = Normal.z = UV.x = UV.y = 0;
	}
};

class Terrain
{
public:
	Terrain(void);
	~Terrain(void);

	HRESULT create(ID3D11Device* device, ConfigParser* parser);
	void destroy();

	void render(ID3D11DeviceContext* context, ID3DX11EffectPass* pass);


private:
	Terrain(const Terrain&);
	Terrain(const Terrain&&);
	void operator=(const Terrain&);

	// Terrain rendering resources
	ID3D11Buffer*                           vertexBuffer;	// The terrain's vertices
	ID3D11Buffer*                           indexBuffer;	// The terrain's triangulation
	ID3D11Texture2D*                        diffuseTexture; // The terrain's material color for diffuse lighting
	ID3D11ShaderResourceView*               diffuseTextureSRV; // Describes the structure of the diffuse texture to the shader stages

	// General resources
	ID3D11ShaderResourceView*               debugSRV;
	std::vector<unsigned int> indexVector;
};

