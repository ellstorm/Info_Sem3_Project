// Game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Gosu/Gosu.hpp"
#include "Gosu/AutoLink.hpp"
#include "time.h"
#include "math.h"
#include "string.h"
#include "Shlobj.h"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

using namespace std;
using namespace Gosu;

class GameWindow : public Window
{
public:
	TCHAR szFolderPath[MAX_PATH];
	Bitmap bg_data;
	Image *bg_image;
	double radius = sqrt(pow(WINDOW_WIDTH, 2) + pow(WINDOW_HEIGHT, 2)) / 2.0; //2203
	double y_pos, x_pos;
	double t, alpha, scale;
	uint32_t k = 100;

	double speed = 20; //pixel per second
	double speed_integrator = 0;
	int8_t speed_mode = 0;
	double heading = 0;
	int8_t heading_mode = 0;


	GameWindow(uint16_t w = WINDOW_WIDTH, uint16_t h = WINDOW_HEIGHT, bool fullscreen = true) : Window(w, h, fullscreen)
	{
		set_caption("Gosu Game");
		
		SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_PROFILE, FALSE);
		wstring buf = szFolderPath;
		string bg_data_path(buf.begin(), buf.end());
		bg_data_path.append("/Source/Repos/graphics/zoom.earth-ber.png");
		load_image_file(bg_data, bg_data_path);
		bg_image = new Image(bg_data);
		y_pos = 4400;
		x_pos = 4400;
		t = 0; //ms
		scale = 1;
	}

	~GameWindow()
	{
		free(bg_image);
	}

	void update() override;

	void draw() override;

	void button_down(Button button) override;

	void button_up(Button button) override;
};

void GameWindow::update()
{
	if (speed_mode)
		speed_integrator += speed_mode * update_interval() / 1000.0;
	else
		speed_integrator += 0.0005 * update_interval() * ((speed_integrator < 0) - (speed_integrator > 0));

	if (speed_integrator > 3)
	{
		speed_integrator = 3;
		speed = 40;
	}
	else if (speed_integrator < -3)
	{
		speed_integrator = -3;
		speed = 10;
	}
	
	if (speed_integrator > 0)
		speed = 20 * (1 - 2.0 / 27.0 * pow(speed_integrator, 3) + 1.0 / 3.0 * pow(speed_integrator, 2));
	else
		speed = 20 / (1 - 2.0 / 27.0 * pow(-speed_integrator, 3) + 1.0 / 3.0 * pow(speed_integrator, 2));
	t += this->update_interval();

	alpha++;
	y_pos += offset_y(heading, speed * update_interval() / 1000.0);
	x_pos += offset_x(heading, speed * update_interval() / 1000.0);
}

void GameWindow::draw()
{
	bg_image->draw_rot(
		WINDOW_WIDTH / 2, 
		WINDOW_HEIGHT / 2, 
		0, 
		alpha, 
		x_pos / bg_image->width, 
		y_pos / bg_image->height,
		scale,
		scale);
}

void GameWindow::button_down(Button button)
{
	switch (button.id())
	{
	default:break;
	}
}

void GameWindow::button_up(Button button)
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

