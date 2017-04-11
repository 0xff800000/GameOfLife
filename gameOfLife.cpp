//============================================================================
// Name        : Langton's.cpp
// Author      : D01000100
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <unistd.h>
#include <SDL2/SDL.h>
//#include <conio.h>

using namespace std;

//################################ Class GameOfLife #########################################################

struct Cell{bool isAlive;bool willSurvive;};
struct Point{int x; int y;};
typedef vector<Point> Form;
vector< Point > glider;

class GameOfLife{
public:
	GameOfLife(int,int,int,int);
	void step();
	void printWorld();
	bool getWorld(int,int);
	void setWorld(int posX,int posY,bool state){world[posX][posY].isAlive=state;};
	void killAll();
	void render();
	void modifyWorld(int,int,bool);
	void placeForm(int,int,Form);

private:
	// Grid dimensions
	int grid_Width;
	int grid_Height;
	// Window resolution
	int w_width;
	int w_height;
	int dx;
	int dy;
	int countNei(int,int);
	int generation;
	int currentPop;
	SDL_Renderer*renderer;
	vector< vector<Cell> > world;
};

GameOfLife::GameOfLife(int g_width=100,int g_height=100, int wi_width=500, int wi_height=500){
	grid_Width = g_width;
	grid_Height = g_height;
	w_width = wi_width;
	w_height = wi_height;
	// Window
	SDL_Window* window = SDL_CreateWindow
	(
		"Game of Life", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		w_width,
		w_height,
		SDL_WINDOW_SHOWN
	);

	renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

	//Black screen
	SDL_SetRenderDrawColor(renderer,0,0,0,0);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer,255,255,255,255);

	generation=0;
	currentPop=0;

	dx = w_width/grid_Width;
	dy = w_height/grid_Height;

	for(int w=0;w<g_width;w++){
		vector<Cell> row;
		for(int h=0;h<g_height;h++){
			row.push_back((Cell){false,false});
		}
		world.push_back(row);
	}
}

int GameOfLife::countNei(int xPos,int yPos){
	int nei=0;
	for(int y=0;y<3;y++){
		for(int x=0;x<3;x++){
			int curX=xPos-1+x,curY=yPos-1+y;
			if(world[((curX<0)?grid_Width-1:(curX>grid_Width-1)?0:curX)][((curY<0)?grid_Height-1:(curY>grid_Height-1)?0:curY)].isAlive)nei++;
		}
	}
	if(world[xPos][yPos].isAlive)nei--;
	return nei;
}

void GameOfLife::step(){
	//Détermination de la prochaine génération
	for(int y=0;y<grid_Height;y++){
		for(int x=0;x<grid_Width;x++){
			int count=countNei(x,y);
			if(count==3){
				world[x][y].willSurvive=true;
			}
			else if(count==2&&world[x][y].isAlive){
				world[x][y].willSurvive=true;
			}
			else{
				world[x][y].willSurvive=false;
			}
		}
	}

	//Met à jour la population
	currentPop=0;
	for(int y=0;y<grid_Height;y++){
		for(int x=0;x<grid_Width;x++){
			world[x][y].isAlive=world[x][y].willSurvive;
			if(world[x][y].isAlive)currentPop++;
		}
	}

	//Augmente l'indice de génération
	generation++;
	render();
}

void GameOfLife::printWorld(){
	cout<<"Langton's Ant : generation "<<generation<<endl;
	for(int y=0;y<grid_Height;y++){
		for(int x=0;x<grid_Width;x++){
			if(world[x][y].isAlive)cout<<"#";
			else cout<<" ";
		}
		cout<<endl;
	}
}

bool GameOfLife::getWorld(int x,int y){
	return world[x][y].isAlive;
}

void GameOfLife::killAll(){
	for(int h=0;h<grid_Height;h++){
		for(int w=0;w<grid_Width;w++){
			world[w][h].isAlive=false;
			world[w][h].willSurvive=false;
		}
	}
}

