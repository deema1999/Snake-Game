
# include <Windows.h>
# include <string>
# include <vector>
# include <queue>
#include <stdlib.h>


/*
Welcome to the Snake game programming challenge. You are required to add code to the following three functions to implement the Snake game.
You can of course write any code that you need to outside of these functions as well.
The functions are:

Initialize()
OnTimer()
OnKeyDown()

The file shows you how to draw rectangles with specific colors and how to draw text if you need to.
You should not need anything else because we are looking to create a visually-simple Snake game, so flat squared should be good enough.

Good luck!
*/


// COLORREF is a Windows data type. It is used by the API to represent 32-bit colors.
// Create a COLORREF value using the RGB() macro:
// COLORREF myColor = RGB(red, green, blue);
// where red, green, and blue are integers from 0-255.

// Another Windows data type is RECT. It represents a rectangle. It has the members:
// top, left, right, bottom.


// Use this to fill a rectangle of the hDC with a specific color.
void FillRect(HDC hDC, const RECT* pRect, COLORREF color)
{
	COLORREF oldColor = SetBkColor(hDC, color);
	ExtTextOut(hDC, 0, 0, ETO_OPAQUE, pRect, TEXT(""), 0, 0);
	SetBkColor(hDC, oldColor);
}

// Use this to draw text if you need to.
void DrawTextLine(HWND hWnd, HDC hDC, const char* sText, RECT* prText, COLORREF clr);


int Random_Food_Position()
{

	return (rand()%24)*25;
	
}
namespace game
{
	struct Point
	{   
		int x; int y;
		Point(){};
		Point(int x,int y){x=x;y=y;};
		
	};

	unsigned int windowWidth = 0;
	unsigned int windowHeight = 0;
	std::vector<Point> snake;
	int InitialSize;
	RECT food;
	enum Directions {UP,DOWN,LEFT,RIGHT,STOP};
	Directions Dir;
	bool active = true;
	std::queue<Directions> Direction;
	// This is called when the application is launched.
	bool Initialize(HWND hWnd)
	{   
		RECT rClient;
		GetClientRect(hWnd, &rClient);
		windowWidth = rClient.right - rClient.left; // rClient.top and rClient.left are always 0.
		windowHeight = rClient.bottom - rClient.top;
		
		InitialSize = 4;
		Dir = LEFT;	
		snake.clear();
		snake.resize(InitialSize);

		for(int i=0;i<InitialSize;i++)
			snake[i].y = windowWidth/2;
	
		snake[0].x = windowHeight/2;
		for(int i=1;i<InitialSize;i++)
			snake[i].x = snake[i-1].x+25;

		RECT r ;
		for(int i =0;i<InitialSize;i++)
		{
			r.left = snake[i].x;
			r.top = snake[i].y;
			r.right = r.left+20;
			r.bottom = r.top+20;
		}
		// Random position for food
		food.left = Random_Food_Position();
		food.top = Random_Food_Position();
		food.right = food.left+20;
		food.bottom = food.top+20;

		active = true;

		OutputDebugStringA("My game has been initialized. This text should be shown in the 'output' window in VS");
		
		return true;
	}

	void Build_And_DrawSnake(HDC hDC)
	{
		RECT s;
		s.left = snake[0].x;
		s.top = snake[0].y;
		s.right = s.left+20;
		s.bottom = s.top+20;
		FillRect(hDC, &s, RGB(255, 255, 0));
		for(int i=1; i<snake.size(); i++)
		{
			s.left = snake[i].x;
			s.top = snake[i].y;
			s.right = s.left+20;
			s.bottom = s.top+20;
			FillRect(hDC, &s, RGB(0, 255, 255)); //Draw the snake
		} 
		
	}

	void Move(Directions Dir)
	{
		
		for(int i=InitialSize-1;i>0;i--)
		{
			snake[i].y = snake[i-1].y;
			snake[i].x = snake[i-1].x;
		}

		if(Dir==UP)
			snake[0].y-=25;
		else if(Dir==DOWN)
			snake[0].y+=25;
		else if(Dir==LEFT)
			snake[0].x-=25;
		else if(Dir==RIGHT)
			snake[0].x+=25;

		
	}

	void CheckGameOver(HWND hWnd)
	{
		// The snake hit the wall
		if(snake[0].x < 0 || snake[0].y < 0 || snake[0].x >= windowWidth || snake[0].y >= windowHeight)
		{
			active = false;
			MessageBox(hWnd, TEXT("You Died!!"), TEXT("Snake Game"), MB_OK | MB_ICONSTOP);
			Initialize(hWnd);
			
		}

		// The snake eat itself
		for(int i=1;i<snake.size();i++)
		{
			if(snake[0].x == snake[i].x && snake[0].y == snake[i].y)
			{
				 active = false;
			     MessageBox(hWnd, TEXT("snake Game"), TEXT("you Died!!"), MB_OK | MB_ICONSTOP);
				 Initialize(hWnd);
				 
			}
		}
	}
	void EatFood()
	{
		
		if(snake[0].x == food.left && snake[0].y == food.top)
			{
				// Generate a new random position for food
				food.left = Random_Food_Position();
				food.top = Random_Food_Position();
				food.right = food.left+20;
				food.bottom = food.top+20;

			    InitialSize++;	
				snake.resize(InitialSize);
				snake.push_back(Point(snake[InitialSize-1].x,snake[InitialSize-1].y));
			}
		
	}
	
	// This is called periodically. Use it to update your game state and draw to the window.
	void CALLBACK OnTimer(HWND hWnd, UINT Msg, UINT_PTR idTimer, DWORD dwTime)
	{
		
		if (!active)
			return;
		
		HDC hDC = GetDC(hWnd);
		RECT rClient;
		GetClientRect(hWnd, &rClient);
		FillRect(hDC, &rClient, RGB(0, 0, 0)); // Clear the window to blackness.
		char text[256] = { 0 };
		sprintf_s(text, "Snake Game");
		RECT rText = { 0, 0, rClient.right, 15 };
		DrawTextLine(hWnd, hDC, text, &rText, RGB(120, 120, 120));

		Direction.push(Dir);
		FillRect(hDC, &food, RGB(255, 0, 0));
		Build_And_DrawSnake(hDC);
		Move(Direction.front());
		Direction.pop();
		EatFood();
		CheckGameOver(hWnd);

		/*if( snake.size() != 4 )
			SetTimer(hWnd, 10010, 85 ,game::OnTimer);*/
		
	}
	// This is called when the user presses a key on the keyboard.
	// Use this to change the direction of your snake.
	
	bool OnKeyDown(WPARAM keyCode)
	{
		
		switch(keyCode)
		{
		case VK_UP:
			{
				Dir = UP;
				break;
			}
		case VK_DOWN:
			{
				
				Dir = DOWN;
				break;
			}
		case  VK_LEFT:
			{
				
				Dir = LEFT;
				break;
			}
		case VK_RIGHT:
			{
			
				Dir = RIGHT;
				break;
			}
		case VK_SPACE:
			{
		
				active = !active;
				break;
			}
		default:
			return false;
			
		}
		
		return true; // They key pressed does not interest us. Let the OS handle it.
	}
}
