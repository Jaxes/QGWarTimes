#ifndef SPRITE3D_H
#define SPRITE3D_H

#include <WinSock2.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <DirectXCollision.h>
#include <string>
#include <fstream>
#include <sstream>
#include <mutex>

#include "GameObject.h"
#include "d3dApp.h"
#include "ConstantBuffer.h"
#include "Vertex.h"
#include "wterr.h"
#include "WarTimesObject.h"
#include "LightHelper.h"

enum class ModelMode
{
	BOX,              //������
	SPHERE,           //����
	CYLINDER,         //Բ����
	PLANE,            //��άƽ��
	FROM_FILE         //���ļ���ȡ
};

class isEqual
{
public:
	isEqual(DirectX::XMFLOAT3& vertexs);
	bool operator()(const std::vector<DirectX::XMFLOAT3>::value_type& value);
private:
	DirectX::XMFLOAT3 Vertexs;
};

class Sprite3D : public GameObject
{
public:
	//���캯��
	Sprite3D(
		ComPtr<ID3D11VertexShader> VertexShader3D,
		ComPtr<ID3D11PixelShader> PixelShader3D,
		ComPtr<ID3D11InputLayout> vertexLayout3D,
		ComPtr<ID3D11Device> device,
		ModelMode mode,
		std::string SourceDir = "",
		ComPtr<ID3D11SamplerState> samplerState = nullptr);
	Sprite3D() = default;

	//��������
	~Sprite3D();

	//���ι���
	WTRESULT init();
	WTRESULT init(DirectX::XMFLOAT3 center, DirectX::XMFLOAT2 planeSize, DirectX::XMFLOAT2 maxTexCoord);
	WTRESULT init(float centerX, float centerY, float centerZ, float width, float depth, float texU, float texV);
	WTRESULT init(float radius, int levels, int slices);
	WTRESULT init(float width, float height, float depth);
	WTRESULT init(float radius, float height, int slices);


	//���벼�֡���ɫ���󶨡�����/������������������
	WTRESULT Update();

	//�����������ת
	WTRESULT RotationX(float angle);
	WTRESULT RotationY(float angle);
	WTRESULT RotationZ(float angle);

	//��������������ת
	WTRESULT RotationAround(DirectX::XMFLOAT3 axis, DirectX::XMFLOAT3 axisPos, float angle, float Max = -1, DirectX::XMFLOAT3 ReferenceVector = { 0.0f,0.0f,0.0f });

	//��������
	WTRESULT Scaling(DirectX::XMFLOAT3 scale);

	//�����ƶ�
	WTRESULT Translation(DirectX::XMFLOAT3 translation);

	//���þ���λ��
	WTRESULT SetPosition(const DirectX::XMFLOAT3& pos);

	//��ȡλ������
	DirectX::XMFLOAT3 GetLook();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetFront();

	// ����
	void Draw();

	//���û�����
	WTRESULT SetBuffer();

	//��������
	WTRESULT ResetMesh();

	WTRESULT SetCanSee(bool canSee);

	bool GetCanSee();

	//����Ƿ񱻻���
	bool CheckShooted(DirectX::XMFLOAT3 Origin, DirectX::XMFLOAT3 Direction);

	//�����ײ
	bool CheckCollision(Sprite3D& Other);

	DirectX::BoundingBox GetCollisionBox();

	//�����ײ��
	WTRESULT SetCollisionBox();

	//ͬ�����ݸ��ɴ�������ݶ���
	WTRESULT DataSync(std::shared_ptr<WarTimes::CObject> pObject);

	//�ӿɴ�������ݶ����л�ȡ����
	WTRESULT DataGet(std::shared_ptr<WarTimes::CObject> pObject);

private:
	// ��������ģ�����ݣ�levels��slicesԽ�󣬾���Խ�ߡ�
	void CreateSphere(float radius, int levels, int slices);

	// ����������ģ������
	void CreateBox(float width, float height, float depth);

	// ����Բ����ģ�����ݣ�slicesԽ�󣬾���Խ�ߡ�
	void CreateCylinder(float radius, float height, int slices);

	// ����Բ����ֻ�в����ģ�����ݣ�slicesԽ�󣬾���Խ��
	MeshData CreateCylinderNoCap(float radius, float height, int slices);

	// ����һ��ƽ��
	void CreatePlane(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT2& planeSize, const DirectX::XMFLOAT2& maxTexCoord);
	void CreatePlane(float centerX, float centerY, float centerZ, float width, float depth, float texU, float texV);

	std::wstring toWString(std::string str);

	// ���ļ���ֱ�Ӽ��ر�����ڴ�����
	// �ļ�ֻ���� SaveVertexToFile ��������ļ�
	WTRESULT ReadVertexFromFile(std::string dir, std::string filename);
	// ��MTL�ж�ȡDDS���� �ļ�������(��)б�ܿ�ͷ, �ļ���·����(��)б�ܽ�β
	WTRESULT ReadVertexAttrFromMTL(std::string dir, std::string filename);

	// ����MTL�е���Ϣ��ȡ�ļ� �ļ���·����(��)б�ܽ�β, ��ʾMTLȥ��ȡ����ʱ���õ����·��
	WTRESULT CreateTextureView(std::string dir);

	//������Դ��ͼ
	std::vector<ComPtr<ID3D11ShaderResourceView>> mTexture;

	//��ɫ��
	ComPtr<ID3D11VertexShader> mVertexShader3D;				// ����3D�Ķ�����ɫ��
	ComPtr<ID3D11PixelShader> mPixelShader3D;				// ����3D��������ɫ��

	//��������
	std::vector<Material> mMaterial;
	std::vector<ComPtr<ID3D11Buffer>> mMaterials;

	//������
	ComPtr<ID3D11SamplerState> mSamplerState;

	//���벼��
	ComPtr<ID3D11InputLayout> mVertexLayout3D;

	//��ģ��
	struct Submodel {
		uint32_t start;
		uint32_t size;
		size_t texIndex;
		std::string mName;     //����ʶ�����ĸ�����
	};
	std::vector<Submodel> mModelIndexRanges;

	//ģ����������
	VertexModel meshData;                                  //���ڴ��ļ���ȡ��ģ������
	MeshData mMeshData;                                    //�����Լ����ɵ�ģ������

	//ģ�Ͳ���
	std::vector<SubModelAttr> mModelAttr;

	//����λ������
	DirectX::XMFLOAT3 mRight;
	DirectX::XMFLOAT3 mUp;
	DirectX::XMFLOAT3 mLook;
	DirectX::XMFLOAT3 mFront;

	//�����ģʽ
	ModelMode mode;

	//�Ƿ�ɼ�
	bool isCanSee;

	//��ײ��
	DirectX::BoundingBox mCollisionBox;
};

#endif //SPRITE3D_H