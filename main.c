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

    SDL_Rect card_rect;
    bool selected;

    int id;
    int texture_index;
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

    Vector_2 mouse_position;
    bool mouse_left_click;
    bool mouse_right_click;
    Set candidate_set;
    int candidate_set_length;

    Vector_2 window; 
    SDL_Rect board_rect;

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
    deck->card_count = 0;

    int texture_index = 0;

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

                    deck->cards[deck->card_count].texture_index = texture_index;

                    deck->cards[deck->card_count].selected = false;

                    deck->card_count += 1;
                }


                texture_index += 1;
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

    destination->card_count = 0;

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

SDL_Texture *textures[27];

void set_color(SDL_Surface *surface, SDL_Color color)
{
    int *pixels = surface->pixels;

    for (int i = 0; i < surface->w * surface->h; i += 1)
    {
        if (pixels[i] == SDL_MapRGB(surface->format, 0, 0, 0)) 
        {
            pixels[i] = SDL_MapRGB(surface->format, color.r, color.b, color.g);
        }
    }
}

void load_this_image_all_colors(SDL_Renderer *renderer, char *filename, int *index)
{
    SDL_Surface *base_surface, *red_surface, *green_surface, *purple_surface;

    SDL_Color red = {234, 28, 45, 0};
    SDL_Color green = {0, 169, 80, 0}; 
    SDL_Color purple = {97, 51, 148, 0}; 

    base_surface = IMG_Load(filename);

    red_surface = SDL_ConvertSurface(base_surface, base_surface->format, SDL_SWSURFACE);
    green_surface = SDL_ConvertSurface(base_surface, base_surface->format, SDL_SWSURFACE);
    purple_surface = SDL_ConvertSurface(base_surface, base_surface->format, SDL_SWSURFACE);

    set_color(red_surface, red);
    set_color(green_surface, green);
    set_color(purple_surface, purple);

    textures[*index] = SDL_CreateTextureFromSurface(renderer, red_surface);
    SDL_FreeSurface(red_surface);
    *index += 1;

    textures[*index] = SDL_CreateTextureFromSurface(renderer, green_surface);
    SDL_FreeSurface(green_surface);
    *index += 1;

    textures[*index] = SDL_CreateTextureFromSurface(renderer, purple_surface);
    SDL_FreeSurface(purple_surface);
    *index += 1;
}

void load_images(SDL_Renderer *renderer)
{
    int index = 0;

    load_this_image_all_colors(renderer, "../assets/open_diamond.png", &index);
    load_this_image_all_colors(renderer, "../assets/striped_diamond.png", &index);
    load_this_image_all_colors(renderer, "../assets/solid_diamond.png", &index);
    load_this_image_all_colors(renderer, "../assets/open_squiggle.png", &index);
    load_this_image_all_colors(renderer, "../assets/striped_squiggle.png", &index);
    load_this_image_all_colors(renderer, "../assets/solid_squiggle.png", &index);
    load_this_image_all_colors(renderer, "../assets/open_oval.png", &index);
    load_this_image_all_colors(renderer, "../assets/striped_oval.png", &index);
    load_this_image_all_colors(renderer, "../assets/solid_oval.png", &index);
}

