#include "SDLApp.h"
#include "TexturedRectangle.h"
#include "GameObject.h"
#include "Bird.h"
#include "Pipe.h"
#include "Ground.h"
#include "CheckCollision.h"
#include "Text.h"
#include "Sound.h"
#include "Button.h"
#include <vector>
#include <fstream>
#include <random>
#include <chrono>

//Screen constant
const int SCREEN_HEIGHT = 720;
const int SCREEN_WIDTH = 1280;
const int SPEED_SCROLLING_SCREEN = 3;

//Bird constant
const int DEFAULT_SPEED_FLY = -8;
const double DEFAULT_ANGLE = -30; //cho chim huong len goc 30 do khi bay
const int SLOW_DOWN_RENDER_BIRD = 4;//dieu chinh do muot
const int BIRD_HEIGHT = 610;
const int BIRD_WIDTH = 2868;
const int DEFAULT_BIRD_X = 256;

//Pipe constant
const int TOTAL_PIPE = 5;
const int MAX_PIPE_Y = 550;
const int MIN_PIPE_Y = 250;
const int PIPE_DISTANCE = 175; //khoang cach 1 cap ong nuoc
const int SPEED_MOVING_VERTICAL = 1;

//Score constant
const int SCORE_X = 10;
const int SCORE_Y = 640;
const int SCORE_WIDTH_CHAR = 15;
const int SCORE_HEIGHT_CHAR = 28;

//Ground constant
const int GROUND_Y = 600;

//Store character Y and width frame
const vector<pair<int, int>> CharacterProperties = { {0, 712}, {520, 713}, {1061, 667}, {1647, 760}, {2250, 717}, {2860, 716}, {3492, 0} };

//Declare main game
SDLApp* app;

//Objects in gameplay
GameObject* background;
Bird* flappyBird;
Ground* ground;
vector<Pipe*> pipe(TOTAL_PIPE);
GameObject* flash;
Text* score;
Text* highScore;
SoundEffect* soundFly;
SoundEffect* soundGetPoint;
SoundEffect* soundDie;
SoundEffect* soundHit;

//Objects in main menu
TexturedRectangle* logo;
Button* startGame;
Button* nextLeft;
Button* nextRight;
Bird* character;
Music* bgMusic;
Button* setting;
TexturedRectangle* settingBoard;
Button* musicOnOff;
Button* soundOnOff;
TexturedRectangle* blackTransition;


//Objects in game over
TexturedRectangle* gameOver;
TexturedRectangle* scoreBoard;
TexturedRectangle* newHighScore;
Text* gameOverScore;
Text* gameOverHighScore;
Button* replay;
Button* home;
SoundEffect* newRecord;





bool isMainMenu = true; //Signal to open main menu

bool isStartingGame = false; //Signal to start game

bool isResetingGame = true; //Signal to reset game, thiet lap lai 1 so trang thai ban dau -> true

//Check if a key is holding or not
//to avoid continuously holding a key so that the bird is flying high faster
bool isHoldingKey = false;

bool isPlayingMusic = false;

bool isPlayingNewRecord = false;

bool isHighScore = false;

bool isSetting = false;

bool repeatFly = false; //Make sure the bird flaps wing only when it flys (space keydown)

//Global variables to handle screen flickering
bool isFlash = false; //Signal to flicker the screen (white flash) when the bird collides
int alphaFlash = SDL_ALPHA_OPAQUE; //Adjust the transparency of white screen when flashing, default = 255 (white)
int mode = 13;//Adjust the white flash appearing speed

int point = 0; //Store point on gameplay

string hScore = "0"; //Temporarily store high score on gameplay
ifstream fin("bin/score.bin", ios::binary | ios::in); //Open binary file storing high score

int numCharacter = 0;
int numberOfFrame = 4;
static int frame = 0; //Store frame number to render bird flying

