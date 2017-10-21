// Game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Gosu/Gosu.hpp"
#include "Gosu/AutoLink.hpp"
#include "time.h"
#include "math.h"
#include "string.h"
#include "Shlobj.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define s8 int8_t
#define s16 int16_t
#define s32 int32_t

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define TILE_WIDTH 200
#define TILE_HEIGHT 200
using namespace std;
using namespace Gosu;

class GameWindow : public Gosu::Window
{
	double mouseX;
	double mouseY;
	bool mleft;
	bool mright;
	bool w_key;
	bool s_key;
	u8 speed;

public:
	TCHAR szFolderPath[MAX_PATH];
	Bitmap bg_data;
	vector<Image> bg_tiles;
	uint16_t row_count;
	double radius = sqrt(pow(WINDOW_WIDTH, 2) + pow(WINDOW_HEIGHT, 2)) / 2.0; //2203
	double y_pos, x_pos;
	double t, alpha, scale;
	uint16_t offset[4] = { 0 };
	uint32_t k = 100;


	GameWindow(uint16_t w = WINDOW_WIDTH, uint16_t h = WINDOW_HEIGHT, bool fullscreen = true) : Window(w, h, fullscreen)
	{
		set_caption("Gosu Tutorial Game");
		
		SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_PROFILE, FALSE);
		wstring buf = szFolderPath;
		string bg_data_path(buf.begin(), buf.end());
		bg_data_path.append("/Source/Repos/graphics/zoom.earth-ber.png");
		load_image_file(bg_data, bg_data_path);
		bg_tiles = load_tiles(bg_data, TILE_WIDTH, TILE_HEIGHT); //44 x 44
		row_count = bg_data.width() / TILE_WIDTH;
		y_pos = 4000;
		x_pos = 4000;
		t = 0;
		scale = 1.5;
	}

	void update() override;

	void draw() override;

	void button_down(Button button) override;
};

void GameWindow::update()
{
	t += 10.0 / k;
	k++;
	alpha++;
	scale = 1.5 + sin(t) * 0.5;
	y_pos = 4000 + sin(t) * k; //y shift
	x_pos = 4000 + cos(t) * k; //x shift

	offset[0] = (y_pos - radius) / TILE_HEIGHT; //y min
	offset[1] = (y_pos + radius) / TILE_HEIGHT; //y max
	offset[2] = (x_pos - radius) / TILE_WIDTH; //x min
	offset[3] = (x_pos + radius) / TILE_WIDTH; //x max 

	mouseX = input().mouse_x();
	mouseY = input().mouse_y();
	mleft = input().down(MS_LEFT);
	mright = input().down(MS_RIGHT);
	w_key = input().down(KB_W);
	s_key = input().down(KB_S);

	if (!(w_key && s_key))
	{
		if (w_key)
			speed = 1;
		else if (s_key)
			speed = -1;
		else
			speed = 0;
	}
	
	



}

void GameWindow::draw()
{
	for (int i = offset[0]; i <= offset[1]; i++)
	{
		for (int j = offset[2]; j <= offset[3]; j++)
		{
			bg_tiles.at(i * (row_count) + j).draw_rot(
				WINDOW_WIDTH / 2, 
				WINDOW_HEIGHT / 2, 
				0, 
				alpha, 
				(x_pos - TILE_WIDTH * j) / double(TILE_WIDTH), 
				(y_pos - TILE_HEIGHT * i) / double(TILE_HEIGHT),
				scale,
				scale);
		}
	}
}

void GameWindow::button_down(Button button)
{
	switch (button.id())
	{
	default:break;
	}
}

int main()
{
	GameWindow window;
	window.show();
	//Gosu::Song song("C:/Users/Public/Music/Sample Music/Kalimba.mp3");
	//song.play();
	return 0;
}

