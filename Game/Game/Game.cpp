// Game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "time.h"
#include "math.h"
#include "string.h"
#include "Shlobj.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>

//#include "drone.h"
#include "Game.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define s8 int8_t
#define s16 int16_t
#define s32 int32_t

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define BG_MID_X_POS 4400
#define BG_MID_Y_POS 4400

#define MAX_SPEED 40
#define MIN_SPEED 10
#define MID_SPEED 20
#define ROT_SPEED 30

#define HIDE_Z_POS 0
#define BG_Z_POS 1
#define NOISE_Z_POS 2
#define CAM_Z_POS 4
#define MISSILE_Z_POS 5
#define DRONE_Z_POS 10

#define RELOAD_T 5000

using namespace std;
using namespace Gosu;

Background::Background(string bg_data_path, string cam_data_path)
{
	bg_image = new Image(bg_data_path);
	cam_image = new Image(cam_data_path);

	objective0 = new Bitmap(50, 50, Color::RED); //Erstes Ziel
	obj_image = new Image(*objective0);
	obj_x_pos = 4400;
	obj_y_pos = 4400;

	cout << bg_data_path << endl;
	scale = 1;
	x_pos = BG_MID_X_POS;
	y_pos = BG_MID_Y_POS;
	z_pos = BG_Z_POS;
	cam_z_pos = CAM_Z_POS;
	indicator = FALSE;
}

Background::~Background()
{
	delete bg_image;
}

Drone::Drone(string drone_data_path, string name_base, uint8_t w, uint16_t n, uint8_t v, string missile_data_path, string missile_name_base, string sprite_data_path, string song_data_path) : n(n), v(v)
{
	//Drone
	stringstream filename;
	drone_array = new Image*[n * v];
	for (volatile int i = 0; i < (n * v); i++)
	{
		filename << drone_data_path << name_base << setfill('0') << setw(w) << i << ".png";
		drone_array[i] = new Image(filename.str());
		cout << filename.str() << endl;
		filename.str(std::string());
		filename.clear();
		//Bitmap buf(900, 600);
		//memcpy(buf.data(), u8_drone_array[i], 4 * 900 * 600);
		//drone_array[i] = new Image(buf);
	}

	for (int i = 0; i < (n * v); i++)
		drone_array[i]->height(); //first operation on element takes ages...no idea why

	//Missile		
	missile_left = new Missile(missile_data_path, missile_name_base, "l.png", song_data_path);
	missile_right = new Missile(missile_data_path, missile_name_base, "r.png", song_data_path);

	//Sprite
	missile_sprite = new Missile_Sprite(sprite_data_path);

	speed = MID_SPEED;
	speed_integrator = 0;
	heading = 0;
	heading_integrator = 0;
	rot_speed = 0;
	scale = 1;
	z_pos = DRONE_Z_POS;
}

Drone::~Drone()
{
	for (int i = 0; i < (n * v); i++)
	{
		delete drone_array[i];
	}
	delete[] drone_array;
	delete missile_left;
	delete missile_right;
	delete missile_sprite;
}

double Drone::speed_eval(bool w_key, bool s_key, double update_interval)
{
	if (w_key != s_key)
	{
		if (w_key)
			speed_integrator += update_interval / 1000.0;
		else
			speed_integrator -= update_interval / 1000.0;
	}
	else
		speed_integrator += 0.0005 * update_interval * ((speed_integrator < 0) - (speed_integrator > 0));

	if (speed_integrator > 3)
	{
		speed_integrator = 3;
		speed = MAX_SPEED;
	}
	else if (speed_integrator < -3)
	{
		speed_integrator = -3;
		speed = MIN_SPEED;
	}

	if (speed_integrator > 0)
		speed = MID_SPEED * (1 - 2.0 / 27.0 * pow(speed_integrator, 3) + 1.0 / 3.0 * pow(speed_integrator, 2));
	else
		speed = MID_SPEED / (1 - 2.0 / 27.0 * pow(-speed_integrator, 3) + 1.0 / 3.0 * pow(speed_integrator, 2));

	return speed;
}