void GameOfLife::render(){
	int h = w_height;
	int w = w_width;
	// Draw world
	for(int y=0;y<grid_Height;y++){
		for(int x=0;x<grid_Width;x++){
			if(world[x][y].isAlive){
				SDL_SetRenderDrawColor(renderer,255,255,255,255);
			}
			else{
				SDL_SetRenderDrawColor(renderer,0,0,0,0);
			}
			SDL_Rect rect;
			rect.x=x*dx;
			rect.y=y*dy;
			rect.w=dx;
			rect.h=dy;
			SDL_RenderFillRect(renderer,&rect);
		}
	}

	// Draw grid
	SDL_SetRenderDrawColor(renderer,50,50,50,50);
	for(int y=0;y<grid_Height; y++){
		for(int x=0; x<grid_Width; x++){
			int x_line = x*dx;
			SDL_RenderDrawLine(renderer,x_line,0,x_line,w_height);
		}
	}
	for(int x=0; x<grid_Width; x++){
		for(int y=0;y<grid_Height; y++){
			int y_line = y*dy;
			SDL_RenderDrawLine(renderer,0,y_line,w_width,y_line);
		}
	}


	SDL_RenderPresent(renderer);
}

void GameOfLife::modifyWorld(int x, int y, bool state){
	int h = w_height;
	int w = w_width;
	setWorld(x*grid_Width/w,y*grid_Height/h,state);
	render();
}

void GameOfLife::placeForm(int x, int y, Form f){
	for(int i=0; i<f.size(); i++){
		setWorld(x+f[i].x,y+f[i].y,true);
	}
}

//#########################################################################################


void loop(int gridW,int gridH){
	GameOfLife ant(gridW,gridH,500,500);
	ant.step();
	ant.placeForm(0,0,glider);
	SDL_Event ev;
	bool autoStep=false;
	bool clicked=false;
	bool state=false;
	int delayVal=50000;
	int timer=0;
	for(;;){

		if(delayVal<0)delayVal=0;

		while(SDL_PollEvent(&ev)){
			switch(ev.type){
				case SDL_QUIT:{
					return;
				}
				case SDL_KEYDOWN:{
					switch(ev.key.keysym.sym){
						case SDLK_ESCAPE: return;
						case 'q':return;
						case 'w':{autoStep=!autoStep;break;}
						case 'e':{autoStep=false;ant.step();break;}
						case 'a':{delayVal=(delayVal-500000<0)?0:delayVal-500000;break;}
						case 's':{delayVal+=500000;break;}
						case 'x':{autoStep=false;ant.killAll();break;}
						default:{
							break;
						}
					}
					break;
				}
				case SDL_MOUSEBUTTONDOWN:{
					state=(ev.button.button==SDL_BUTTON_LEFT);
					int mouseX=ev.button.x;
					int mouseY=ev.button.y;
					ant.modifyWorld((int)mouseX,(int)mouseY,state);
					clicked=true;
					break;
				}
				case SDL_MOUSEBUTTONUP:{
					state=(ev.button.button==SDL_BUTTON_LEFT);
					clicked=false;
					break;
				}
				case SDL_MOUSEMOTION:{
					if(clicked){
						int mouseX=ev.motion.x;
						int mouseY=ev.motion.y;
						ant.modifyWorld((int)mouseX,(int)mouseY,state);
					}
					break;
				}
				default: break;
			}
		}
		if(autoStep)ant.step();
		usleep(delayVal);
	}
}

int main(int argc, char** argv) {
	struct Point a = {1,0};
	glider.push_back(a);
	a={2,1};
	glider.push_back(a);
	a={0,2};
	glider.push_back(a);
	a={1,2};
	glider.push_back(a);
	a={2,2};
	glider.push_back(a);

	int grid_Width=100,grid_Height=100;
	if(argc == 3){
		grid_Width = atoi(argv[1]);
		grid_Height = atoi(argv[2]);
	}
	else if(argc == 2){
		grid_Width = grid_Height = atoi(argv[1]);
	}

	if(grid_Width>500)grid_Width=500;
	if(grid_Height>500)grid_Height=500;

	loop(grid_Width,grid_Height);

	SDL_Quit();
	return 0;
}
