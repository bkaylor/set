#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"

typedef enum Shape {
    SHAPE_DIAMOND,
    SHAPE_SQUIGGLE,
    SHAPE_OVAL
} Shape;

typedef enum Shading {
    SHADING_OPEN,
    SHADING_STRIPED,
    SHADING_SOLID
} Shading;

typedef enum Color {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_PURPLE
} Color;

typedef struct Card {
    Shape shape;
    Shading shading;
    Color color;
    int number;

    int id;
} Card;

typedef struct Deck {
    Card cards[100];
    int card_count;
} Deck;

typedef struct Set {
    int ids[3];
} Set;

typedef struct Vector_2 {
    int x, y;
} Vector_2;

typedef struct Game_State {
    Deck deck;
    Deck board;
    Vector_2 window; 

    int sets_in_this_board;
    int sets_found;

    bool quit;
    bool reset;
} Game_State;

char *get_shape_name(Shape shape)
{
    if (shape == SHAPE_DIAMOND) return "diamond";
    if (shape == SHAPE_SQUIGGLE) return "squiggle";
    if (shape == SHAPE_OVAL) return "oval";

    return "NO SHAPE";
}

char *get_shading_name(Shading shading)
{
    if (shading == SHADING_OPEN) return "open";
    if (shading == SHADING_STRIPED) return "striped";
    if (shading == SHADING_SOLID) return "solid";

    return "NO SHADING";
}

char *get_color_name(Shading color)
{
    if (color == COLOR_RED) return "red";
    if (color == COLOR_GREEN) return "green";
    if (color == COLOR_PURPLE) return "purple";

    return "NO COLOR";
}

void generate_deck(Deck *deck)
{
    for (int shape_count = 0; shape_count < 3; shape_count += 1)
    {
        for (int shading_count = 0; shading_count < 3; shading_count += 1)
        {
            for (int color_count = 0; color_count < 3; color_count += 1)
            {
                for (int number_count = 1; number_count <= 3; number_count += 1)
                {
                    deck->cards[deck->card_count].shape = shape_count;
                    deck->cards[deck->card_count].shading = shading_count;
                    deck->cards[deck->card_count].color = color_count;
                    deck->cards[deck->card_count].number = number_count;

                    deck->cards[deck->card_count].id = deck->card_count;

                    deck->card_count += 1;
                }
            }
        }
    }
}

void print_card(Card card)
{
    printf("%d: %d %s %s %s\n", card.id, card.number, get_shading_name(card.shading), get_color_name(card.color), get_shape_name(card.shape));
}

void print_deck(Deck deck)
{
    printf("\n");
    for (int i = 0; i < deck.card_count; i += 1)
    {
        Card card = deck.cards[i];
        print_card(card);
    }
    printf("\n");
}

bool is_set(Deck deck, int a, int b, int c)
{
    Card card_a = deck.cards[a], card_b = deck.cards[b], card_c = deck.cards[c];

    // Check number
    int matches = 0;
    if (card_a.number == card_b.number) matches += 1;
    if (card_b.number == card_c.number) matches += 1;
    if (card_a.number == card_c.number) matches += 1;
    if (matches == 1 || matches == 2) return false;

    // Check shading
    matches = 0;
    if (card_a.shading == card_b.shading) matches += 1;
    if (card_b.shading == card_c.shading) matches += 1;
    if (card_a.shading == card_c.shading) matches += 1;
    if (matches == 1 || matches == 2) return false;

    // Check color
    matches = 0;
    if (card_a.color == card_b.color) matches += 1;
    if (card_b.color == card_c.color) matches += 1;
    if (card_a.color == card_c.color) matches += 1;
    if (matches == 1 || matches == 2) return false;

    // Check shape
    matches = 0;
    if (card_a.shape == card_b.shape) matches += 1;
    if (card_b.shape == card_c.shape) matches += 1;
    if (card_a.shape == card_c.shape) matches += 1;
    if (matches == 1 || matches == 2) return false;

    return true;
}

int find_sets(Deck deck)
{
    int sets_count = 0;

    Set sets[1500]; // The full deck has 1080 so this should be fine.

    for (int i = 0; i < deck.card_count; i += 1)
    {
        for (int j = i; j < deck.card_count; j += 1)
        {
            for (int k = j; k < deck.card_count; k += 1)
            {
                if (i == j || i == k || j == k) continue;

                if (is_set(deck, i, j, k)) { 
                    printf("%d:", sets_count);
                    printf("\t");
                    print_card(deck.cards[i]);
                    printf("\t");
                    print_card(deck.cards[j]);
                    printf("\t");
                    print_card(deck.cards[k]);
                    printf("\n");

                    sets[sets_count].ids[0] = i;
                    sets[sets_count].ids[1] = j;
                    sets[sets_count].ids[2] = k;

                    sets_count += 1;
                }
            }
        }
    }

    printf("%d sets\n", sets_count);

    return sets_count;
}

