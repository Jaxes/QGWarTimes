#ifndef SENCE_H
#define SENCE_H
#include <WinSock2.h>
#include <vector>
#include <memory>

#include "Sprite2D.h"
#include "Sprite3D.h"
#include "wterr.h"

enum class SENCE
{
	START,
	MODE_SELECT,
	ROOM_SELECT,
	POSITION_SELECT,
	WAIT_OTHERS,
	LOADING,
	SETTLEMENT,
	FIGHT
};

class Sence
{
public:
	Sence(SENCE sence);
	~Sence() = default;

	WTRESULT AddSprite2D(std::shared_ptr<Sprite2D> sprite);
	WTRESULT AddSprite3D(std::shared_ptr<Sprite3D> sprite);
	WTRESULT Draw2D();
	WTRESULT Draw3D();
	SENCE GetSence();
private:
	SENCE sence;
	std::vector<std::shared_ptr<Sprite2D>> mSence2D;
	std::vector<std::shared_ptr<Sprite3D>> mSence3D;
};

#endif //SENCE_H