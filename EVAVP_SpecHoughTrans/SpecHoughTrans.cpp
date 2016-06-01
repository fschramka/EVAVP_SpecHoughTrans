#include "SDL.h"
#include "SDL_syswm.h"
#include "CameraSimulator.h"
#include "PaperFinder.h"
#include "HoughTransform.h"
#include <chrono>
#include <iostream>

using namespace std;

static SDL_Window* gWindow = NULL;
static SDL_Renderer* gRenderer = NULL;
static CameraSimulator* gCam = NULL;
const static int gMaxIntensity = 255;


int initSDL(int width, int height){
	// initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
		cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
		return -1;
	}

	// create window 
	gWindow = SDL_CreateWindow("LivePaper",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_ALLOW_HIGHDPI);

	if(gWindow == NULL){
		cout << "window could not be created!" << SDL_GetError() << endl;
		return -1;
	}

	gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
	if( gRenderer == NULL ){
		cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << endl; 
		return -1;
	}

	 SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(gWindow, &wmInfo);

	SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
	SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "best");

	return 0;
}

void closeSDL(){

	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );

	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char** argv){
	chrono::high_resolution_clock::time_point t1, t2, t3, t4, t5, t6, t7;

	gCam = new CameraSimulator(1280, 720);
		
	//Get Cam Resolution
	int cols = gCam->getCameraResolutionX();
	int rows = gCam->getCameraResolutionY();

	if(initSDL(cols, rows) != 0){
		cout << "SDL Error!" << endl;
		return 0;
	}

	SDL_Event e;
	bool quit = false;

	// activate the hough room
	bool showHoughRoom = false;

	PaperFinder pf(cols, rows);

	SDL_Texture* texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, cols, rows);

	int count = 0;
	double times[6];
	memset(times, 0, sizeof(*times)*6);
	int rounds = 1000;
	int visPointSize = 8;

	SDL_Window* houghWindow = NULL;
	SDL_Renderer* houghRenderer = NULL;
	SDL_Texture* houghTexture = NULL;
	int houghheight = (int)(sqrt(cols*cols + rows*rows) * 2);
	HoughTransform ht(cols, rows);
	vector<pair<Point, Point>> houghPoints;
	vector<Point> corners;

	if(showHoughRoom){

		houghWindow = SDL_CreateWindow("Hough Visual",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			800, 800,
			SDL_WINDOW_ALLOW_HIGHDPI);

		if(houghWindow == NULL){
			cout << "Hough window could not be created!" << SDL_GetError() << endl;
			return -1;
		}

		houghRenderer = SDL_CreateRenderer(houghWindow, -1, SDL_RENDERER_ACCELERATED );
		if(houghRenderer == NULL ){
			cout << "Hough Renderer could not be created! SDL Error: " << SDL_GetError() << endl; 
			return -1;
		}

		houghTexture = SDL_CreateTexture(houghRenderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_STREAMING, 180, ht.getHoughHeight());
	}

	
	bool showGridLines = false;
	bool showhoughLines = false;


	while(!quit){ 
		
		// Handle Events
		while( SDL_PollEvent( &e ) != 0 ) {
			//User requests quit
			if( e.type == SDL_QUIT )
				quit = true;
			else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_g){
				showGridLines = !showGridLines;
			}
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_h) {
				showhoughLines = !showhoughLines;
			}
		}

		t1 = chrono::high_resolution_clock::now();
		unsigned char* image = gCam->getSingleImage(false, "dummy");
		t2 = chrono::high_resolution_clock::now();

		pf.setBuffer(image);
		vector<Point> reportedPoints = pf.findPaper(40,40);

		t3 = chrono::high_resolution_clock::now();

		// load image into texture
		void* vdst;
		int dummy;

		SDL_LockTexture(texture, NULL, &vdst, &dummy);

		ht.setInput(reportedPoints);
		houghPoints = ht.transform();

		vector<Point> cutPoints = pf.getRectangle(houghPoints);
		corners = pf.filterPoints(cutPoints);

		t4 = chrono::high_resolution_clock::now();

		unsigned char* dst = (unsigned char*)vdst;

		for(int i = 0; i<rows*cols; ++i){
			*dst++ = image[i];			//B
			*dst++ = image[i]; 			//G
			*dst++ = image[i];			//R
			*dst++ = 255;				//A
		}

		t5 = chrono::high_resolution_clock::now();
		SDL_UnlockTexture(texture);

		SDL_RenderCopy(gRenderer, texture, NULL, NULL);

		if(showHoughRoom){

			unsigned char* dataSpec = ht.getData();
			void* hvdst;
			int hdummy;

			SDL_LockTexture(houghTexture, NULL, &hvdst, &hdummy);

			unsigned char* hdst = (unsigned char*)hvdst;

			
			for(int i = 0; i< 180*ht.getHoughHeight(); i++){
				if (dataSpec[i] > 25) {
					*hdst++ = 0;			//B
					*hdst++ = 0; 			//G
					*hdst++ = 255; 			//R
					*hdst++ = 255;			//A
				}
				else {
					*hdst++ = 255 - (dataSpec[i] * 8 > 255 ? dataSpec[i] : dataSpec[i] * 8);			//B
					*hdst++ = 255 - (dataSpec[i] * 8 > 255 ? dataSpec[i] : dataSpec[i] * 8);			//G
					*hdst++ = 255 - (dataSpec[i] * 8 > 255 ? dataSpec[i] : dataSpec[i] * 8);			//R
					*hdst++ = 255;																		//A
				}
			}

			SDL_UnlockTexture(houghTexture);

			SDL_RenderCopy(houghRenderer, houghTexture, NULL, NULL);
			SDL_RenderPresent(houghRenderer);
		}

		t6 = chrono::high_resolution_clock::now();

		SDL_SetRenderDrawColor(gRenderer, 180, 180, 180, 255 );

		if (showhoughLines) {
			for (auto& p : houghPoints)
				SDL_RenderDrawLine(gRenderer, p.first.getX(), p.first.getY(), p.second.getX(), p.second.getY());
		}
		if(showGridLines){
			// visualize reported Points
			for(Point p: reportedPoints){
				SDL_Rect r;
				r.x=p.getX()-visPointSize/2;
				r.y=p.getY()-visPointSize/2;
				r.h = visPointSize;
				r.w = visPointSize;

				if(p.isVertical())
					SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255 );
				else
					SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255 );

				SDL_RenderFillRect(gRenderer, &r);
			}
		}

		SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255 );
		for(Point p: corners){
			SDL_Rect r;
			r.x=p.getX()-visPointSize/2;
			r.y=p.getY()-visPointSize/2;
			r.h = visPointSize;
			r.w = visPointSize;
	
			SDL_RenderFillRect(gRenderer, &r);
		}

		SDL_RenderPresent(gRenderer);

		t7 = chrono::high_resolution_clock::now();

		count++;

		chrono::duration<double, milli> time_span = chrono::duration_cast<chrono::duration<double, milli>>(t2 - t1);
		times[0] += time_span.count();
		time_span = chrono::duration_cast<chrono::duration<double, milli>>(t3 - t2);
		times[1] += time_span.count();
		time_span = chrono::duration_cast<chrono::duration<double, milli>>(t4 - t3);
		times[2] += time_span.count();
		time_span = chrono::duration_cast<chrono::duration<double, milli>>(t5 - t4);
		times[3] += time_span.count();
		time_span = chrono::duration_cast<chrono::duration<double, milli>>(t6 - t5);
		times[4] += time_span.count();
		time_span = chrono::duration_cast<chrono::duration<double, milli>>(t7 - t6);
		times[5] += time_span.count();
	}

	cout << "Take single Image " << times[0]/rounds << endl;
	cout << "Find Paper Algorithm" << times[1]/rounds << endl;
	cout << "Hough Transformation " << times[2]/rounds << endl;
	cout << "Copy to texture image " << times[3]/rounds << endl;
	cout << "Copy to texture houghSpace " << times[4]/rounds << endl;
	cout << "Render Points into image " << times[5]/rounds << endl;

	closeSDL();

	cout << "Done! Press Enter to exit..." << endl; 

	return 0;
}