void generate_random_set_from_deck(Deck source, Deck *destination, int size)
{
    /*
    int *ids = malloc(sizeof(int) * size);

    int *nth_available_card = malloc(sizeof(int) * size);

    bool *available = malloc(sizeof(bool) * source.card_count);
    for (int i = 0; i < size; i += 1) available[i] = true;

    for (int i = 0; i < size; i += 1)
    {
        nth_available_card[i] = rand() % (source.card_count - i);

        int id;
        int available_cards_left = nth_available_card[i];
        for (int j = 0; available_cards_left > 0; j += 1)
        {
            if (available[j]) {
                available_cards_left -= 1;
                id = j;
            }
        }

        ids[i] = id;
    }
    */

    int *ids = malloc(sizeof(int) * size);

    for (int i = 0; i < size; i += 1)
    {
        bool duplicate;
        int id_candidate;

        do {
            id_candidate = rand() % source.card_count;
            duplicate = false;

            for (int j = 0; j < size; j += 1)
            {
                if (id_candidate == ids[j]) {
                    duplicate = true;
                    break;
                }
            }
        } while (duplicate);

        ids[i] = id_candidate;
    }

    for (int i = 0; i < size; i += 1)
    {
        destination->cards[i] = source.cards[ids[i]];  
        destination->card_count += 1;
    }

}

void get_input(Game_State *game_state)
{
    int x, y;
    SDL_GetMouseState(&x, &y);

    // Handle events.
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        game_state->quit = true;
                        break;

                    case SDLK_r:
                        game_state->reset = true;
                        break;

                    default:
                        break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:

                if (event.button.button == SDL_BUTTON_LEFT ) {
                }

                if (event.button.button == SDL_BUTTON_RIGHT ) {
                }
                break;

            case SDL_QUIT:
                game_state->quit = true;
                break;

            default:
                break;
        }
    }
}

void draw_text(SDL_Renderer *renderer, int x, int y, char *string, TTF_Font *font, SDL_Color font_color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, string, font_color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int x_from_texture, y_from_texture;
    SDL_QueryTexture(texture, NULL, NULL, &x_from_texture, &y_from_texture);
    SDL_Rect rect = {x, y, x_from_texture, y_from_texture};

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void update(Game_State *game_state, float delta_t)
{
    if (game_state->reset) {
        generate_random_set_from_deck(game_state->deck, &game_state->board, 12);
        game_state->sets_in_this_board = find_sets(game_state->board);
        game_state->sets_found = 0;

        game_state->reset = false;
    }
}

void render(SDL_Renderer *renderer, Game_State game_state, TTF_Font *font, SDL_Color font_color)
{
    SDL_RenderClear(renderer);

    // Set background color.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, NULL);

    draw_text(renderer, 0, 0, "Testing set", font, font_color);
}


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init video error: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        printf("SDL_Init audio error: %s\n", SDL_GetError());
        return 1;
    }

	// Setup window
	SDL_Window *window = SDL_CreateWindow("Set",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			600, 800,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	// Setup renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Setup font
	TTF_Init();
	TTF_Font *font = TTF_OpenFont("liberation.ttf", 16);
	if (!font)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error: Font", TTF_GetError(), window);
		return -666;
	}
	SDL_Color font_color = {255, 255, 255};

    printf("Set\n");
    srand(time(NULL));

    Game_State game_state;
    generate_deck(&game_state.deck);
    // game_state.board = set;
    game_state.quit = false;
    game_state.reset = true;

    int frame_time_start, frame_time_finish;
    float delta_t = 0;

    while (!game_state.quit)
    {
        frame_time_start = SDL_GetTicks();

        SDL_PumpEvents();
        get_input(&game_state);

        if (!game_state.quit)
        {
            SDL_GetWindowSize(window, &game_state.window.x, &game_state.window.y);

            update(&game_state, delta_t);
            render(renderer, game_state, font, font_color);

            frame_time_finish = SDL_GetTicks();
            delta_t = (float)((frame_time_finish - frame_time_start) / 1000.0f);
        }
    }

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
    return 0;
}
