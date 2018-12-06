#include "WarTimesPlayer.h"
using namespace DirectX;

SerializeHelper<WarTimes::CPlayer> WarTimes::CPlayer::mSerializeHelper;

WarTimes::CPlayer::CPlayer(
	CObject const& objInfo,
	std::wstring const&  name,
	std::shared_ptr<WarTimes::CWeapon> weapon,
	uint32_t blood) :
	CObject(objInfo),
	mpWeapon(weapon),
	mName(name),
	mBlood(blood),
	mScore(0) {
}

WarTimes::CPlayer::CPlayer(std::wstring const & name)
	:mName(name)
{
}

std::shared_ptr<ISerialize> WarTimes::CPlayer::construct() {
	return std::shared_ptr<ISerialize>(new CPlayer(CObject(), L"", std::dynamic_pointer_cast<CWeapon>(CWeaponSubmachineGun::construct()), WarTimes::BLOOD_MAX));
}

std::wistream & WarTimes::CPlayer::fromString(std::wistream & in) {
	size_t nameLen;
	in >> nameLen;
	in.ignore();
	mName.resize(nameLen);
	in.get(&mName[0], nameLen * sizeof(wchar_t));
	in.ignore();
	in >> mBlood >> mScore;
	mpWeapon->fromString(in);
	return CObject::fromString(in);
}

std::wostream & WarTimes::CPlayer::toString(std::wostream & out) const {
	out << mName.size() << std::endl;
	out << mName << std::endl;
	out << mBlood << std::endl;
	out << mScore << std::endl;
	mpWeapon->toString(out);
	return CObject::toString(out);
}