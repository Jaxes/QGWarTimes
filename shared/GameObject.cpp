#include "GameObject.h"
using namespace DirectX;

GameObject::GameObject(ComPtr<ID3D11Device> device)
	:md3dDevice(device)
{
	md3dImmediateContext = nullptr;
	md3dDevice->GetImmediateContext(md3dImmediateContext.GetAddressOf());
}

GameObject::~GameObject()
{

}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3(mWorldMatrix(3, 0), mWorldMatrix(3, 1), mWorldMatrix(3, 2));
}

void GameObject::SetWorldMatrix(const DirectX::XMFLOAT4X4 & world)
{
	mWorldMatrix = world;
}

void GameObject::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	XMStoreFloat4x4(&mWorldMatrix, world);
}

void GameObject::SetTexTransformMatrix(const DirectX::XMFLOAT4X4 & texTransform)
{
	mTexTransform = texTransform;
}

void GameObject::SetTexTransformMatrix(DirectX::FXMMATRIX texTransform)
{
	XMStoreFloat4x4(&mTexTransform, texTransform);
}