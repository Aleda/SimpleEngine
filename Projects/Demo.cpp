#include "Demo.h"
#include "../Engine.h"
#include "Texture.h"
#include "InputManager.h"
#include "../SpriteRenderer.h"
#include "../ThirdParty/NetEase/Sprite2.h"
#include "../ThirdParty/NetEase/WDF.h"
#include "../ThirdParty/NetEase/WAS.h"
#include "GameMap.h"
#include "../Environment.h"
#include "../ResourceManager.h"
#include "Config.h"

void ProcessInput();
void toggleActorState();

// Game-related State data
SpriteRenderer  *Renderer;
std::vector<std::vector<Texture*>> mSpriteTextures;

std::vector<Sprite2> mSprite2;

#define PI 3.1415926
#define DegreeToRadian(d) (d*PI/180.0f)
#define RadianToDegree(r) (r*180.0f/PI)


int cur_frame = 0;
int frame_count = 0;
int dir_count = 0;
GLfloat delta = 0;

int dir = 0;
//右下，左下，左上，右上，下，左，上，右
int dirs[8] = { 0,1,2,3,4,5,6,7 };

//↑ → ↓ ← ↗ ↘ ↙ ↖

double src_x , src_y ;
double cur_x, cur_y;
double dest_x = src_x, dest_y= src_y;
GameMap *mGameMap;
std::list<Pos> mMoveList;
double step_range_x = 0;
double step_range_y = 0;
double move_velocity = 500;
Pos dest;

bool bmove = true;

float ScreenWidth =800.0f;
float ScreenHeight =600.0f;
Texture* p_Texture2;

int actor_state = 1;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		bmove = false;
		mMoveList.clear();
		int mapOffsetX = 320 + mSprite2[actor_state].mWidth / 2 - cur_x;
		int mapOffsetY = 240 + mSprite2[actor_state].mHeight / 2 - cur_y;
		double mouse_x = InputManager::GetInstance()->GetMouseX();
		double mouse_y = InputManager::GetInstance()->GetMouseY();
		mMoveList = mGameMap->Move(cur_x/20, cur_y/20, (-mapOffsetX+mouse_x)/20,
			(-mapOffsetY + mouse_y)/20);
		
		bmove=true;
		actor_state = 1;
		frame_count = mSprite2[actor_state].mFrameSize;
		cur_frame = 0;

	}
}

