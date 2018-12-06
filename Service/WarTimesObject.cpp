#include "WarTimesObject.h"

using namespace DirectX;

std::wostream& operator<< (std::wostream& out, DirectX::XMVECTOR const& float3) {
	out << DirectX::XMVectorGetX(float3) << " ";
	out << DirectX::XMVectorGetY(float3) << " ";
	out << DirectX::XMVectorGetZ(float3);
	return out;
}

std::wistream& operator>> (std::wistream& in, DirectX::XMVECTOR & float3) {
	float x, y, z;
	in >> x >> y >> z;
	float3 = DirectX::XMVectorSet(x, y, z, 0);
	return in;
}

std::wostream& operator<< (std::wostream& out, DirectX::XMFLOAT4X4 const& mat) {
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			out << mat.m[i][j];
			if (i != 3 || j != 3)
			{
				out << " ";
			}
		}
	}
	return out;
}

std::wistream& operator>> (std::wistream& in, DirectX::XMFLOAT4X4 & mat) {
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			in >> mat.m[i][j];
		}
	}
	return in;
}

WarTimes::CObject::CObject(DirectX::XMFLOAT4X4 & world)
	:mWorldMatrix(world),
	mUp(XMVectorSet(0.0f, 1.0f, 0.0f,0.0f)),
	mLook(XMVectorSet(-1.0f, 0.0f, 0.0f,0.0f)),
	mRight(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
	mFront(mLook)
{
}

std::shared_ptr<ISerialize> WarTimes::CObject::construct() {
	return std::shared_ptr<ISerialize>(new CObject());
}

std::wistream & WarTimes::CObject::fromString(std::wistream & in) {
	in >> mRight;
	in >> mUp;
	in >> mLook;
	in >> mFront;
	in >> mWorldMatrix;
	in.ignore();
	return in;
}

std::wostream & WarTimes::CObject::toString(std::wostream & out) const {
	out << mRight << std::endl;
	out << mUp << std::endl;
	out << mLook << std::endl;
	out << mFront << std::endl;
	out << mWorldMatrix << std::endl;
	return out;
}
