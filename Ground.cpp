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
	GetTexturedRectangle().SetPosition(mScrollingGround, GetY());//thiet lap vi tri doan dat cuon
	Render();
	GetTexturedRectangle().SetPosition(mScrollingGround + GetTexturedRectangle().GetWidth(), GetY());// doan dat cuon tiep theo ben phai
	Render();
}
