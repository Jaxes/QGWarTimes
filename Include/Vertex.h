#ifndef VERTEX_H
#define VERTEX_H

/*
这个文件到时候再整理
*/

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>
struct VertexPosColor
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 color;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosNormalColor
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};


struct VertexPosNormalTex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

struct MeshData
{
	std::vector<VertexPosNormalTex> vertexVec;	// 顶点数组
	std::vector<WORD> indexVec;					// 索引数组
};

struct PicturnData
{
	std::vector<VertexPosNormalColor> vertexVec;	    // 顶点数组
	std::vector<WORD> indexVec;					        // 索引数组
};

struct VertexModel {
	std::vector<VertexPosNormalTex> vertexs;
	std::vector<uint32_t> indexes;
	struct SubModel {
		std::string name;
		std::string usemtl;
		uint32_t startIndex;
		uint32_t count;
	};
	std::vector<SubModel> subModels;
};

struct SubModelAttr {
	std::string name;
	bool KaValid;
	DirectX::XMFLOAT3 Ka;
	bool KdValid;
	DirectX::XMFLOAT3 Kd;
	bool KsValid;
	DirectX::XMFLOAT3 Ks;
	bool NsValid;
	int Ns;
	bool illumValid;
	int illum;
	std::string colorFilePath; // 自定义 
	std::string diffuseFilePath; // 自定义
	std::string reflexFilePath; // 自定义
};

#endif