void get_input(Game_State *game_state)
{
    SDL_GetMouseState(&game_state->mouse_position.x, &game_state->mouse_position.y);

    game_state->mouse_left_click = false; 
    game_state->mouse_right_click = false; 

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
                    game_state->mouse_left_click = true;
                }

                if (event.button.button == SDL_BUTTON_RIGHT ) {
                    game_state->mouse_right_click = true;
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

SDL_Rect get_board_rect(Game_State game_state)
{
    float board_padding = 0.05f;

    SDL_Rect board_rect = {
        game_state.window.x * board_padding,
        game_state.window.y * board_padding,
        game_state.window.x * (1.0f - 2*board_padding),
        game_state.window.y * (1.0f - 2*board_padding),
    };

    return board_rect;
}

void set_card_rects(Game_State *game_state)
{
    int rows = 3, columns = 4;
    float card_padding = 0.05f;

    SDL_Rect board_rect = game_state->board_rect;

    int card_id = 0;
    for (int i = 0; i < columns; i += 1)
    {
        for (int j = 0; j < rows; j += 1)
        {
            SDL_Rect card_rect = {
                board_rect.x + (board_rect.w/columns) * i + (board_rect.w/columns) * card_padding,
                board_rect.y + (board_rect.h/rows) * j + (board_rect.h/rows) * card_padding,
                (board_rect.w/columns) - (board_rect.w/columns) * card_padding * 2,  
                (board_rect.h/rows) - (board_rect.h/rows) * card_padding * 2,
            };

            game_state->board.cards[card_id].card_rect = card_rect;
            card_id += 1;
        }
    }
}

void reset_candidate_set(Game_State *game_state)
{
    for (int i = 0; i < game_state->candidate_set_length; i += 1)
    {
        int id_to_find = game_state->candidate_set.ids[i];

        for (int j = 0; j < game_state->board.card_count; j += 1)
        {
            if (game_state->board.cards[j].id == id_to_find) {
                game_state->board.cards[j].selected = false;
                break;
            }

        }
    }

    game_state->candidate_set_length = 0;
}

void update(Game_State *game_state, float delta_t)
{
    if (game_state->reset) {
        generate_random_set_from_deck(game_state->deck, &game_state->board, 12);
        game_state->sets_in_this_board = find_sets(game_state->board);
        game_state->sets_found = 0;

        game_state->board_rect = get_board_rect(*game_state); 
        set_card_rects(game_state);

        reset_candidate_set(game_state);

        game_state->reset = false;
    }

    if (game_state->mouse_left_click) {
        for (int i = 0; i < 12; i += 1)
        {
            SDL_Rect mouse_position_rect = {
                game_state->mouse_position.x,
                game_state->mouse_position.y,
                1,
                1
            };

            SDL_Rect intersection_rect;
            if (SDL_IntersectRect(&mouse_position_rect, &game_state->board.cards[i].card_rect, &intersection_rect)) {
                game_state->candidate_set.ids[game_state->candidate_set_length] = game_state->board.cards[i].id;
                game_state->candidate_set_length += 1;

                game_state->board.cards[i].selected = true;

                if (game_state->candidate_set_length == 3) {
                    if (is_set(game_state->board, 
                           game_state->candidate_set.ids[0], 
                           game_state->candidate_set.ids[1], 
                           game_state->candidate_set.ids[2])) {
                        game_state->sets_found += 1;
                    }

                    reset_candidate_set(game_state);
                    break;
                }

                printf("Clicked: \n");
                print_card(game_state->board.cards[i]);
            }
        }
    }

    if (game_state->mouse_right_click) {
        reset_candidate_set(game_state);
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

void draw_shape(SDL_Renderer *renderer, SDL_Rect shape_rect, int texture_index)
{
    SDL_RenderCopy(renderer, textures[texture_index], NULL, &shape_rect);
}

void draw_card(SDL_Renderer *renderer, SDL_Rect card_rect, Card card)
{
    // Card background
    if (card.selected) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 255, 0);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    }
    SDL_RenderFillRect(renderer, &card_rect);

    // Shapes
    float outer_padding = 0.05f;
    float inter_card_padding = 0.03f;

    int inter_card_padding_px = card_rect.h * inter_card_padding;

    SDL_Rect shape_rect;
    shape_rect.w = card_rect.w  - (card_rect.w * outer_padding * 2);
    shape_rect.h = (card_rect.h - (outer_padding * card_rect.h) - (3 * inter_card_padding_px)) / 3;

    for (int i = 0; i < card.number; i += 1)
    {
        shape_rect.x = card_rect.x + card_rect.w * outer_padding;
        shape_rect.y = card_rect.y + card_rect.h * outer_padding;

        shape_rect.y += shape_rect.h * i;
        shape_rect.y += inter_card_padding_px * i;

        if (card.number == 1) {
            shape_rect.y += shape_rect.h;
        } else if (card.number == 2) {
            shape_rect.y += shape_rect.h * 0.5;
        }

        draw_shape(renderer, shape_rect, card.texture_index);
    }
}

void render(SDL_Renderer *renderer, Game_State game_state, TTF_Font *font, SDL_Color font_color)
{
    SDL_RenderClear(renderer);

    // Set background color.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, NULL);

    draw_text(renderer, 0, 0, "Testing set", font, font_color);

    char progress_text[50];
    sprintf(progress_text, "%d/%d", game_state.sets_found, game_state.sets_in_this_board);
    draw_text(renderer, 0, 15, progress_text, font, font_color);

    // Draw the board
    float board_padding = 0.05f;
    int rows = 3, columns = 4;

    float card_padding = 0.05f;

    int card_id = 0;
    for (int i = 0; i < columns; i += 1)
    {
        for (int j = 0; j < rows; j += 1)
        {
            draw_card(renderer, game_state.board.cards[card_id].card_rect, game_state.board.cards[card_id]);
            card_id += 1;
        }
    }

    SDL_RenderPresent(renderer);
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
			800, 800,
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
    load_images(renderer);

    printf("Set\n");
    srand(time(NULL));

    Game_State game_state;
    generate_deck(&game_state.deck);
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
