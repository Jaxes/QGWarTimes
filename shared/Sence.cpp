#include "Sence.h"

Sence::Sence(SENCE sence)
{
	this->sence = sence;
}

WTRESULT Sence::AddSprite2D(std::shared_ptr<Sprite2D> sprite)
{
	this->mSence2D.push_back(sprite);
	return WTRESULT_OK;
}

WTRESULT Sence::AddSprite3D(std::shared_ptr<Sprite3D> sprite)
{
	this->mSence3D.push_back(sprite);
	return WTRESULT_OK;
}

WTRESULT Sence::Draw2D()
{
	if (this->mSence2D.size() > 0)
	{
		for (auto const& i: this->mSence2D)
		{
			if (i->GetCanSee())
			{
				i->Update();
				i->Draw();
			}
		}
	}
	return WTRESULT_OK;
}

WTRESULT Sence::Draw3D()
{
	if (this->mSence3D.size() > 0)
	{
		for (auto const& i : this->mSence3D)
		{
			if (i->GetCanSee())
			{
				i->Update();
				i->Draw();
			}
		}
	}
	return WTRESULT_OK;
}

SENCE Sence::GetSence()
{
	WTINFO(L"%s", WTStrErr(WTRESULT_OK));
	return this->sence;
}
