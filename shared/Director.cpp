#include "Director.h"

Director::Director(ComPtr<ID3D11DeviceContext> immediateContext)
	:md3dImmediateContext(immediateContext)
{
	this->sence = SENCE::START;
}

Director::~Director()
{

}

WTRESULT Director::AddSence(std::shared_ptr<Sence> sence)
{
	this->mDirector.push_back(sence);
	return WTRESULT_OK;
}

WTRESULT Director::DirectorDrawSence()
{
	for (auto const& i : mDirector)
	{
		if (i->GetSence() == sence)
		{
			md3dImmediateContext->OMSetDepthStencilState(nullptr, 0);
			md3dImmediateContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
			WTR(i->Draw3D());
			md3dImmediateContext->OMSetDepthStencilState(RenderStates::DSSNoDepthTest.Get(), 1);
			md3dImmediateContext->OMSetBlendState(RenderStates::BSTransparent.Get(), nullptr, 0xFFFFFFFF);
			WTR(i->Draw2D());
			return WTRESULT_OK;
		}
	}
	return WTRESULT_OK;
}

WTRESULT Director::SetSence(SENCE sence)
{
	this->sence = sence;
	return WTRESULT_OK;
}

SENCE Director::GetSence()
{
	return this->sence;
}
