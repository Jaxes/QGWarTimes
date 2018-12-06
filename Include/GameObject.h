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
	// ʹ��ģ�����(C++11)��������
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	//���캯��
	GameObject(ComPtr<ID3D11Device> device);

	//��������
	~GameObject();

	// ��ȡλ��
	DirectX::XMFLOAT3 GetPosition() const;

	// �����������
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void SetWorldMatrix(DirectX::FXMMATRIX world);

	//�����������
	void SetTexTransformMatrix(const DirectX::XMFLOAT4X4& texTransform);
	void SetTexTransformMatrix(DirectX::FXMMATRIX texTransform);

protected:
	//�豸������
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;
	//D3D�豸
	ComPtr<ID3D11Device> md3dDevice;
	// �������
	DirectX::XMFLOAT4X4 mWorldMatrix;
	// ����任����
	DirectX::XMFLOAT4X4 mTexTransform;
	// ���㻺����
	ComPtr<ID3D11Buffer> mVertexBuffer;
	// ����������
	ComPtr<ID3D11Buffer> mIndexBuffer;
	// ������Ŀ
	int mIndexCount;
};
#endif //GAMEOBJECT_H