int alphaBlackTransition = 0;
bool isBlackTransition = false; //if true, the screen will gradually become black.
int volumeMusic = MIX_MAX_VOLUME; //max value in SDL_mixer

int alphaGameOver = 0;




void HandleEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {

		if (event.type == SDL_QUIT) {

			//Store high score before closing game
			int sc;
			fin.read((char*)&sc, sizeof(sc));
			if (sc < stoi(hScore)) {
				int tmphScore = stoi(hScore);
				ofstream fout("bin/score.bin", ios::binary | ios::out | ios::trunc);
				fout.write((char*)&tmphScore, sizeof(int));
			}

			app->EndAppLoop();

		}

		if (event.type == SDL_MOUSEMOTION) {
			startGame->HandleMouseOver();
			replay->HandleMouseOver();
			home->HandleMouseOver();
			//setting->HandleMouseOver();
		}
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			startGame->HandleMouseDown();
			nextLeft->HandleMouseDown();
			nextRight->HandleMouseDown();
			replay->HandleMouseDown();
			home->HandleMouseDown();
			setting->HandleMouseDown();
			soundOnOff->HandleMouseDown();
			musicOnOff->HandleMouseDown();
		}

		//Game reseting process when pressing S key
		if (event.key.keysym.sym == SDLK_s || replay->IsClicking()) {

			if (!isResetingGame && !isStartingGame) { //khi dang choi hoac dang reset se khong reset duoc
				isResetingGame = true;

				flappyBird->SetDefaultStatus(DEFAULT_BIRD_X, GROUND_Y / 2, 0, 0, 0); // angle =0, velFly =  velRotate = 0;

				for (int i = 0; i < TOTAL_PIPE; i++) {
					pipe[i]->SetStatus(SCREEN_WIDTH + i * SCREEN_WIDTH / (TOTAL_PIPE - 1), MAX_PIPE_Y, MIN_PIPE_Y, PIPE_DISTANCE);
					pipe[i]->SetMode();

				}

				alphaFlash = SDL_ALPHA_OPAQUE;
				isFlash = false;

				point = 0;
				score->SetSize(SCORE_X, SCORE_Y, SCORE_WIDTH_CHAR * 8, SCORE_HEIGHT_CHAR);
				score->ChangeText("Score: 0"); //cap nhat lai size

				highScore->SetSize(SCORE_X, SCORE_Y + SCORE_HEIGHT_CHAR + 2, SCORE_WIDTH_CHAR * (12 + hScore.length()), SCORE_HEIGHT_CHAR);
				highScore->ChangeText("High score: " + hScore);
				isHighScore = false;
				isPlayingNewRecord = false;


			}

		}
		//return main menu
		if (event.key.keysym.sym == SDLK_m || home->IsClicking()) {
			isPlayingMusic = false;
			isMainMenu = true;
			isStartingGame = false;
			isResetingGame = true;
			isSetting = false;
			flappyBird->SetDefaultStatus(DEFAULT_BIRD_X, GROUND_Y / 2, 0, 0, 0);

			for (int i = 0; i < TOTAL_PIPE; i++) {
				pipe[i]->SetStatus(SCREEN_WIDTH + i * SCREEN_WIDTH / (TOTAL_PIPE - 1), MAX_PIPE_Y, MIN_PIPE_Y, PIPE_DISTANCE);
				pipe[i]->SetMode();

			}
			volumeMusic = MIX_MAX_VOLUME;
			alphaFlash = SDL_ALPHA_OPAQUE;
			isFlash = false;

			point = 0;
			score->SetSize(SCORE_X, SCORE_Y, SCORE_WIDTH_CHAR * 8, SCORE_HEIGHT_CHAR);
			score->ChangeText("Score: 0");

			highScore->SetSize(SCORE_X, SCORE_Y + SCORE_HEIGHT_CHAR + 2, SCORE_WIDTH_CHAR * (12 + hScore.length()), SCORE_HEIGHT_CHAR);
			highScore->ChangeText("High score: " + hScore);

			alphaBlackTransition = 0;
			isBlackTransition = false;

			//volumeMusic = MIX_MAX_VOLUME;
			isHighScore = false;

			isPlayingNewRecord = false;
		}
		//Handle events when pressing Spacebar
		if (event.key.keysym.sym == SDLK_SPACE) {
			if (event.type == SDL_KEYDOWN) {
				if (!isMainMenu) {
					//Handle starting game
					if (!isStartingGame && isResetingGame && alphaBlackTransition == 0 ) { //man hinh dang hien len roi moi choi duoc
						isStartingGame = true;
						isResetingGame = false;
					}

					//Handle bird flying
					if (!isHoldingKey && isStartingGame) {
						flappyBird->Fly(DEFAULT_SPEED_FLY, DEFAULT_ANGLE);
						flappyBird->DrawFrame(0, SLOW_DOWN_RENDER_BIRD);
						flappyBird->ShiftColliders();
						soundFly->PlaySound(0);
						frame = 0; //dat lai trang thai frame dau tien de ve tiep
						isHoldingKey = true;
						repeatFly = false;

					}
				}
			}
			else {
				isHoldingKey = false; //Prevent holding Spacebar to help the bird continuously fly high
			}

		}


		//Reset high score when pressing R key
		if (event.key.keysym.sym == SDLK_r) {
			if (!isStartingGame) {
				ofstream fout("bin/score.bin", ios::trunc | ios::binary | ios::out);
				hScore = "0";
				fout.write((char*)&hScore, sizeof(int));
				highScore->ChangeText("High score: 0");
				highScore->SetSize(SCORE_X, SCORE_Y + SCORE_HEIGHT_CHAR + 2, SCORE_WIDTH_CHAR * 13, SCORE_HEIGHT_CHAR);
			}
		}

		if (event.type == SDLK_LEFT) {
			nextLeft->HandleMouseDown();
		}
		if (event.type == SDLK_RIGHT) {
			nextRight->HandleMouseDown();
		}
		if (nextLeft->IsClicking() || (event.key.keysym.sym == SDLK_LEFT && event.type == SDL_KEYDOWN)) {
			if (numCharacter == 0) numCharacter = 5;
			else --numCharacter;
		}
		if (nextRight->IsClicking() || (event.key.keysym.sym == SDLK_RIGHT && event.type == SDL_KEYDOWN)) {
			if (numCharacter == 5) numCharacter = 0;
			else ++numCharacter;
		}
		if (startGame->IsClicking()) {
			isBlackTransition = true;
			//Create shape to check collision for bird
			vector<Point> birdCollisionBox;
			switch (numCharacter) {
			case 0:
			{
				birdCollisionBox.push_back({ 708, 294 });
				birdCollisionBox.push_back({ 667, 134 });
				birdCollisionBox.push_back({ 467, 74 });
				birdCollisionBox.push_back({ 262, 133 });
				birdCollisionBox.push_back({ 145, 333 });
				birdCollisionBox.push_back({ 273, 485 });
				birdCollisionBox.push_back({ 577, 485 });
				break;
			}
			case 1:
			{
				birdCollisionBox.push_back({ 707, 345 });
				birdCollisionBox.push_back({ 657, 110 });
				birdCollisionBox.push_back({ 426, 0 });
				birdCollisionBox.push_back({ 218, 78 });
				birdCollisionBox.push_back({ 151, 201 });
				birdCollisionBox.push_back({ 181, 430 });
				birdCollisionBox.push_back({ 451, 430 });
				break;
			}
			case 2:
			{
				birdCollisionBox.push_back({ 662, 407 });
				birdCollisionBox.push_back({ 590, 147 });
				birdCollisionBox.push_back({ 398, 67 });
				birdCollisionBox.push_back({ 247, 113 });
				birdCollisionBox.push_back({ 114, 291 });
				birdCollisionBox.push_back({ 182, 448 });
				birdCollisionBox.push_back({ 389, 491 });
				break;
			}
			case 3:
			{
				birdCollisionBox.push_back({ 755, 340 });
				birdCollisionBox.push_back({ 570, 0 });
				birdCollisionBox.push_back({ 310, 0 });
				birdCollisionBox.push_back({ 161, 297 });
				birdCollisionBox.push_back({ 211, 467 });
				birdCollisionBox.push_back({ 418, 523 });
				birdCollisionBox.push_back({ 678, 533 });
				break;
			}
			case 4:
			{
				birdCollisionBox.push_back({ 714, 550 });
				birdCollisionBox.push_back({ 714, 382 });
				birdCollisionBox.push_back({ 655, 77 });
				birdCollisionBox.push_back({ 470, 0 });
				birdCollisionBox.push_back({ 319, 108 });
				birdCollisionBox.push_back({ 139, 312 });
				birdCollisionBox.push_back({ 139, 550 });
				break;
			}
			default:
			{
				birdCollisionBox.push_back({ 712, 428 });
				birdCollisionBox.push_back({ 657, 201 });
				birdCollisionBox.push_back({ 428, 92 });
				birdCollisionBox.push_back({ 248, 160 });
				birdCollisionBox.push_back({ 156, 284 });
				birdCollisionBox.push_back({ 221, 490 });
				birdCollisionBox.push_back({ 412, 519 });
				break;
			}
			}

			flappyBird->CreateCollisionShape(birdCollisionBox);
		}
		if (setting->IsClicking()) {
			if (isSetting) { //dang mo, nhan thi dong
				isSetting = false;
			}
			else isSetting = true; //nguoc lai
		}
		if (soundOnOff->IsClicking()) {
			int volume = 0;
			if (soundOnOff->GetClip().x == 0) { //dang bat
				soundOnOff->SetClip(432, 0, 432, 171);
				volume = 0;
			}
			else {
				soundOnOff->SetClip(0, 0, 432, 171);
				volume = MIX_MAX_VOLUME;
			}
			newRecord->SetVolume(volume);
			soundFly->SetVolume(volume);
			soundGetPoint->SetVolume(volume);
			soundHit->SetVolume(volume);
			soundDie->SetVolume(volume);
		}
		if (musicOnOff->IsClicking()) {
			if (musicOnOff->GetClip().x == 0) {
				musicOnOff->SetClip(432, 0, 432, 171);
				//volumeMusic = 0;
				bgMusic->PauseSound();
			}
			else {
				musicOnOff->SetClip(0, 0, 432, 171);
				//volumeMusic = MIX_MAX_VOLUME;
				if (isPlayingMusic) {
					bgMusic->ResumeSound();
				}
				//else {
				//
				//	bgMusic->PlaySound(-1);
				//	isPlayingMusic = true;
				//}
			}
		}


	}
}