Demo::Demo()
{

	InputManager::GetInstance()->SetMouseButtonCallback(mouse_button_callback);

	std::string vPath =  Environment::GetAbsPath("Shader/sprite.vs");
	std::string fPath =  Environment::GetAbsPath("Shader/sprite.frag");
	std::cout <<" vPath:" << vPath << std::endl;
	std::cout <<" fPath:" << fPath << std::endl;

	ResourceManager::LoadShader(vPath, fPath, "", "sprite");

  // Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, 800.0f,600.0f, 0.0f, -1.0f, 1.0f);
	Shader* shader = ResourceManager::GetShader("sprite");
	shader->Bind();

	glUniform1i(glGetUniformLocation(shader->GetProgramID(), "image"), 0);    
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgramID(), "projection"), 1, GL_FALSE, (GLfloat*) (&projection));

	Renderer = new SpriteRenderer(shader);

   	/*读取配置*/
	Config config(Environment::GetAbsPath("Projects/config.txt" ));
	config.Print();

	std::string path = config.GetMapPath(config.mDict["map"][0]);
	mGameMap = new GameMap(path);

	actor_state = 0;

	for(int i=0;i<2;i++)
	{
		int id=0;std::string s = config.mDict["shape.wdf"][i];sscanf(s.c_str(),"%x",&id);
		NetEase::WDF wdf(config.GetWdfPath("shape.wdf"));
		
		std::vector<Texture*> tSprit;
		mSprite2.push_back ( wdf.LoadSprite(id) );

		int totalSize = mSprite2[actor_state].mFrameSize*mSprite2[actor_state].mGroupSize;
		for (int i = 0; i< totalSize ; i++) {
			int gpos = i / mSprite2[actor_state].mFrameSize;
			int cpos = i%  mSprite2[actor_state].mFrameSize;

			tSprit.push_back(new Texture(mSprite2[actor_state].mWidth,mSprite2[actor_state].mHeight, true, (uint8*)&mSprite2[actor_state].mFrames[gpos][cpos].src[0] ) );
		}
		mSpriteTextures.push_back(tSprit);

		actor_state++;
	}

	

	for(int i=0;i<2;i++)
	{
		int id=0;std::string s = config.mDict["shape.wd3"][i];sscanf(s.c_str(),"%x",&id);
		NetEase::WDF wdf(config.GetWdfPath("shape.wd3"));

		mSprite2.push_back (wdf.LoadSprite(id) );
		// mSprite2[actor_state] = wdf.LoadSprite(id);
		std::vector<Texture*> tSprit;
		int totalSize = mSprite2[actor_state].mFrameSize*mSprite2[actor_state].mGroupSize;
		for (int i = 0; i< totalSize ; i++) {
			int gpos = i / mSprite2[actor_state].mFrameSize;
			int cpos = i%  mSprite2[actor_state].mFrameSize;

			tSprit.push_back( new Texture(mSprite2[actor_state].mWidth,mSprite2[actor_state].mHeight, true, (uint8*)&mSprite2[actor_state].mFrames[gpos][cpos].src[0] ) );
		}
		mSpriteTextures.push_back(tSprit);

		actor_state++;
	}
	
	actor_state = 1;
	dir_count = mSprite2[actor_state].mGroupSize;
	frame_count = mSprite2[actor_state].mFrameSize;


 // mMoveList = mGameMap->Move(252, 60, 38, 16);

	cur_x = 1150;
	cur_y = 220;

}
void toggleActorState()
{
	if(actor_state==1)
	{
		actor_state = 0;
	}
	else
	{
		actor_state=1;
	}
	dir_count = mSprite2[actor_state].mGroupSize;
	frame_count = mSprite2[actor_state].mFrameSize;
}

double Astar_GetDistance(double sx, double sy, double ex, double ey)
{
	double dx = sx - ex;
	double dy = sy - ey;
	return sqrt(pow(dx,2) + pow(dy,2));
}

double Astar_GetAngle(double sx, double sy, double ex, double ey)
{
	double dx = ex - sx;
	double dy = ey - sy;
	
	double angle;
	double degree;

	angle = atan(dy / dx);

	degree = RadianToDegree(angle);


	if (dx < 0 && dy >= 0) {
		degree += 180;
		if(fabs(dx) <=0.001f && dy >= 0)
		{
			degree = 270;
		}
	}
	else if (dx <=0 && dy<0) {
		degree += 180;

	}
	else if (dx > 0 && dy <0) {
		degree += 360;

	}
	else if (dx>=0 && dy>=0) {
		if(fabs(dx) <=0.001f && dy >= 0 && fabs(dy)<=0.001f)
		{
			degree = 270;
		}
	}
	return degree;
}

int Astar_GetDir(double degree) {
  //右下，左下，左上，右上，下，左，上，右
  // 0    1    2    3   4  5   6  7
  //  2 3 0 1       6 7   4 5
  //int innerDirs[8] = { 7,3,6,2,5,1,4,0 };
	int innerDirs[8] = { 7,0,4,1,5,2,6,3};
	if (360 - 22.5 <= degree || degree < 22.5) {
		return innerDirs[0];
	}
	else {
		for (int i = 1; i < 8; i++) {
			if (i * 45 - 22.5 <= degree && degree < i * 45 + 22.5) {
				return innerDirs[i];
			}
		}
		return innerDirs[0];
	}
}



Demo::~Demo()
{

}


