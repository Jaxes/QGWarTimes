#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <WinSock2.h>
#include <d3d11_1.h>
#include <vector>
#include <DirectXMath.h>

#include "Vertex.h"
#include "d3dApp.h"
#include "wterr.h"

class GameObject
{
public:
	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	//构造函数
	GameObject(ComPtr<ID3D11Device> device);

	//析构函数
	~GameObject();

	// 获取位置
	DirectX::XMFLOAT3 GetPosition() const;

	// 设置世界矩阵
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void SetWorldMatrix(DirectX::FXMMATRIX world);

	//设置纹理矩阵
	void SetTexTransformMatrix(const DirectX::XMFLOAT4X4& texTransform);
	void SetTexTransformMatrix(DirectX::FXMMATRIX texTransform);

protected:
	//设备上下文
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;
	//D3D设备
	ComPtr<ID3D11Device> md3dDevice;
	// 世界矩阵
	DirectX::XMFLOAT4X4 mWorldMatrix;
	// 纹理变换矩阵
	DirectX::XMFLOAT4X4 mTexTransform;
	// 顶点缓冲区
	ComPtr<ID3D11Buffer> mVertexBuffer;
	// 索引缓冲区
	ComPtr<ID3D11Buffer> mIndexBuffer;
	// 索引数目
	int mIndexCount;
};
#endif //GAMEOBJECT_H