void HandleRendering() {

	if (isMainMenu) {
		background->Render();
		if (!isPlayingMusic && musicOnOff->GetClip().x == 0) {
			bgMusic->PlaySound(-1);
			isPlayingMusic = true;
		}
		character->SetDefaultFrame(0, CharacterProperties[numCharacter].first, CharacterProperties[numCharacter].second, CharacterProperties[numCharacter + 1].first - CharacterProperties[numCharacter].first);
		character->DrawFrame(0, 4);

		logo->Render(app->GetRenderer());
		nextLeft->Render();
		nextRight->Render();
		character->Render();
		startGame->Render();
		setting->Render();

		if (isSetting) {
			settingBoard->Render(app->GetRenderer());
			soundOnOff->Render();
			musicOnOff->Render();
		}

		ground->Scroll(SPEED_SCROLLING_SCREEN);
		ground->RenderScrolling();

		if (isBlackTransition) {
			alphaBlackTransition += 10;
			volumeMusic -= 5;
			if (volumeMusic < 0) volumeMusic = 0;


			if (alphaBlackTransition > SDL_ALPHA_OPAQUE) { //khi vuot qua nguong -> trang thai khong trong suot
				alphaBlackTransition = SDL_ALPHA_OPAQUE; //gan gia tri -> dam bao da hoan toan chuyen den
				isMainMenu = false;
				isResetingGame = true;
				bgMusic->StopSound();
			}
			blackTransition->SetAlpha(alphaBlackTransition);
			blackTransition->Render(app->GetRenderer());
		}
		bgMusic->SetVolume(volumeMusic);



	}
	else {
		if (isResetingGame) {
			background->Render();

			if (numCharacter == 0 || numCharacter == 1 || numCharacter == 5) {
				numberOfFrame = 2;
			}
			else {
				numberOfFrame = 4;
			}

			++frame; //một khung hình mới đã được vẽ hoặc hiển thị.
			if (frame / SLOW_DOWN_RENDER_BIRD >= numberOfFrame) {
				frame = 0;
			} //

			ground->Scroll(SPEED_SCROLLING_SCREEN);
			ground->RenderScrolling();
			if (isBlackTransition) {
				alphaBlackTransition -= 10;


				if (alphaBlackTransition < 0) {
					alphaBlackTransition = 0;
					isBlackTransition = false;
				}
				blackTransition->SetAlpha(alphaBlackTransition);

			}


		}
		else {

			if (isStartingGame) {

				background->Render();

				if (flappyBird->GetY() < GROUND_Y - 52) {
					flappyBird->FreeFall(0.5, 0.1);
				}
				else {
					flappyBird->StopOnGround(GROUND_Y - 52);
					isStartingGame = false;
					soundHit->PlaySound(0);

					if (point > stoi(hScore)) {
						hScore = to_string(point);
						isHighScore = true;
					}
				}
				++frame;
				if (frame / SLOW_DOWN_RENDER_BIRD >= numberOfFrame && !repeatFly) {
					frame = SLOW_DOWN_RENDER_BIRD;
					repeatFly = true;
				}
				else if (repeatFly) {
					if (numCharacter == 0 || numCharacter == 1 || numCharacter == 5) {
						frame = 0;
					}
					else {
						frame = SLOW_DOWN_RENDER_BIRD;
					}
				}
				flappyBird->ShiftColliders();


				for (int i = 0; i < TOTAL_PIPE; i++) {
					pipe[i]->MoveHorizontal(SPEED_SCROLLING_SCREEN);
					pipe[i]->Render();

					//If pipe moves out of screen, reset normal status and pipe mode
					if (pipe[i]->GetPipeX() < -(pipe[i]->GetPipeWidth())) {
						pipe[i]->SetStatus(SCREEN_WIDTH + SCREEN_WIDTH / (TOTAL_PIPE - 1) - pipe[i]->GetPipeWidth(), MAX_PIPE_Y, MIN_PIPE_Y, PIPE_DISTANCE);
						pipe[i]->SetMode();
					}
					if (pipe[i]->IsMoving()) {
						if (pipe[i]->GetPipeY() < MIN_PIPE_Y) pipe[i]->SetMoveDown(true);
						else if (pipe[i]->GetPipeY() > MAX_PIPE_Y) pipe[i]->SetMoveDown(false);
						pipe[i]->MoveVertical(SPEED_MOVING_VERTICAL, pipe[i]->IsMovingDown());
					}
					if (pipe[i]->DetectScoring(flappyBird)) {
						++point;
						string s = "Score: " + to_string(point);
						score->SetSize(SCORE_X, SCORE_Y, SCORE_WIDTH_CHAR * s.length(), SCORE_HEIGHT_CHAR);
						score->ChangeText(s);
						soundGetPoint->PlaySound(0);

					}


					if (flappyBird->IsColliding(pipe[i]->GetPipeUp()) == SDL_TRUE || flappyBird->IsColliding(pipe[i]->GetPipeDown()) == SDL_TRUE) {

						isStartingGame = false;
						soundHit->PlaySound(0);

						if (point > stoi(hScore)) {
							hScore = to_string(point);
							isHighScore = true;
						}
					}

				}

				ground->Scroll(SPEED_SCROLLING_SCREEN);

			}
			else {
				background->Render();
				for (int i = 0; i < TOTAL_PIPE; i++) {
					pipe[i]->Render();
				}
				if (flappyBird->GetY() < GROUND_Y - 52) {
					flappyBird->FreeFall(0.5, 0.1);


				}
				else {
					flappyBird->StopOnGround(GROUND_Y - 52);
					alphaGameOver += 10;
					if (alphaGameOver > SDL_ALPHA_OPAQUE) alphaGameOver = SDL_ALPHA_OPAQUE; //khong trong suot

					gameOver->SetAlpha(alphaGameOver);
					scoreBoard->SetAlpha(alphaGameOver);
					gameOver->Render(app->GetRenderer());
					scoreBoard->Render(app->GetRenderer());
					if (alphaGameOver == SDL_ALPHA_OPAQUE) {
						string p = to_string(point);
						gameOverScore->ChangeText(p);
						gameOverScore->SetSize(SCREEN_WIDTH / 2 + 175 - (SCORE_WIDTH_CHAR + 10) * (p.length() - 1), SCREEN_HEIGHT / 2 - 65, (SCORE_WIDTH_CHAR + 10) * p.length(), SCORE_HEIGHT_CHAR + 20);
						gameOverScore->Render();

						gameOverHighScore->ChangeText(hScore);
						gameOverHighScore->SetSize(SCREEN_WIDTH / 2 + 175 - (SCORE_WIDTH_CHAR + 10) * (hScore.length() - 1), SCREEN_HEIGHT / 2 + 20, (SCORE_WIDTH_CHAR + 10)* hScore.length(), SCORE_HEIGHT_CHAR + 20);
						gameOverHighScore->Render();
					}
					replay->Render();
					home->Render();
					if (isHighScore) {
						newHighScore->Render(app->GetRenderer());
						if (!isPlayingNewRecord) {
							newRecord->PlaySound(0);
							isPlayingNewRecord = true;
						}

					}


				}


			}
		}

		flappyBird->SetDefaultFrame(0, CharacterProperties[numCharacter].first, CharacterProperties[numCharacter].second, CharacterProperties[numCharacter + 1].first - CharacterProperties[numCharacter].first);
		flappyBird->DrawFrame(frame, SLOW_DOWN_RENDER_BIRD);
		flappyBird->Render();
		ground->RenderScrolling();
		score->Render();
		highScore->Render();
		if (!isResetingGame && !isStartingGame && !isFlash) {

			flash->GetTexturedRectangle().SetAlpha(alphaFlash);

			if (alphaFlash >= SDL_ALPHA_OPAQUE) {
				mode = -13;
			}
			alphaFlash += mode;
			flash->Render();

			if (alphaFlash < 0) {
				isFlash = true;

			}
		}
		//setting->Render();
		blackTransition->Render(app->GetRenderer());
	}




}

