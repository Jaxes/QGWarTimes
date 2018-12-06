#include "WarTimesWeapon.h"

using namespace WarTimes;

//*********************************** CWeapon 武器基类

WarTimes::CWeapon::CWeapon(CObject objInfo, uint32_t power) :
	CObject(objInfo),
	mPower(power) {
}

std::wistream & WarTimes::CWeapon::fromString(std::wistream & in) {
	in >> mPower;
	in.ignore();
	return CObject::fromString(in);
}

std::wostream & WarTimes::CWeapon::toString(std::wostream & out) const {
	out << mPower << std::endl;
	return CObject::toString(out);
}

//*********************************** 冲锋枪

SerializeHelper<CWeaponSubmachineGun> CWeaponSubmachineGun::mSerializeHelper;

WarTimes::CWeaponSubmachineGun::CWeaponSubmachineGun(
	CObject const& objInfo,
	uint32_t power,
	uint32_t bullet) :
	CWeapon(objInfo, power),
	mBullet(bullet) {
}

// 攻击, 成功返回 0, 否则返回非 0
int WarTimes::CWeaponSubmachineGun::Attack() {
	if(mBullet > 0) {
		mBullet -= 1;
		return 0;
	} else {
		return -1;
	}
}

std::shared_ptr<ISerialize> WarTimes::CWeaponSubmachineGun::construct() {
	return std::shared_ptr<ISerialize>(new CWeaponSubmachineGun(CObject()));
}

std::wistream & WarTimes::CWeaponSubmachineGun::fromString(std::wistream & in) {
	in >> mBullet;
	in.ignore();
	return CWeapon::fromString(in);
}

std::wostream & WarTimes::CWeaponSubmachineGun::toString(std::wostream & out) const {
	out << mBullet << std::endl;
	return CWeapon::toString(out);
}

//*********************************** 霰弹枪

SerializeHelper<CWeaponShotGun> CWeaponShotGun::mSerializeHelper;

WarTimes::CWeaponShotGun::CWeaponShotGun(CObject const& objInfo, uint32_t power, uint32_t bullet) :
	CWeapon(objInfo, power),
	mBullet(bullet) {
}

int WarTimes::CWeaponShotGun::Attack() {
	if(mBullet > 0) {
		mBullet -= 1;
		return 0;
	} else {
		return -1;
	}
}

std::shared_ptr<ISerialize> WarTimes::CWeaponShotGun::construct() {
	return std::shared_ptr<ISerialize>(new CWeaponShotGun(CObject()));
}

std::wistream & WarTimes::CWeaponShotGun::fromString(std::wistream & in) {
	in >> mBullet;
	in.ignore();
	return CWeapon::fromString(in);
}

std::wostream & WarTimes::CWeaponShotGun::toString(std::wostream & out) const {
	out << mBullet << std::endl;
	return CWeapon::toString(out);
}
