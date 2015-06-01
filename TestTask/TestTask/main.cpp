#include "SDL.h"
#include "SDL_thread.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <Windows.h>

int MainWindowHigh=320, MainWindowWidth=320;

enum RT{
	LEFT,
	RIGHT,
	NONE
};

class Car
{
private:
	int x,y,r;
	int RotateCenterX,RotateCenterY;
	RT RotateType;
	double angle,RotateDif;
public:
	Car()
	{
		this->x=0;
		this->y=0;
		this->angle=0;
		RotateType=NONE;
		RotateDif=-3.14/2;
	}

	void SetX(int x)
	{
		this->x=x;
		this->r=x-MainWindowWidth/2;
		this->RotateCenterX=MainWindowWidth/2;
	}

	void SetY(int y)
	{
		this->y=y;
	}

	void SetAngle(double angle)
	{
		this->angle=angle;
	}

	int GetX(void)
	{
		return this->x;
	}

	int GetY(void)
	{
		return this->y;
	}

	double GetAngle(void)
	{
		return this->angle;
	}

	void RotateLeft()
	{
		this->RotateCenterY=y;
		if (this->RotateType==RIGHT)
		{
			this->RotateDif=-this->RotateDif;
			if (x<x+cos(this->angle+RotateDif)*r)
				this->RotateCenterY+=r;
			else
				this->RotateCenterY-=r;
			this->angle=-this->angle;
		}
		this->RotateType=LEFT;

	}

	void RotateRight()
	{
		this->RotateCenterY=y;
		if (this->RotateType==LEFT)
		{
			if (x<x+cos(this->angle+RotateDif)*r)
				this->RotateCenterY+=r;
			else
				this->RotateCenterY-=r;
			this->RotateDif=-this->RotateDif;
			this->angle=-this->angle;
		}
		this->RotateType=RIGHT;
	}

	void Move(int r)
	{
		this->x=this->x+cos(this->angle)*r;
		this->y=this->y+sin(this->angle)*r;
	}

	bool GetDirection(void) // True - UP, False - DOWN
	{
		int Y=this->y+sin(angle+RotateDif)*20;
		if (y<Y)
			return false;
		else 
			return true;
	}

	void Draw(SDL_Renderer *Scene)
	{
		SDL_SetRenderDrawColor(Scene, 0, 0, 0, 0);
		angle+=RotateDif;
		SDL_RenderDrawLine(Scene, x, y, x+cos(angle)*20, y+sin(angle)*20);
		SDL_RenderDrawLine(Scene, x+cos(angle)*20, y+20*sin(angle), x+cos(angle-0.4)*10, y+sin(angle-0.4)*10);
		SDL_RenderDrawLine(Scene, x+cos(angle)*20, y+20*sin(angle), x+cos(angle+0.4)*10, y+sin(angle+0.4)*10);
		SDL_RenderDrawLine(Scene, x+cos(angle-1.57079632679)*5, y+sin(angle-1.57079632679)*5, x+cos(angle+1.57079632679)*5,y+sin(angle+1.57079632679)*5);

		angle+=-RotateDif;
		SDL_RenderPresent(Scene);
	}

	void ResetAngle(void)
	{
		if (GetDirection())
			this->angle=-3.14/2;
		else
			this->angle=3.14/2;
	}

	void Erase(SDL_Renderer *Scene)
	{
		SDL_SetRenderDrawColor(Scene, 255, 255, 255, 255);
		SDL_Rect r;
		r.x = this->x-this->r/2;
		r.y = this->y-this->r/2;
		r.w = this->r;
		r.h = this->r;

		SDL_RenderFillRect(Scene,&r);
		SDL_RenderPresent(Scene);
	}