int main(int argc, char* args[]) {

	//Setup the SDLApp
	const char* title = "Flappy Bird";
	app = new SDLApp(IMG_INIT_PNG, title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT);

	//Initialize background
	background = new GameObject(app->GetRenderer(), "asset/image/backgroundgame1.png");
	background->GetTexturedRectangle().SetPosition(0, 0);
	background->GetTexturedRectangle().SetDimension(SCREEN_WIDTH, SCREEN_HEIGHT);

	//Initialize bird
	flappyBird = new Bird(app->GetRenderer(), "asset/image/flappy-bird.png");
	flappyBird->SetDefaultStatus(DEFAULT_BIRD_X, GROUND_Y / 2, 0, 0, 0);
	flappyBird->GetTexturedRectangle().SetDimension(72, 61);
	flappyBird->SetDefaultFrame(0, 0, BIRD_WIDTH / numberOfFrame, BIRD_HEIGHT);



	//Initialize ground (base)
	ground = new Ground(app->GetRenderer(), "asset/image/base.png");
	ground->GetTexturedRectangle().SetDimension(SCREEN_WIDTH, SCREEN_HEIGHT / 6);
	ground->GetTexturedRectangle().SetPosition(0, GROUND_Y);

	//Initialize pipes
	for (int i = 0; i < TOTAL_PIPE; i++) {
		pipe[i] = new Pipe(app->GetRenderer(), "asset/image/pipe-green.png", 104, 640);
		pipe[i]->SetStatus(SCREEN_WIDTH + i * SCREEN_WIDTH / (TOTAL_PIPE - 1), MAX_PIPE_Y, MIN_PIPE_Y, PIPE_DISTANCE);
		pipe[i]->SetMode();
	}

	//Initialize white texture for screen flickering
	flash = new GameObject(app->GetRenderer(), "asset/image/white.png");
	flash->GetTexturedRectangle().SetPosition(-50, -50);
	flash->GetTexturedRectangle().SetDimension(SCREEN_WIDTH + 300, SCREEN_HEIGHT + 300);

	//Initialize gameplay score text
	score = new Text(app->GetRenderer(), "asset/font/Flappy-Bird.ttf", "Score: 0", 112, { 254,0,0 });
	score->SetSize(SCORE_X, SCORE_Y, SCORE_WIDTH_CHAR * 8, SCORE_HEIGHT_CHAR);

	//Initialize high score text
	int tmphScore;
	fin.read((char*)&tmphScore, sizeof(int));
	hScore = to_string(tmphScore);
	highScore = new Text(app->GetRenderer(), "asset/font/Flappy-Bird.ttf", "High score: " + hScore, 112, { 254,0,0 });
	highScore->SetSize(SCORE_X, SCORE_Y + SCORE_HEIGHT_CHAR + 2, SCORE_WIDTH_CHAR * (12 + hScore.length()), SCORE_HEIGHT_CHAR);

	//Initialize sound
	soundFly = new SoundEffect("asset/sound/fly.wav");
	soundGetPoint = new SoundEffect("asset/sound/get-point.wav");
	soundDie = new SoundEffect("asset/sound/die.wav");
	soundHit = new SoundEffect("asset/sound/hit.wav");

	bgMusic = new Music("asset/sound/cute-mood-151368.wav");
	bgMusic->SetVolume(128);

	logo = new TexturedRectangle(app->GetRenderer(), "asset/image/logo.png");
	logo->SetPosition(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 8);
	logo->SetDimension(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 5);

	startGame = new Button(app->GetRenderer(), "asset/image/start.png", SCREEN_WIDTH / 2 - 75, 2 * SCREEN_HEIGHT / 3 + 15, 150, 75);

	nextLeft = new Button(app->GetRenderer(), "asset/image/nextLeft.png", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 10, 20, 25);

	nextRight = new Button(app->GetRenderer(), "asset/image/nextRight.png", SCREEN_WIDTH / 2 + 130, SCREEN_HEIGHT / 2 - 10, 20, 25);

	character = new Bird(app->GetRenderer(), "asset/image/flappy-bird.png");
	character->GetTexturedRectangle().SetDimension(108, 91);
	character->GetTexturedRectangle().SetPosition(SCREEN_WIDTH / 2 - 54, SCREEN_HEIGHT / 2 - 40);
	character->SetDefaultFrame(0, CharacterProperties[0].first, CharacterProperties[0].second, CharacterProperties[1].first);

	blackTransition = new TexturedRectangle(app->GetRenderer(), "asset/image/black.png");
	blackTransition->SetPosition(-50, -50);
	blackTransition->SetDimension(SCREEN_WIDTH + 300, SCREEN_HEIGHT + 300);

	gameOver = new TexturedRectangle(app->GetRenderer(), "asset/image/gameOver.png");
	gameOver->SetPosition(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 8);
	gameOver->SetDimension(SCREEN_WIDTH / 3, SCREEN_HEIGHT / 8);

	scoreBoard = new TexturedRectangle(app->GetRenderer(), "asset/image/scoreboard.png");
	scoreBoard->SetPosition(SCREEN_WIDTH / 3 - 30, SCREEN_HEIGHT / 3);
	scoreBoard->SetDimension(SCREEN_WIDTH / 3 + 60, SCREEN_HEIGHT / 3 - 30);

	gameOverScore = new Text(app->GetRenderer(), "asset/font/Flappy-Bird.ttf", "0", 200, { 34, 139, 34 });
	gameOverScore->SetSize(SCREEN_WIDTH / 2 + 175, SCREEN_HEIGHT / 2 - 65, SCORE_WIDTH_CHAR + 10, SCORE_HEIGHT_CHAR + 20);

	gameOverHighScore = new Text(app->GetRenderer(), "asset/font/Flappy-Bird.ttf", "0", 200, { 34, 139, 34 });
	gameOverHighScore->SetSize(SCREEN_WIDTH / 2 + 175, SCREEN_HEIGHT / 2 + 20, SCORE_WIDTH_CHAR + 10, SCORE_HEIGHT_CHAR + 20);

	replay = new Button(app->GetRenderer(), "asset/image/replay.png", SCREEN_WIDTH / 2 - 200, 2 * SCREEN_HEIGHT / 3 + 15, 150, 75);

	home = new Button(app->GetRenderer(), "asset/image/home.png", SCREEN_WIDTH / 2 + 60, 2 * SCREEN_HEIGHT / 3 + 15, 150, 75);

	newHighScore = new TexturedRectangle(app->GetRenderer(), "asset/image/newHighScore.png");
	newHighScore->SetPosition(SCREEN_WIDTH / 2 + 62, SCREEN_HEIGHT / 2 - 15);
	newHighScore->SetDimension(54, 24);

	newRecord = new SoundEffect("asset/sound/new-record.wav");

	setting = new Button(app->GetRenderer(), "asset/image/settingbutton.png", 10, 10, 40, 40);

	settingBoard = new TexturedRectangle(app->GetRenderer(), "asset/image/setting-board.png");
	settingBoard->SetPosition(60, 10);
	settingBoard->SetDimension(200, 100);

	soundOnOff = new Button(app->GetRenderer(), "asset/image/on-off.png", 170, 25, 80, 30);
	soundOnOff->SetClip(0, 0, 432, 171);

	musicOnOff = new Button(app->GetRenderer(), "asset/image/on-off.png", 170, 65, 80, 30);
	musicOnOff->SetClip(0, 0, 432, 171);

	//Handle events and rendering
	app->SetEventCallback(HandleEvents);
	app->SetRenderCallback(HandleRendering);

	//Game loop
	app->RunAppLoop();

	//Clean up our game
	delete background;
	delete flappyBird;
	delete ground;
	for (int i = 0; i < TOTAL_PIPE; i++) {
		delete pipe[i];
	}
	delete flash;
	delete score;
	delete soundDie;
	delete soundFly;
	delete soundGetPoint;
	delete soundHit;
	delete bgMusic;
	delete logo;
	delete startGame;
	delete nextLeft;
	delete nextRight;
	delete character;
	delete blackTransition;
	delete gameOver;
	delete scoreBoard;
	delete gameOverScore;
	delete gameOverHighScore;
	delete replay;
	delete home;
	delete newHighScore;
	delete newRecord;
	delete setting;
	delete app;

	return 0;
}