int cnt = 0;
GLfloat delta2=0;
bool draw_cell = false;
void Demo::Update()
{
	double dt = Engine::GetInstance()->GetDeltaTime();
	delta += dt;
	delta2 += dt;
	draw_cell = false;
	if (delta2 >= 4 * dt) {
		delta2 = 0;
		draw_cell = true;
		cur_frame++;
		if (cur_frame >= frame_count) {
			cur_frame = 0;

		}
	}
	if (delta >= dt) {
		cnt++;
		delta = 0;
		if (bmove) {
			if (!mMoveList.empty())
			{
				double local_velocity = move_velocity*dt;
				Pos d = mMoveList.front();
				dest.x = d.x * 20 + 10;
				dest.y = d.y * 20 + 10;

				if (Astar_GetDistance(cur_x, cur_y, dest.x, dest.y) > local_velocity ) {
					double degree = Astar_GetAngle(cur_x, cur_y, dest.x, dest.y);

					dir = Astar_GetDir(degree);

					step_range_x = cos(DegreeToRadian(degree));
					step_range_y = sin(DegreeToRadian(degree));

					cur_x += step_range_x * local_velocity;  
					cur_y += step_range_y * local_velocity;

				}
				else {
					Pos d = mMoveList.front();
					cur_x = d.x * 20 + 10;
					cur_y = d.y * 20 + 10;
					mMoveList.pop_front();
				}

			}
			else {
				bmove = false;
				actor_state=0;
				frame_count = mSprite2[actor_state].mFrameSize;
				cur_frame = 0;
				
			}
		}
		else {

		}
	}

	ProcessInput();

}

void ProcessInput()
{

	if(InputManager::GetInstance()->IsKeyDown(GLFW_KEY_W))
	{
		cur_y -= 3;
	}


	if(InputManager::GetInstance()->IsKeyDown(GLFW_KEY_A))
	{
		cur_x -= 3;
	}


	if(InputManager::GetInstance()->IsKeyDown(GLFW_KEY_S))
	{
		cur_y += 3;
	}


	if(InputManager::GetInstance()->IsKeyDown(GLFW_KEY_D))
	{
		cur_x += 3;
	}

}

void Demo::Draw()
{
	int mapOffsetX = 320 + mSprite2[actor_state].mWidth / 2 - cur_x;
	int mapOffsetY =  240 + mSprite2[actor_state].mHeight / 2 - cur_y;
	mGameMap->Draw(Renderer,mapOffsetX,mapOffsetY);

	int px =ScreenWidth/2 - mSprite2[actor_state].mWidth/2 ;
	int py =ScreenHeight/2 - mSprite2[actor_state].mHeight/2;


	int gpos = cur_frame / frame_count;
	int cpos = cur_frame%frame_count;
	Sprite2::Sequence& frame1 = mSprite2[actor_state].mFrames[gpos][cpos];
	Sprite2::Sequence& frame2 = mSprite2[actor_state+2].mFrames[gpos][cpos];

	Renderer->DrawSprite(mSpriteTextures[actor_state][cur_frame + dir*frame_count],
		glm::vec2(px, py),
		glm::vec2(mSprite2[actor_state].mWidth, mSprite2[actor_state].mHeight), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));

	int px2 = px - (mSprite2[actor_state+2].mKeyX - mSprite2[actor_state].mKeyX);  
	int py2 = py - (mSprite2[actor_state+2].mKeyY - mSprite2[actor_state].mKeyY);

	Renderer->DrawSprite(mSpriteTextures[actor_state+2][cur_frame + dir*frame_count],
		glm::vec2(px2, py2),
		glm::vec2(mSprite2[actor_state+2].mWidth, mSprite2[actor_state+2].mHeight),
		0.0f, 
		glm::vec3(1.0f, 1.0f, 1.0f));

	mapOffsetX = 320 + mSprite2[actor_state].mWidth / 2 - cur_x;
	mapOffsetY =  240 + mSprite2[actor_state].mHeight / 2 - cur_y;
	mGameMap->DrawMask(Renderer, mapOffsetX, mapOffsetY);

  //mGameMap->DrawCell(Renderer, mapOffsetX, mapOffsetY);


}