	void Rotate90(SDL_Renderer *renderer)
	{
		if (RotateType==LEFT)
		for (int i=0;i<10;i++)
		{
			this->angle-=0.157;
			x=RotateCenterX+cos(this->angle)*r;
			y=RotateCenterY+sin(this->angle)*r;
			Erase(renderer);
			Draw(renderer);
			Sleep(75);
		}
		if (RotateType==RIGHT)
		for (int i=0;i<10;i++)
		{
			this->angle+=0.157;
			x=RotateCenterX+cos(this->angle)*r;
			y=RotateCenterY+sin(this->angle)*r;
			Erase(renderer);
			Draw(renderer);
			Sleep(75);
		}

		if (this->y<10)
		{
			Erase(renderer);
			Draw(renderer);
			this->y+=MainWindowHigh/4*3;
			this->RotateCenterY+=MainWindowHigh/4*3;
		}
		if (this->y>MainWindowHigh-10)
		{
			Erase(renderer);
			Draw(renderer);
			this->y-=MainWindowHigh/4*3;
			this->RotateCenterY-=MainWindowHigh/4*3;
		}

	}

	~Car()
	{
	}
};

class Road
{
private:
	int x,y;
public:
	Road()
	{
		this->x=MainWindowWidth/2;
		this->y=20;
	}

	void Draw (SDL_Renderer *Scene)
	{
		SDL_SetRenderDrawColor(Scene,255,255,255,255);
		SDL_RenderDrawLine(Scene,MainWindowWidth/2,0,MainWindowWidth/2,MainWindowHigh);
		SDL_SetRenderDrawColor(Scene, 0,0,0,0);

		SDL_RenderDrawLine(Scene,MainWindowWidth/5.5,0,MainWindowWidth/5.5,MainWindowHigh);
		SDL_RenderDrawLine(Scene,MainWindowWidth-MainWindowWidth/5.5,0,MainWindowWidth-MainWindowWidth/5.5,MainWindowHigh);

		SDL_RenderDrawLine(Scene,MainWindowWidth/2,0,x,y);
		SDL_RenderDrawLine(Scene,x,y+80,x,MainWindowHigh);
		SDL_RenderPresent(Scene);
	}

	void Move (bool Direction)
	{
		if (Direction)
		{
			this->y+=10;
			if (this->y>=MainWindowHigh)
			{
				this->y=-80;
			}
		}
		else
		{
			this->y-=10;
			if (this->y<=-80)
			{
				this->y=MainWindowHigh;
			}
		}
	}

	int GetCurrentY(void)
	{
		return this->y;
	}
};


class Actions
{
private:
	bool AFlag, BFlag, CFlag; //A-пересечение сплошной, B-движение по встречке, C - успешный разворот
public:
	Actions()
	{
		AFlag=false;
		BFlag=false;
		CFlag=false;
	}
	
	void ScanA(Car car, Road road)
	{
		if ((car.GetY()<road.GetCurrentY()-15)||(car.GetY()>road.GetCurrentY()+65))
			AFlag=true;
		else 
			AFlag=false;
	}

	void ScanBC(Car car, Road road)
	{
		if (((car.GetDirection()==false)&&(car.GetX()>MainWindowWidth/2+10))||
			((car.GetDirection()==true)&&(car.GetX()<MainWindowWidth/2-10)))
			BFlag=true;
		else 
			BFlag=false;

		if (((car.GetX()>MainWindowWidth/2)||(car.GetX()<MainWindowWidth/2))&&(!BFlag))
			CFlag=true;
	}
	
	void ActionOutput(void)
	{
		if ((AFlag)||(BFlag)||(CFlag))
			std::cout << std::endl << "--------------------------------" << std::endl;
		if (AFlag)
			std::cout << "Crossing of solid detected! " << std::endl;
		if (BFlag)
			std::cout << "Exit into the oncoming lane detected! " << std::endl;
		if ((!AFlag)&&(!BFlag)&&(CFlag))
			std::cout << "Rotation successful! " << std::endl;
	}

