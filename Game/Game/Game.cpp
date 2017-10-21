// Game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Gosu/Gosu.hpp"
#include "Gosu/AutoLink.hpp"
#include "time.h"
#include "math.h"
using namespace std;

class GameWindow : public Gosu::Window
{
public:
	const uint16_t window_width;
	const uint16_t window_height;
	int16_t y_pos, x_pos;
	double t, alpha, x_rot, y_rot;
	uint32_t offset[4] = { 0 };
	uint32_t k = 100;
	Gosu::Bitmap bg_data, bg;
	vector<Gosu::Image> bg_tiles;

	GameWindow(uint16_t w = 800, uint16_t h = 600) : Window(w, h), window_width(w), window_height(h)
	{
		set_caption("Gosu Tutorial Game");
		bg.resize(w, h);
		Gosu::load_image_file(bg_data, "C:/Users/Konstantin/Source/Repos/graphics/zoom.earth-ber.png");
		bg_tiles = Gosu::load_tiles(bg_data, 200, 200); //44 x 44
		y_pos = 4000;
		x_pos = 4000;
		t = 0;
	}

	void update() override;

	void draw() override;

	void button_down(Gosu::Button button) override;
};

void GameWindow::update()
{
	t += 10.0 / k;
	k++;
	//y_pos = 4000 + sin(t) * k; //y shift
	//x_pos = 4000 + cos(t) * k; //x shift
	offset[0] = y_pos / 200; //y min
	offset[1] = (y_pos + 600) / 200; //y max
	offset[2] = x_pos / 200; //x min
	offset[3] = (x_pos + 800) / 200; //x max
}

void GameWindow::draw()
{
	for (int i = offset[0]; i <= offset[1]; i++)
	{
		for (int j = offset[2]; j <= offset[3]; j++)
		{
			bg_tiles.at(i * 44 + j).draw_rot(j * 200 - x_pos, i * 200 - y_pos, 0, alpha, x_rot, y_rot);

			//printf("%d\n", i * 44 + j);
			//printf("%d\n", x + j * 200);
			//printf("%d\n", y + i * 200);
		}
	}
}

void GameWindow::button_down(Gosu::Button button)
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

