#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

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

void find_sets(Deck deck)
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

int main(int argc, char *argv[])
{
    printf("Set\n");
    srand(time(NULL));

    Deck deck = {0};
    generate_deck(&deck);
    // print_deck(deck);
    // find_sets(deck);

    Deck set = {0};
    generate_random_set_from_deck(deck, &set, 12);
    print_deck(set);
    find_sets(set);

    return 0;
}
