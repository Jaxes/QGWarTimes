#ifndef CONSTANTBUFFER_H
#define CONSTANTBUFFER_H
#include <DirectXMath.h>
#include <LightHelper.h>

struct CBChangesEveryDrawing
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX worldInvTranspose;
	DirectX::XMMATRIX texTransform;
};

struct CBChangesEveryFrame
{
	DirectX::XMMATRIX view;
	DirectX::XMFLOAT4 eyePos;
};

struct CBChangesOnResize
{
	DirectX::XMMATRIX proj;
};

struct CBNeverChange
{
	DirectionalLight dirLight[10];
	PointLight pointLight[10];
	SpotLight spotLight[10];
	int numDirLight;
	int numPointLight;
	int numSpotLight;
	float pad;		// 打包保证16字节对齐
};

struct CBMaterial
{
	Material material;
};

#endif //CONSTANTBUFFER_H