double Drone::heading_eval(bool a_key, bool d_key, double update_interval)
{
	if (a_key != d_key)
	{
		if (a_key)
			heading_integrator += update_interval / 1000.0;
		else
			heading_integrator -= update_interval / 1000.0;
	}
	else
	{
		if (abs(heading_integrator) < 0.017) //rot_speed < 1
			heading_integrator = 0;
		heading_integrator += 0.0005 * update_interval * ((heading_integrator < 0) - (heading_integrator > 0));
	}

	if (heading_integrator > 1)
	{
		heading_integrator = 1;
		rot_speed = ROT_SPEED;
	}
	else if (heading_integrator < -1)
	{
		heading_integrator = -1;
		rot_speed = -ROT_SPEED;
	}

	if (heading_integrator > 0)
		rot_speed = -ROT_SPEED * pow(heading_integrator, 2) + 60 * heading_integrator;
	else
		rot_speed = ROT_SPEED * pow(heading_integrator, 2) + 60 * heading_integrator;

	heading += rot_speed * update_interval / 1000.0;
	return heading;
}

void Drone::update_missiles(double time) {
	if (missile_left->missile_launch) {
		missile_left->update(time);
	}
	if (missile_right->missile_launch) {
		missile_right->update(time);
	}
}

void Drone::draw_missiles() {
	if (missile_left->missile_launch)
		missile_left->draw();
	if (missile_right->missile_launch)
		missile_right->draw();
}

void Drone::setup() {
	missile_left->x_pos = WINDOW_WIDTH / 2 - 92;
	missile_left->y_pos = WINDOW_HEIGHT / 2 + 38;
	missile_right->x_pos = WINDOW_WIDTH / 2 + 74;
	missile_right->y_pos = WINDOW_HEIGHT / 2 + 38;
}

Missile::Missile(string missile_data_path, string name_base, string name_suffix, string song_data_path)
{
	//Missile
	stringstream filename;
	filename << missile_data_path << name_base << name_suffix;
	missile_image = new Image(filename.str());
	missile_sound = new Sample(song_data_path);

	missile_launch = FALSE;
	z_pos = MISSILE_Z_POS;
	speed = 0;
	missile_integrator = 0;
	speed_fac = 1;
	delay_timestamp = 0;
}

Missile::~Missile()
{
	delete missile_image;
}

void Missile::draw() {
	missile_image->draw(x_pos, y_pos, z_pos);
}

void Missile::update(double time) {
	this->speed_eval(time);
	this->y_pos -= this->speed;
}

double Missile::speed_eval(double time)
{
	if (missile_launch)
	{
		missile_integrator++;
		if (y_pos < -107)
		{
			missile_integrator = 0;
			if (!delay_timestamp) {
				delay_timestamp = time; // reload timer starten
			}
			if ((time - delay_timestamp) > RELOAD_T) { // 5000ms, dann nachladen
				missile_launch = FALSE;
				y_pos = WINDOW_HEIGHT / 2 + 38;
				delay_timestamp = 0;
			}
		}
		speed = speed_fac * missile_integrator;
	}

	return speed;
}

Missile_Sprite::Missile_Sprite(string sprite_data_path)
{
	stringstream filename;
	for (int i = 0; i <= 103; i++)
	{
		filename << sprite_data_path << i << ".png";
		sprite_array[i] = new Image(filename.str());
		filename.str(std::string());
		filename.clear();
	}

	sprite_l = FALSE;
	sprite_r = FALSE;
	sprite_cnt_l = 0;
	sprite_cnt_r = 0;
}

Missile_Sprite::~Missile_Sprite()
{
	for (int i = 0; i <= 103; i++)
		delete sprite_array[i];
}

