#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

typedef struct VideoMemory VideoMemory;

volatile sig_atomic_t stop = 0;

extern VideoMemory* VideoMemory_create();
extern void VideoMemory_destroy(VideoMemory* vm);
extern bool VideoMemory_initialize(VideoMemory* vm, const char* title);
extern void VideoMemory_clearMemory(VideoMemory* vm, uint8_t color);
extern void VideoMemory_setPixel(VideoMemory* vm, int x, int y, uint8_t color);
extern void VideoMemory_render(VideoMemory* vm);

typedef struct SnakeNode {
    int x;
    int y;
    struct SnakeNode* next;
} SnakeNode;

typedef struct {
    int x;
    int y;
} Food;

void free_snake(SnakeNode* snake);

void handle_signal(int signal) {
    stop = 1;
}

SnakeNode* create_snake(int x, int y, int length) {
    SnakeNode* head = malloc(sizeof(SnakeNode));
    if (!head) {
        return NULL;
    }

    head->x = x;
    head->y = y;
    head->next = NULL;

    SnakeNode* current = head;
    for (int i = 1; i < length; ++i) {
        SnakeNode* new_node = malloc(sizeof(SnakeNode));
        if (!new_node) {
            free_snake(head);
            return NULL;
        }
        new_node->x = x - i;
        new_node->y = y;
        new_node->next = NULL;

        current->next = new_node;
        current = new_node;
    }

    return head;
}

void free_snake(SnakeNode* snake) {
    SnakeNode* current = snake;
    while (current != NULL) {
        SnakeNode* next = current->next;
        free(current);
        current = next;
    }
}

void move_snake(SnakeNode* snake, int dx, int dy, int width, int height) {
    int prev_x = snake->x;
    int prev_y = snake->y;

    snake->x = (snake->x + dx + width) % width;
    snake->y = (snake->y + dy + height) % height;

    SnakeNode* current = snake->next;
    while (current != NULL) {
        int temp_x = current->x;
        int temp_y = current->y;
        current->x = prev_x;
        current->y = prev_y;
        prev_x = temp_x;
        prev_y = temp_y;
        current = current->next;
    }
}


bool is_collision(SnakeNode* snake, int width, int height) {
    if (snake->x < 0 || snake->x >= width || snake->y < 0 || snake->y >= height) {
        return true;
    }

    SnakeNode* current = snake->next;
    while (current != NULL) {
        if (current->x == snake->x && current->y == snake->y) {
            return true;
        }
        current = current->next;
    }

    return false;
}

Food generate_food(int width, int height, SnakeNode* snake) {
    Food food;
    bool collision;
    do {
        collision = false;
        food.x = rand() % width;
        food.y = rand() % height;

        SnakeNode* current = snake;
        while (current != NULL) {
            if (current->x == food.x && current->y == food.y) {
                collision = true;
                break;
            }
            current = current->next;
        }
    } while (collision);

    return food;
}

void grow_snake(SnakeNode** snake, int x, int y) {
    SnakeNode* new_node = malloc(sizeof(SnakeNode));
    if (new_node) {
        new_node->x = x;
        new_node->y = y;
        new_node->next = *snake;
        *snake = new_node;
    }
}

void render_snake(SnakeNode* snake, VideoMemory* vm, uint8_t color) {
    SnakeNode* current = snake;
    while (current != NULL) {
        VideoMemory_setPixel(vm, current->x, current->y, color);
        current = current->next;
    }
}

int main() {
    srand(time(NULL));
    signal(SIGINT, handle_signal);

    int width = 80;
    int height = 40;

    VideoMemory* vm = VideoMemory_create();
    if (!vm) {
        return -1;
    }

    if (!VideoMemory_initialize(vm, "Linked List Snake Game")) {
        VideoMemory_destroy(vm);
        return -1;
    }

    VideoMemory_clearMemory(vm, 0);

    SnakeNode* snake = create_snake(width / 2, height / 2, 4);
    if (!snake) {
        VideoMemory_destroy(vm);
        return -1;
    }

    Food food = generate_food(width, height, snake);

    int dx = 1, dy = 0;

    while (!stop) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                stop = 1;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP && dy == 0) {
                    dx = 0; dy = -1;
                } else if (event.key.keysym.sym == SDLK_DOWN && dy == 0) {
                    dx = 0; dy = 1;
                } else if (event.key.keysym.sym == SDLK_LEFT && dx == 0) {
                    dx = -1; dy = 0;
                } else if (event.key.keysym.sym == SDLK_RIGHT && dx == 0) {
                    dx = 1; dy = 0;
                }
            }
        }

        move_snake(snake, dx, dy, width, height);

        if (is_collision(snake, width, height)) {
            break;
        }

        if (snake->x == food.x && snake->y == food.y) {
            grow_snake(&snake, food.x, food.y);
            food = generate_food(width, height, snake);
        }

        VideoMemory_clearMemory(vm, 0);

        render_snake(snake, vm, 1);
        VideoMemory_setPixel(vm, food.x, food.y, 2);

        VideoMemory_render(vm);

        SDL_Delay(50);
    }

    free_snake(snake);
    VideoMemory_destroy(vm);
    return 0;
}


