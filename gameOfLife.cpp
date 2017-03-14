//============================================================================
// Name        : Langton's.cpp
// Author      : D01000100
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
//#include <conio.h>

using namespace std;

//################################ Class GameOfLife #########################################################

struct Cell{bool isAlive;bool willSurvive;};

class GameOfLife{
public:
	GameOfLife(int,int);
	void step();
	void printWorld();
	bool getWorld(int,int);
	void setWorld(int posX,int posY,bool state){world[posX][posY].isAlive=state;};
	void killAll();
	int Width;
	int Height;
private:
	int countNei(int,int);
	int generation;
	int currentPop;
	vector< vector<Cell> > world;
};

GameOfLife::GameOfLife(int width,int height){
	generation=0;
	currentPop=0;

	this->Width=width;
	this->Height=height;

	for(int w=0;w<width;w++){
		vector<Cell> row;
		for(int h=0;h<height;h++){
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
			if(world[((curX<0)?Width-1:(curX>Width-1)?0:curX)][((curY<0)?Height-1:(curY>Height-1)?0:curY)].isAlive)nei++;
		}
	}
	if(world[xPos][yPos].isAlive)nei--;
	return nei;
}

void GameOfLife::step(){
	//Détermination de la prochaine génération
	for(int y=0;y<this->Height;y++){
		for(int x=0;x<this->Width;x++){
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
	for(int y=0;y<this->Height;y++){
		for(int x=0;x<this->Width;x++){
			world[x][y].isAlive=world[x][y].willSurvive;
			if(world[x][y].isAlive)currentPop++;
		}
	}

	//Augmente l'indice de génération
	generation++;
}

void GameOfLife::printWorld(){
	cout<<"Langton's Ant : generation "<<generation<<endl;
	for(int y=0;y<Height;y++){
		for(int x=0;x<Width;x++){
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
	for(int h=0;h<Height;h++){
		for(int w=0;w<Width;w++){
			world[w][h].isAlive=false;
			world[w][h].willSurvive=false;
		}
	}
}

//#########################################################################################

void drawWorld(SDL_Renderer*renderer,GameOfLife&ant,int w,int h){
	for(int y=0;y<ant.Height;y++){
		for(int x=0;x<ant.Width;x++){
			if(ant.getWorld(x,y)){
				SDL_SetRenderDrawColor(renderer,255,255,255,255);
			}
			else{
				SDL_SetRenderDrawColor(renderer,0,0,0,0);
			}
			SDL_Rect rect;
			rect.x=x*w/ant.Width;
			rect.y=y*h/ant.Height;
			rect.w=w/ant.Width;
			rect.h=h/ant.Height;
			SDL_RenderFillRect(renderer,&rect);
		}
	}

	SDL_RenderPresent(renderer);
}

void modifyWorld(SDL_Renderer*renderer,GameOfLife&ant,int w,int h,int mouseX,int mouseY,bool state){
	ant.setWorld(mouseX*ant.Width/w,mouseY*ant.Height/h,state);
	//cout<<"Mouse:"<<mouseX*ant.Width/w<<","<<mouseY*ant.Height/h<<endl;
	if(state){
		SDL_SetRenderDrawColor(renderer,255,255,255,255);
	}
	else{
		SDL_SetRenderDrawColor(renderer,0,0,0,0);
	}
	SDL_Rect rect;
	rect.x=(mouseX*ant.Width/w)*w/ant.Width;
	rect.y=(mouseY*ant.Height/h)*h/ant.Height;
	/*
	rect.x=mouseX*w/ant.Width;
	rect.y=mouseY*h/ant.Height;
	*/
	rect.w=w/ant.Width;
	rect.h=h/ant.Height;

	SDL_RenderFillRect(renderer,&rect);

	SDL_RenderPresent(renderer);
}

void loop(SDL_Renderer*renderer,int w,int h,int gridW,int gridH){
	GameOfLife ant(gridW,gridH);
	SDL_Event ev;
	bool autoStep=false;
	bool clicked=false;
	bool state=false;
	int delayVal=0;
	int timer=0;
    for(;;){

    	if(delayVal<0)delayVal=0;

    	if(autoStep&&(timer++>=delayVal)){
    		timer=0;
    		ant.step();
    		drawWorld(renderer,ant,w,h);
    		//SDL_Delay(delayVal);
    	}

    	while(SDL_PollEvent(&ev)){
    		switch(ev.type){
				case SDL_QUIT:{
					return;
				}
				case SDL_KEYDOWN:{
					switch(ev.key.keysym.sym){
						case SDLK_ESCAPE: return;
						//Camera rotation
						case 'q':return;
						case 'w':{autoStep=!autoStep;break;}
						case 'e':{autoStep=false;ant.step();drawWorld(renderer,ant,w,h);break;}
						case 'a':{delayVal=(delayVal-500000<0)?0:delayVal-500000;break;}
						case 's':{delayVal+=500000;break;}
						case 'x':{autoStep=false;ant.killAll();drawWorld(renderer,ant,w,h);break;}
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
					modifyWorld(renderer,ant,w,h,(int)mouseX,(int)mouseY,state);
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
						modifyWorld(renderer,ant,w,h,(int)mouseX,(int)mouseY,state);
					}
					break;
				}
				default: break;
    		}
    	}
    }
}

int main(int argc, char** argv) {
	//freopen("CON", "w", stdout);
	// Window
	const int width=500,height=500;
	SDL_Window* window = SDL_CreateWindow
	(
		"Game of Life", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width,
		height,
		SDL_WINDOW_SHOWN
	);

    SDL_Renderer*renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    //Black screen
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    int Width=100,Height=100;
    if(argc == 3){
    	Width = atoi(argv[1]);
    	Height = atoi(argv[2]);
    }
    else if(argc == 2){
    	Width = Height = atoi(argv[1]);
    }
    // cout<<"###### Game of Life ######"<<endl;
    // cout<<"Controls: q:quit, w:start/stop, e:step, x:kill all\na:slow down, s:speed up, mouse left:set, mouse right:reset"<<endl;
    // cout<<"Enter width (max:500):";cin>>Width;
    // cout<<"Enter height (max:500):";cin>>Height;

    if(Width>500)Width=500;
    if(Height>500)Height=500;

    loop(renderer,width,height,Width,Height);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