GameWindow::GameWindow(uint16_t w = WINDOW_WIDTH, uint16_t h = WINDOW_HEIGHT, bool fullscreen = true) : Window(w, h, fullscreen)
{
	set_caption("Gosu Game");

	SHGetSpecialFolderPath(NULL, szFolderPath, CSIDL_PROFILE, FALSE);
	wstring buf(szFolderPath);
	string graphics_path(buf.begin(), buf.end());
	graphics_path.append("/Source/Repos/graphics");

	//Background
	stringstream bg_data_path;
	bg_data_path << graphics_path << "/zoom.earth-ber.jpg";
	stringstream cam_data_path;
	cam_data_path << graphics_path << "/camera/CRT_overlay.png";
	background = new Background(bg_data_path.str(), cam_data_path.str());

	//Drone
	//Missiles
	//Missile Sprites
	stringstream drone_data_path;
	drone_data_path << graphics_path << "/drone/resize/reduced_size/";
	stringstream missile_data_path;
	missile_data_path << graphics_path << "/drone/missiles/";
	stringstream sprite0_data_path;
	sprite0_data_path << graphics_path << "/rocket/edited/flame";
	stringstream song_data_path;
	song_data_path << graphics_path << "/missile_launch.wav";
	drone = new Drone(drone_data_path.str(), "render", 4, 121, 4, missile_data_path.str(), "missile_", sprite0_data_path.str(), song_data_path.str());
	drone->setup();

	song_data_path.str(std::string());
	song_data_path.clear();
	song_data_path << graphics_path << "/drone_loop.wav";
	drone_hum = new Song(song_data_path.str());

	stringstream cursor_data_path;
	cursor_data_path << graphics_path << "/cursor.png";
	cursor = new Image(cursor_data_path.str());

	drone_hum->play(TRUE);
	drone_hum->change_volume(1.0);

	background->y_pos = BG_MID_Y_POS;
	background->x_pos = BG_MID_X_POS;
	t = 0; //ms
	tick = 0;
	mode = 0;

	obj_handler = new Objective_Handler();
}

GameWindow::~GameWindow()
{
	delete background;
	delete drone;
}

void GameWindow::input_eval()
{
	w_key = input().down(KB_W);
	s_key = input().down(KB_S);
	a_key = input().down(KB_A);
	d_key = input().down(KB_D);
}

void GameWindow::update()
{
	input_eval();

	drone->speed_eval(w_key, s_key, update_interval());
	drone->heading_eval(a_key, d_key, update_interval());
	
	drone->drone_offset = 121 * (drone->missile_left->missile_launch | drone->missile_right->missile_launch << 1) + 60 - drone->heading_integrator * 60;
	background->y_pos += offset_y(-drone->heading, drone->speed * update_interval() / 1000.0);
	background->x_pos += offset_x(-drone->heading, drone->speed * update_interval() / 1000.0);

	drone->update_missiles(t);

	drone->missile_sprite->sprite_cnt_l += drone->missile_sprite->sprite_l * 1.75;
	drone->missile_sprite->sprite_cnt_r += drone->missile_sprite->sprite_r * 1.75;
	if (drone->missile_sprite->sprite_cnt_l > 103)
	{
		drone->missile_sprite->sprite_cnt_l = 0;
		drone->missile_sprite->sprite_l = FALSE;
	}
	if (drone->missile_sprite->sprite_cnt_r > 103)
	{
		drone->missile_sprite->sprite_cnt_r = 0;
		drone->missile_sprite->sprite_r = FALSE;
	}

	obj_handler->update();

	double dist = 0;
	dist = pow(corner_x, 2);
	dist += pow(corner_y, 2);
	dist = sqrt(dist) / 3;
	//dist = 100;
	double rot = atan2(corner_x, corner_y) / M_PI * 180;
	cout << rot << "	" << drone->heading << endl;
	rot -= drone->rot_speed * update_interval() / 1000.0;
	corner_x = dist * sin(rot / 180.0 * M_PI) * 3;
	corner_y = dist * cos(rot / 180.0 * M_PI) * 3;
	corner_y += drone->speed * update_interval() / 1000.0 * 3;

	if (mode == 1)
	{
		double mouse_x = input().mouse_x();
		double mouse_y = input().mouse_y();
	}

	t += this->update_interval();
	tick++;
}

