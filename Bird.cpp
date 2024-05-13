#include "Bird.h"



void Bird::SetDefaultStatus(double posX, double posY, double angle, double velFly, double velRotate)
{
	GetTexturedRectangle().SetPosition(posX, posY);
	GetTexturedRectangle().SetAngle(angle);
	mVelFly = velFly;
	mVelRotate = velRotate;
}

void Bird::SetDefaultFrame(int x, int y, int w, int h)
{
	mFrame.x = x;
	mFrame.y = y;
	mFrame.w = w;
	mFrame.h = h;
	mSpriteWidth = w;
	mSpriteHeight = h;
}



void Bird::DrawFrame(int frame, int slowDownSpeed)
{
	mFrame.x = mSpriteWidth * (int)(frame / slowDownSpeed);
	GetTexturedRectangle().SetClip(&mFrame);
}

void Bird::Fly(double velFly, double angle)
{
	mVelFly = velFly;
	GetTexturedRectangle().SetAngle(angle);
	mVelRotate = 0;

}

void Bird::FreeFall(double accelerationFly, double accelerationRotate)
{
	double posY = GetY();
	posY += mVelFly + accelerationFly;
	mVelFly += accelerationFly;

	double angle = GetAngle();
	angle += mVelRotate + accelerationRotate;
	mVelRotate += accelerationRotate;

	if (angle >= 90) angle = 90;
	GetTexturedRectangle().SetPosition(GetX(), posY);
	GetTexturedRectangle().SetAngle(angle);
}

void Bird::StopOnGround(double posY)
{
	GetTexturedRectangle().SetPosition(GetX(), posY);
}

void Bird::CreateCollisionShape(vector<Point> collisionShape)
{
	/*for (int i = 0; i < collisionShape.size(); i++) {
		mCollisionShape.push_back(collisionShape[i]);
	}*/
	vector<Point> tmp;
	/*for (int i = 0; i < collisionShape.size(); i++) {
		mCollisionInit.push_back({ collisionShape[i].x * GetTexturedRectangle().GetWidth() / (1.0 * mSpriteWidth), collisionShape[i].y * GetTexturedRectangle().GetHeight() / (1.0 * mSpriteHeight) });
		mCollisionShape.push_back({ collisionShape[i].x * GetTexturedRectangle().GetWidth() / (1.0 * mSpriteWidth), collisionShape[i].y * GetTexturedRectangle().GetHeight() / (1.0 * mSpriteHeight) });
	}*/
	for (int i = 0; i < collisionShape.size(); i++) {
		tmp.push_back({ collisionShape[i].x * GetTexturedRectangle().GetWidth() / (1.0 * mSpriteWidth), collisionShape[i].y * GetTexturedRectangle().GetHeight() / (1.0 * mSpriteHeight) });
		//tmp.push_back({ collisionShape[i].x * GetTexturedRectangle().GetWidth() / (1.0 * mSpriteWidth), collisionShape[i].y * GetTexturedRectangle().GetHeight() / (1.0 * mSpriteHeight) });
	}
	mCollisionInit = tmp;
	mCollisionShape = tmp;
	//mCollisionShape = collisionShape;
}

vector<Point>& Bird::GetCollisionShape()
{
	return mCollisionShape;
}

void Bird::ShiftColliders() //dich chuyen hop va cham theo chim
{

	//Go through the dot's collision boxes
	for (int set = 0; set < mCollisionInit.size(); ++set)
	{
		//Center the collision box
		mCollisionShape[set].x = mCollisionInit[set].x + GetX();

		//Set the collision box at its row offset
		mCollisionShape[set].y = mCollisionInit[set].y + GetY();
	}
}

SDL_bool Bird::IsColliding(GameObject& obj)
{
	SDL_Rect tmpRect = obj.GetTexturedRectangle().GetRect();
	Point center = { GetTexturedRectangle().GetRect().x + 1.0 * GetTexturedRectangle().GetRect().w / 2, GetTexturedRectangle().GetRect().y + 1.0 * GetTexturedRectangle().GetRect().h / 2 };
	vector<Point> tmpShape = GetCollisionShape();

	return CheckCollision2(tmpShape, GetAngle(), center, &tmpRect);
}

void Bird::SetNumberOfFrames(int numOfFrames)
{
	mNumberOfFrames = numOfFrames;
}

int Bird::GetNumberOfFrames()
{
	return mNumberOfFrames;
}