	~Actions()
	{
	}
};

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *window;
	window = SDL_CreateWindow("SDLwindow", 600, 300, MainWindowWidth, MainWindowHigh, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = NULL;
	renderer =  SDL_CreateRenderer( window, 0, SDL_RENDER_TARGETS_RESET);

	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	SDL_RenderClear( renderer );

	SDL_SetRenderDrawColor(renderer,0,0,0,0);
	SDL_RenderDrawLine(renderer,MainWindowWidth/12,MainWindowHigh-30,MainWindowWidth/12,MainWindowHigh-70);
	SDL_RenderDrawLine(renderer,MainWindowWidth/12,MainWindowHigh-30,MainWindowWidth/12-10,MainWindowHigh-50);
	SDL_RenderDrawLine(renderer,MainWindowWidth/12,MainWindowHigh-30,MainWindowWidth/12+10,MainWindowHigh-50);

	SDL_RenderDrawLine(renderer,MainWindowWidth/12*11,MainWindowHigh-30,MainWindowWidth/12*11,MainWindowHigh-70);
	SDL_RenderDrawLine(renderer,MainWindowWidth/12*11,MainWindowHigh-70,MainWindowWidth/12*11-10,MainWindowHigh-50);
	SDL_RenderDrawLine(renderer,MainWindowWidth/12*11,MainWindowHigh-70,MainWindowWidth/12*11+10,MainWindowHigh-50);

	Road road;
	road.Draw(renderer);

	Car object;
	object.SetX(MainWindowWidth/5*3);
	object.SetY(MainWindowHigh/5*3);
	object.SetAngle(0);
	object.Draw(renderer);

	Actions actions;

	SDL_Event event;
	int rl=0,rp=0;

	while (1) 
	{
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
			case SDL_KEYDOWN:
				{
					if (SDL_GetScancodeFromKey(event.key.keysym.sym)==SDL_SCANCODE_LEFT)
					{
						if (((object.GetX()>=MainWindowWidth/2-10)&&(object.GetDirection()==true))||
							((object.GetX()<=MainWindowWidth/2+10)&&(object.GetDirection()==false)))
						{
							bool AChecked=false;
							if ((object.GetX()<MainWindowWidth/2+10)&&(object.GetX()>MainWindowWidth/2-10))
							{
								actions.ScanA(object,road);
								AChecked=true;
							}
							if (rp==1)
							{
								rl=0;
								rp=0;
							}
							if (rl==0)
							{
								object.RotateLeft();
								rl++;
							}
							object.Rotate90(renderer);
							road.Draw(renderer);
							if (!AChecked)
								actions.ScanA(object,road);

							actions.ScanBC(object,road);
							actions.ActionOutput();
						}
					}
					if (SDL_GetScancodeFromKey(event.key.keysym.sym)==SDL_SCANCODE_RIGHT)
					{
						if (((object.GetX()>=MainWindowWidth/2-10)&&(object.GetDirection()==false))||
							((object.GetX()<=MainWindowWidth/2+10)&&(object.GetDirection()==true)))
						{
							bool AChecked=false;
							if ((object.GetX()<MainWindowWidth/2+10)&&(object.GetX()>MainWindowWidth/2-10))
							{
								actions.ScanA(object,road);
								AChecked=true;
							}
							actions.ScanA(object,road);
							if (rl==1)
							{
								rl=0;
								rp=0;
							}
							if (rp==0)
							{
								object.RotateRight();
								rp++;
							}
							object.Rotate90(renderer);
							road.Draw(renderer);
							if (!AChecked)
								actions.ScanA(object,road);
							actions.ScanBC(object,road);
							actions.ActionOutput();
						}
					}
					if (SDL_GetScancodeFromKey(event.key.keysym.sym)==SDL_SCANCODE_UP)
					{
						if ((object.GetX()<MainWindowWidth/2-10)||(object.GetX()>MainWindowWidth/2+10))
						{
							road.Draw(renderer);
							road.Move(object.GetDirection());
							Sleep(50);
						}
					}
					if (SDL_GetScancodeFromKey(event.key.keysym.sym)==SDL_SCANCODE_ESCAPE)
						return 0;
				}
				break;
			case SDL_QUIT:
				exit(0);
			}
		}
	}

	SDL_DestroyWindow(window); 
	SDL_Quit();

	return 0;
}