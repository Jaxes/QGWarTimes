#include "WarTimes.h"

SerializeHelper<WarTimes::CWarTimes> WarTimes::CWarTimes::mSerializeHelper;

void WarTimes::CWarTimes::update()
{
	for (auto& r : this->room)
	{
		r->PlayerNum = r->team[0].size() + r->team[1].size();
	}
}

std::shared_ptr<ISerialize> WarTimes::CWarTimes::construct() {
	return std::shared_ptr<ISerialize>(new CWarTimes());
}

std::wistream & WarTimes::CWarTimes::fromString(std::wistream & in) {
	size_t roomNum;
	in >> roomNum;
	this->room.clear();
	if (roomNum > 0)
	{
		this->room.resize(roomNum);
	}
	for (int i = 0; i < roomNum; i++)
	{
		room[i] = std::shared_ptr<Room>(new Room());
		in >> room[i]->PlayerNum;
		for (int j = 0; j < 2; j++)
		{
			size_t TeamNum = 0;
			in >> TeamNum;
			for (int k = 0; k < TeamNum; k++)
			{
				int seat;
				in >> seat;
				std::shared_ptr<CPlayer> pPlayer = std::dynamic_pointer_cast<CPlayer>(CPlayer::construct());
				pPlayer->fromString(in);
				room[i]->team[j].insert(std::make_pair(seat, pPlayer));
			}
		}
	}
	size_t PlayerInfoSize;
	in >> PlayerInfoSize;
	PlayerRoomInfo.clear();
	for (int i = 0; i < PlayerInfoSize; i++)
	{
		std::wstring PlayerName;
		DirectX::XMINT3 seatPos;
		in >> PlayerName >> seatPos.x >> seatPos.y >> seatPos.z;
		PlayerRoomInfo.insert(std::make_pair(PlayerName, seatPos));
	}
	return in;
}

std::wostream & WarTimes::CWarTimes::toString(std::wostream & out) const {
	size_t roomNum = this->room.size();
	out << roomNum << std::endl;
	for (auto const r : room)
	{
		out << r->PlayerNum << std::endl;
		for (int i = 0; i < 2; i++)
		{
			out << r->team[i].size() << std::endl;
			for (auto const s : r->team[i])
			{
				out << s.first << std::endl;
				s.second->toString(out);
			}
		}
	}
	out << PlayerRoomInfo.size() << std::endl;
	for (auto i : PlayerRoomInfo)
	{
		out << i.first << L" " << i.second.x << L" " << i.second.y << L" " << i.second.z << std::endl;
	}
	return out;
}
