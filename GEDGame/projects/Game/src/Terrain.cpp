#include "Terrain.h"
#include "GameEffect.h"
#include <algorithm>
#include <DDSTextureLoader.h>
#include "DirectXTex.h"
#include "Utils.h"

// You can use this macro to access your height field
#define IDX(X,Y,WIDTH) ((X) + (Y) * (WIDTH))

Terrain::Terrain(void) :
indexBuffer(nullptr),
//vertexBuffer(nullptr),
diffuseTexture(nullptr),
diffuseTextureSRV(nullptr),
debugSRV(nullptr)
{
}


Terrain::~Terrain(void)
{
}



HRESULT Terrain::create(ID3D11Device* device, ConfigParser* parser)
{
	HRESULT hr;

	// Loading the heightMap
	std::wstring outDir(TARGET_DIRECTORY);
	outDir += L"resources\\";
	std::wstring tmp = Utils::s2ws(parser->getTerrainPath());
	outDir += tmp;
	GEDUtils::SimpleImage heightMap(outDir.c_str());
	std::cout << "Loading height map " << parser->getTerrainPath() << " into DirectX \n";

	// Loading Terrain Texture
	outDir = TARGET_DIRECTORY;
	outDir += L"resources\\";
	tmp = Utils::s2ws(parser->getTerrainTexturePath());
	outDir += tmp;
	V(DirectX::CreateDDSTextureFromFile(device, outDir.c_str() , nullptr, &diffuseTextureSRV));
	std::cout << "Loading texture map " << parser->getTerrainTexturePath() << " into DirectX \n";

	// Loading Normal Map
	outDir = TARGET_DIRECTORY;
	outDir += L"resources\\";
	tmp = Utils::s2ws(parser->getTerrainNormalPath());
	outDir += tmp;
	V(DirectX::CreateDDSTextureFromFile(device, outDir.c_str(), nullptr, &normalmapTextureSRV));
	std::cout << "Loading normal map " << parser->getTerrainNormalPath() << " into DirectX \n";

	if (hr != S_OK) {
		MessageBoxA(NULL, "Could not load Terrain Texture", "Invalid texture", MB_ICONERROR | MB_OK);
		return hr;
	}

	// Initialising vectors
	std::vector<SimpleVertex> vertexVector(heightMap.getWidth() * heightMap.getHeight(), {});
	indexVector = std::vector<unsigned int>((heightMap.getWidth() - 1) * (heightMap.getHeight() - 1) * 6, 0);

	/*// Vertex buffer init
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = vertexVector.size() * sizeof(SimpleVertex);//The size in bytes of the pointer array of the heightmap
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA id;
	id.pSysMem = &vertexVector[0];
	id.SysMemPitch = 10 * sizeof(SimpleVertex); // Stride
	id.SysMemSlicePitch = 0;*/

	// index buffer init stuff
	D3D11_SUBRESOURCE_DATA idi;
	ZeroMemory(&idi, sizeof(idi));
	idi.pSysMem = &indexVector[0];
	
 	D3D11_BUFFER_DESC bdi;
	ZeroMemory(&bdi, sizeof(bdi));
	bdi.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bdi.ByteWidth = indexVector.size() * sizeof(unsigned int); //The size in bytes of the pointer array of the heightmap
	bdi.CPUAccessFlags = 0;
	bdi.MiscFlags = 0;
	bdi.Usage = D3D11_USAGE_DEFAULT;

	// init stuff for vertexVector and indexVector initialisation 
	int indexCnt = 0;
	int vertexCnt = 0;
	int width = heightMap.getWidth();
	int height = heightMap.getHeight();

	// Copying heightMap Data from the picture 
	heightMapVector = std::vector<float>(height*width);
	for (int z = 0; z < height; z++){
		for (int x = 0; x < width; x++){
			heightMapVector[IDX(x, z, width)] = heightMap.getPixel(x, z);
		}
	}
	for (int z = 0; z < height; z++){
		for (int x = 0; x < width; x++){

			// doesn't have to add triangles for the  last row/col
			if (x < width - 1 && z < height - 1){

				// first triangle
				indexVector[indexCnt++] = IDX(x, z, width);
				indexVector[indexCnt++] = IDX(x + 1, z, width);
				indexVector[indexCnt++] = IDX(x, z + 1, width);

				// second triangle
				indexVector[indexCnt++] = IDX(x + 1, z, width);
				indexVector[indexCnt++] = IDX(x + 1, z + 1, width);
				indexVector[indexCnt++] = IDX(x, z + 1, width);
			}
		}
	}

	// Vertex buffer init
	D3D11_BUFFER_DESC bd_heightmap;
	bd_heightmap.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bd_heightmap.ByteWidth = heightMapVector.size() * sizeof(float);//The size in bytes of the pointer array of the heightmap
	bd_heightmap.CPUAccessFlags = 0;
	bd_heightmap.MiscFlags = 0;
	bd_heightmap.Usage = D3D11_USAGE_DEFAULT;

	std::cout << "\nbd_heightmap ByteWidth=" << (heightMapVector.size() * sizeof(float)) << " Bytes \n";


	D3D11_SUBRESOURCE_DATA id_heightmap;
	id_heightmap.pSysMem = &heightMapVector[0];
	id_heightmap.SysMemPitch = sizeof(float);
	id_heightmap.SysMemSlicePitch = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.Buffer.FirstElement = 0;
	srvd.Buffer.NumElements = width * height;

	V(device->CreateBuffer(&bdi, &idi, &indexBuffer));
	V(device->CreateBuffer(&bd_heightmap, &id_heightmap, &heightBuffer));

	V(device->CreateShaderResourceView(heightBuffer, &srvd, &heightBufferSRV));
	//V(device->CreateBuffer(&bd, &id, &vertexBuffer)); // http://msdn.microsoft.com/en-us/library/ff476899%28v=vs.85%29.aspx
	return hr;
}


