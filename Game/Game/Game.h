#pragma once

#include "Gosu/Gosu.hpp"
#include "Gosu/AutoLink.hpp"

using namespace std;
using namespace Gosu;

class Background;
class Clouds;
class Drone;
class Missile;
class Missile_Sprite;
class GameWindow;

class Background
{
public:
	Image *bg_image;
	double y_pos, x_pos;
	double scale = 1;
	double clouds_map_ratio;
	Clouds *clouds;
	Background(string bg_data_path, string cloud_data_path);
	~Background();
};

class Clouds
{
public:
	Image *cloud_image;
	double y_pos, x_pos;
	double scale = 1;

	Clouds(string cloud_data_path);
	~Clouds();
};


class Drone
{
public:
	uint16_t n = 0;
	uint8_t v = 0;

	Image **drone_array;
	Missile *missile_left = NULL;
	Missile *missile_right = NULL;
	uint16_t drone_offset;
	Missile_Sprite *missile_sprite = NULL;

	double speed = 20;
	double speed_integrator = 0;
	double heading = 0;
	double heading_integrator = 0;
	double rot_speed = 0;
	double scale = 1;

	Drone(string drone_data_path, string name_base, uint8_t w, uint16_t n, uint8_t v, string missile_data_path, string missile_name_base, string sprite_data_path);
	~Drone();

	double speed_eval(bool w_key, bool s_key, double update_interval);
	double heading_eval(bool a_key, bool d_key, double update_interval);
};

class Missile
{
public:
	Image *missile_image = NULL;
	bool missile_launch = FALSE;
	double y_pos, x_pos;
	double speed = 0;
	double missile_integrator = 0;
	const double speed_fac = 1;

	Missile(string missile_data_path, string name_base, string name_suffix);
	~Missile();

	double speed_eval();
};

class Missile_Sprite
{
public:
	Image *sprite_array[104] = { NULL };
	bool sprite_l = FALSE;
	bool sprite_r = FALSE;
	uint8_t sprite_cnt_l = 0;
	uint8_t sprite_cnt_r = 0;

	Missile_Sprite(string sprite_data_path);
	~Missile_Sprite();
};

class GameWindow : public Window
{
public:
	Background *background;
	Drone *drone;
	TCHAR szFolderPath[MAX_PATH];
	double t;

	//Input (evtl in eigene Klasse)
	bool w_key;
	bool s_key;
	bool a_key;
	bool d_key;

	GameWindow(uint16_t w, uint16_t h, bool fullscreen);

	~GameWindow();

	void update() override;

	void draw() override;

	void button_down(Button button) override;

	void button_up(Button button) override;

	void input_eval();
};