void GameWindow::draw()
{
	background->bg_image->draw_rot(
		WINDOW_WIDTH / 2, 
		WINDOW_HEIGHT / 2 + 100,
		background->z_pos, 
		drone->heading, 
		background->x_pos / background->bg_image->width(), 
		background->y_pos / background->bg_image->height(),
		background->scale,
		background->scale);
	drone->drone_array[drone->drone_offset]->draw(WINDOW_WIDTH / 2 - 450, WINDOW_HEIGHT / 2 - 200, drone->z_pos);

	drone->draw_missiles();

	if (mode == 1)
	{
		if (!(tick % 120))
			background->indicator = FALSE;
		if (!((tick + 30) % 120))
			background->indicator = TRUE;
		if(background->indicator)
			background->cam_image->draw_mod(0, 0, background->cam_z_pos, 1, 1, Color(255, 160, 160), Color(0xFFFFFFFF), Color(200, 100, 100), Color(0xFFFFFFFF));
		else
			background->cam_image->draw(0, 0, background->cam_z_pos);

		
		background->obj_image->draw_rot(
			WINDOW_WIDTH / 2,
			WINDOW_HEIGHT / 2 + 100,
			background->z_pos,
			drone->heading,
			(0.5 + (background->x_pos - BG_MID_X_POS) / background->obj_image->width()),
			(0.5 + (background->y_pos - BG_MID_Y_POS) / background->obj_image->height()),
			background->scale,
			background->scale);
		cursor->draw(corner_x + 960 - 240, corner_y + 640 - 240, 3, 1, 1);
		//cout << corner_x << "	" << corner_y << endl;
	}

	//if (drone->missile_sprite->sprite_l)
		//sprite0_array[sprite0l_cnt]->draw(WINDOW_WIDTH / 2 - 186, 0, 2);
	//if (drone->missile_sprite->sprite_r)
		//sprite0_array[sprite0r_cnt]->draw(WINDOW_WIDTH / 2 - 20, 0, 3);

	obj_handler->draw();
}

void GameWindow::button_down(Button button)
{
	switch (button.id())
	{
	case KB_F: 
		if (!drone->heading_integrator && !mode)
		{
			if(!drone->missile_left->missile_launch)
				drone->missile_right->missile_sound->play();
			drone->missile_left->missile_launch = TRUE;
			drone->missile_sprite->sprite_l = TRUE;
		}
		break;
	case KB_G:
		if (!drone->heading_integrator && !mode)
		{
			if(!drone->missile_right->missile_launch)
				drone->missile_right->missile_sound->play();
			drone->missile_right->missile_launch = TRUE;
			drone->missile_sprite->sprite_r = TRUE;
		}
		break;
	case KB_E:
		mode++;
		mode &= 0x01;
		if (mode == 1)
		{
			background->scale = 3;
			drone->missile_left->z_pos = HIDE_Z_POS;
			drone->missile_right->z_pos = HIDE_Z_POS;
			drone->z_pos = HIDE_Z_POS;
			drone_hum->change_volume(0.5);
		}
		else
		{
			background->scale = 1;
			drone->missile_left->z_pos = MISSILE_Z_POS;
			drone->missile_right->z_pos = MISSILE_Z_POS;
			drone->z_pos = DRONE_Z_POS;
			drone_hum->change_volume(1.0);
		}
		break;
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

Objective::Objective(double x, double y, string t, string m):x_pos(x),y_pos(y),title(t),mission(m) {
	active = FALSE;
	cleared = FALSE;
}

Objective::~Objective() {

}

void Objective::draw() {
	if (cleared) {
		//Bild von Krater oder abgehakt oder so
	}
	else if (active) {
		//Zielmarkierung
	}
	else {
		//Pfeil oder Nadel wie bei google maps?
	}
}

bool Objective::update() {
	return cleared;
}

bool Objective::destroy() {
	//Animation für Zerstörung?!
	cleared = TRUE;
	return cleared;
}

bool Objective::operator== (const Objective& o1) {
	if (this->x_pos == o1.x_pos && this->y_pos == o1.y_pos) {
		return TRUE;
	}
	return FALSE;
}

Objective_Handler::Objective_Handler():selected_objective(Objective(0, 0, "","")) {
	add_objective(4000, 4000, "Erste Mission", "Zerstöre den feindlichen Truck im Mittelpunkt der Karte");
	selected_objective = objective_list.at(0);
}

void Objective_Handler::select_new_objective() {
	for (auto i = objective_list.begin(); i != objective_list.end(); i++) {
		if (!i->cleared) {
			selected_objective = *i;
		}
	}
}

void Objective_Handler::add_objective(double x, double y, string t, string m) {
	auto o = Objective(x, y, t, m);
	objective_list.push_back(o);
}

void Objective_Handler::update() {
	for (auto i = objective_list.begin(); i != objective_list.end(); i++) {
		if (i->update() && selected_objective == *i) {
			select_new_objective();
		}
	}
}

void Objective_Handler::draw() {
	for (auto i = objective_list.begin(); i != objective_list.end(); i++) {
		i->draw();
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