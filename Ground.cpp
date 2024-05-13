#include "Ground.h"

void Ground::Scroll(int speedScrolling)
{
	mScrollingGround -= speedScrolling;
	if (mScrollingGround < -GetTexturedRectangle().GetWidth()) {
		mScrollingGround = 0; // -> hieu ung cuon vo han
	}
	GetTexturedRectangle().SetPosition(mScrollingGround, GetY());
}

void Ground::RenderScrolling()
{
	GetTexturedRectangle().SetPosition(mScrollingGround, GetY());
	Render();
	GetTexturedRectangle().SetPosition(mScrollingGround + GetTexturedRectangle().GetWidth(), GetY());
	Render();
}