void Terrain::destroy()
{
	//SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(debugSRV);
	SAFE_RELEASE(diffuseTextureSRV);

	//Added in Assignment05
	SAFE_RELEASE(normalmapTexture);
	SAFE_RELEASE(normalmapTextureSRV);
	SAFE_RELEASE(heightBuffer);
	SAFE_RELEASE(heightBufferSRV);
}


void Terrain::render(ID3D11DeviceContext* context, ID3DX11EffectPass* pass)
{
	HRESULT hr;

	// Bind the terrain vertex buffer to the input assembler stage 
	ID3D11Buffer* vbs[] = { nullptr, };
	unsigned int strides[] = { 10 * sizeof(float), }, offsets[] = { 0, };
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Tell the input assembler stage which primitive topology to use
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Bind the SRV of the terrain diffuse texture to the effect variable
	V(g_gameEffect.diffuseEV->SetResource(diffuseTextureSRV));

	//Added in Assignment05
	V(g_gameEffect.heightmap->SetResource(heightBufferSRV));
	V(g_gameEffect.normalmap->SetResource(normalmapTextureSRV));

	V(g_gameEffect.resolution->SetInt(sqrt(heightMapVector.size())));
	//std::cout << "Set g_gameEffect.resolution = 256 \n";

	// Apply the rendering pass in order to submit the necessary render state changes to the device
	V(pass->Apply(0, context));

	// Draw the terrain geometry using as shared vertex list
	context->DrawIndexed(indexVector.size(), 0, 0);
	//context->Draw(3, 0);
}

Vec3f Terrain::calculateNormal(int x, int z, int resolution){

	Vec3f normalAtCurPoint(0.0f,0.0f,0.0f);
	Vec3f vecXAxes(0.0f,0.0f,0.0f);
	Vec3f vecYAxes(0.0f, 0.0f, 0.0f);

	normalAtCurPoint.x = normalAtCurPoint.y = normalAtCurPoint.z = 0;
	vecXAxes.x = 1.0f;
	vecXAxes.y = 0.0f;
	vecYAxes.x = 0.0f;
	vecYAxes.y = 1.0f;

	//Central Differecnes
	if (x == 0)
	{
		//Forward Differences
		vecXAxes.z = (heightMapVector[IDX(x + 1, z, resolution)] - heightMapVector[IDX(x, z, resolution)]);
	}
	else if (x == resolution - 1)
	{
		//Backward Differences
		vecXAxes.z = (heightMapVector[IDX(x, z, resolution)] - heightMapVector[IDX(x - 1, z, resolution)]);
	}
	else
	{
		//Central Differences
		vecXAxes.z = (heightMapVector[IDX(x + 1, z, resolution)] - heightMapVector[IDX(x - 1, z, resolution)]) / 2.0f;
	}

	if (z == 0)
	{
		//Forward Differences
		vecYAxes.z = (heightMapVector[IDX(x, z + 1, resolution)] - heightMapVector[IDX(x, z, resolution)]);
	}
	else if (z == resolution - 1)
	{
		//Backward Differences
		vecYAxes.z = (heightMapVector[IDX(x, z, resolution)] - heightMapVector[IDX(x, z - 1, resolution)]);
	}
	else
	{
		//Central Differences
		vecYAxes.z = (heightMapVector[IDX(x, z + 1, resolution)] - heightMapVector[IDX(x, z - 1, resolution)]) / 2.0f;
	}

	normalAtCurPoint = vecXAxes.cross(vecYAxes);

	//Rescaling because of different dimensions
	normalAtCurPoint.x *= resolution;
	normalAtCurPoint.y *= resolution;

	normalAtCurPoint.normalize();

	return normalAtCurPoint;
}

float Terrain::getHeight(int x, int z){
	return heightMapVector[(x, z, sqrt(this->heightMapVector.size()))];
}

float Terrain::getCenterHeight(){
	return getHeight(sqrt(this->heightMapVector.size()) / 2, sqrt(this->heightMapVector.size()) / 2);
}

