#include <iostream>
#include "raylib.h"

Color BgCircalColor = Color{ 13,126, 254, 156 };

int player_score = 0;
int enemy_score = 0;

class Ball {

public:
	float x, y; // starting position of ball
	float radius;
	int speed_x, speed_y;
	

	void draw() {
		DrawCircle(x, y, radius, WHITE);
	}

	void resetBall() {
		x = GetScreenWidth() / 2;
		y = GetScreenHeight() / 2;

		int choose_direction[2] = { -1,1 };
		speed_x *= choose_direction[GetRandomValue(0, 1)];
		speed_y *= choose_direction[GetRandomValue(0, 1)];
	}

	void playBallDrop(Sound ball_drop_sound){
		PlaySound(ball_drop_sound);
	}

	void update(Sound ball_drop_sound, Sound gain_point_sound, Sound lose_point_sound) {
		x += speed_x;
		y += speed_y;

		if (y + radius >= GetScreenHeight() || y - radius <= 0) {
			speed_y *= -1;
			playBallDrop(ball_drop_sound);
		}
		if (x + radius >= GetScreenWidth()) {
			player_score++;
			PlaySound(gain_point_sound);
			resetBall();
		}
		if (x - radius <= 0) {
			enemy_score++;
			PlaySound(lose_point_sound);
			resetBall();
		}
	}
};

class Paddle {

protected:
	void LimitMovement() {
		if (y <= 0)
			y = 0;
		if (y + height >= GetScreenHeight())
			y = GetScreenHeight() - height;
	}

public:
	float x, y;
	float height, width;
	int speed, margin;

	void draw() {
		DrawRectangleRounded(Rectangle{ x , y, width, height }, .8, 0, WHITE);
	}

	void update() {
		if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) 
			y -= speed;
		if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) 
			y += speed;

		LimitMovement();
	}

};

class EnemyPaddle: public Paddle {

public:
	void update(int ball_y) {
		if (y + height/2 > ball_y)
			y = y - speed;
		if (y + height / 2 <= ball_y)
			y = y + speed;

		LimitMovement();
	}
};

Ball* ball = new Ball();
Paddle* player = new Paddle();
EnemyPaddle* enemyPlayer = new EnemyPaddle();

int main()
{
	const int screen_width = 1200;
	const int screen_height = 800;
	const char* title = "Pong Ball";

	InitWindow(screen_width, screen_height, title);
	InitAudioDevice();

	Image Icon = LoadImage("icon.png");
	
	Sound ball_drop_sound = LoadSound("Audio/ball_drop_sound.wav"); // load ball drop sound file
	Sound ball_hit_sound = LoadSound("Audio/ball_hit_sound.wav");
	Sound gain_point_sound = LoadSound("Audio/gian_point_sound.wav");
	Sound lose_point_sound = LoadSound("Audio/lose_point_sound.wav");
	
	SetTargetFPS(60);
	SetWindowTitle("Unlimited Pong Game");
	SetWindowIcon(Icon);

	ball->x = screen_width / 2;
	ball->y = screen_height / 2;
	ball->radius = 16;
	ball->speed_x = 7;
	ball->speed_y = 7;

	player->height = 120;
	player->width = 25;
	player->margin = 10;
	player->x = player->margin;
	player->y = screen_height / 2 - player->height / 2;
	player->speed = 6;

	enemyPlayer->height = 120;
	enemyPlayer->width = 25;
	enemyPlayer->margin = 10;
	enemyPlayer->x = screen_width - enemyPlayer->width - enemyPlayer->margin;
	enemyPlayer->y = screen_height / 2 - enemyPlayer->height / 2;
	enemyPlayer->speed = 6;

	while (!WindowShouldClose()) {

		BeginDrawing();

		// updating
		ball->update(ball_drop_sound, gain_point_sound, lose_point_sound);
		player->update();
		enemyPlayer->update(ball->y);

		// checking for collisions
		if (CheckCollisionCircleRec(Vector2{ ball->x, ball->y }, ball->radius, Rectangle{ player->x, player->y, player->width, player->height }))
		{
			ball->speed_x *= -1;
			PlaySound(ball_hit_sound);
		}
		if (CheckCollisionCircleRec(Vector2{ ball->x, ball->y }, ball->radius, Rectangle{ enemyPlayer->x, enemyPlayer->y, enemyPlayer->width, enemyPlayer->height }))
		{
			ball->speed_x *= -1;
			PlaySound(ball_hit_sound);
		}
		

		//=========================== Start Drawring ============================

		// backgroun color
		ClearBackground(DARKBLUE);
		
		// Draw Circle
		DrawCircle(screen_width / 2, screen_height / 2, 150, BgCircalColor);
		
		// draw pong ball
		ball->draw();

		// draw left board
		player->draw();

		// draw right board
		enemyPlayer->draw();

		// draw player score
		DrawText(TextFormat("%i", player_score), GetScreenWidth() / 4 - 20, 20, 80, WHITE);
		// draw enemy score
		DrawText(TextFormat("%i", enemy_score), (GetScreenWidth() * 3) / 4 - 20, 20, 80, WHITE);

		// draw mid line
		DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);

		//=========================== End Drawring ============================


		EndDrawing();

	}

	UnloadSound(ball_drop_sound);
	UnloadSound(ball_hit_sound);
	UnloadSound(gain_point_sound);
	UnloadSound(lose_point_sound);
	UnloadImage(Icon);

	CloseAudioDevice();
	CloseWindow();
}