#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>
#include <math.h>
#include <stdbool.h>

#define WIDTH 320
#define HEIGHT 240

SDL_Window *window;
SDL_Surface *surface;
Uint32 *pixels;

bool keys[SDL_NUM_SCANCODES];

Uint32 rgb(int r, int g, int b) {
	return (r << 16) | (g << 8) | b;
}

void pixel(int x, int y, Uint32 c) {
	if (x > WIDTH || y > HEIGHT) return;
	pixels[y * WIDTH + x] = c;
}

void vertical(int x, int h, Uint32 c) {
	for (int i = 0; i < h; i++) {
		pixel(x, HEIGHT/2 - h/2 + i, c);
	}
}

float px = WIDTH/2;
float py = HEIGHT/2;
float pR = 0;

#define BLOCK_SIZE 80

int level[] = {
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,0,1,1,1,0,1,
	1,0,0,0,0,1,0,1,
	1,1,0,0,0,0,0,1,
	1,0,0,1,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

bool coll(int x, int y) {
	if (level[y/BLOCK_SIZE * 8 + x/BLOCK_SIZE]) {
		return true;
	}
	return false;
}


#define RENDER_DIST 640

int castRay(int a) {
    double angle = pR - M_PI / 6.0 + (M_PI / 3.0 * a / WIDTH);
    double rX = px;
    double rY = py;
    int rayLen = 0;
    while (rayLen < RENDER_DIST) {
        rX += cos(angle);
        rY += sin(angle);
        if (coll((int) rX, (int) rY)) {
            return rayLen;
        }
        rayLen++;
    }
    return 0;
}
void rayCast() {
    int rL;
    int samplingFactor = 1;
    for(int i = 0; i < WIDTH / samplingFactor; i++) {
        rL = castRay(i * samplingFactor);
        if (rL != 0) {

            int lineHeight = HEIGHT / (0.02 * rL);
            
            lineHeight = (lineHeight > HEIGHT) ? HEIGHT : lineHeight;
            vertical(i * samplingFactor, lineHeight, rgb(0, 20, 200));
        }
    }
}


double spd = 16;
double rSpd = 0.2;
void movePlayer(double delta) {
    float newX = px;
    float newY = py;
    float newR = pR;

    if (keys[SDL_SCANCODE_LEFT]) {
        newR -= rSpd * delta;
    }
    if (keys[SDL_SCANCODE_RIGHT]) {
        newR += rSpd * delta;
    }

    if (keys[SDL_SCANCODE_W]) {
        newX += spd * cos(newR) * delta;
        newY += spd * sin(newR) * delta;
    }
    if (keys[SDL_SCANCODE_S]) {
        newX -= spd * cos(newR) * delta;
        newY -= spd * sin(newR) * delta;
    }
    if (keys[SDL_SCANCODE_A]) {
        newX += spd * cos(newR - M_PI / 2) * delta;
        newY += spd * sin(newR - M_PI / 2) * delta;
    }
    if (keys[SDL_SCANCODE_D]) {
        newX += spd * cos(newR + M_PI / 2) * delta;
        newY += spd * sin(newR + M_PI / 2) * delta;
    }

    if (!coll((int)newX, (int)py)) {
        px = newX;
    }
    if (!coll((int)px, (int)newY)) {
        py = newY;
    }

    pR = newR;
}


int main(int argc, char *argv[]) {
    SDL_VideoInit(NULL);
    window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    surface = SDL_GetWindowSurface(window);
    
    
    Uint32 startTime = SDL_GetTicks();
    Uint32 prevTime = startTime;
    double deltaTime = 0.0;
    double delta;
    
    bool quit = false;
    SDL_Event event;
    
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    keys[event.key.keysym.scancode] = true;
                    break;
                case SDL_KEYUP:
                    keys[event.key.keysym.scancode] = false;
                    break;
            }
        }
        
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - prevTime);
        prevTime = currentTime;
        
        delta = deltaTime/60;
        
        pixels = (Uint32*)surface->pixels;
        memset(pixels, 0, WIDTH * HEIGHT * sizeof(Uint32));
		
		movePlayer(delta);
		rayCast();
		
        SDL_UpdateWindowSurface(window);
    }
    
    

    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_VideoQuit();
    return 0;
}
