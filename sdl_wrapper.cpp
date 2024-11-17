#include <SDL2/SDL.h>
#include <cstring>

class VideoMemory {
public:
    static const int WIDTH = 80;
    static const int HEIGHT = 40;
    static const int SCALE = 10;

    VideoMemory() : window(nullptr), renderer(nullptr) {
        memset(buffer, 0, sizeof(buffer));
    }

    ~VideoMemory() {
        cleanup();
    }

    bool initialize(const char* title) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            return false;
        }
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_SHOWN);
        if (!window) {
            return false;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            return false;
        }
        return true;
    }

    void clearMemory(uint8_t color) {
        memset(buffer, color, sizeof(buffer));
    }

    void setPixel(int x, int y, uint8_t color) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            buffer[y][x] = color;
        }
    }

    void render() {
        if (!renderer) return;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);

        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                if (buffer[y][x]) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                }
                SDL_Rect rect = { x * SCALE, y * SCALE, SCALE, SCALE };
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);
    }

private:
    void cleanup() {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }

    SDL_Window* window;
    SDL_Renderer* renderer;
    uint8_t buffer[HEIGHT][WIDTH]; 
};

extern "C" {
    VideoMemory* VideoMemory_create() {
        return new VideoMemory();
    }

    void VideoMemory_destroy(VideoMemory* vm) {
        delete vm;
    }

    bool VideoMemory_initialize(VideoMemory* vm, const char* title) {
        return vm->initialize(title);
    }

    void VideoMemory_clearMemory(VideoMemory* vm, uint8_t color) {
        vm->clearMemory(color);
    }

    void VideoMemory_setPixel(VideoMemory* vm, int x, int y, uint8_t color) {
        vm->setPixel(x, y, color);
    }

    void VideoMemory_render(VideoMemory* vm) {
        vm->render();
    }
}

