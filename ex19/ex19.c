#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "ex19.h"


int Monster_init(void *self) {
    Monster *monster = self;
    monster->hit_points = 10;
    return 1;
}

int Monster_attack(void *self, int damage) {
    Monster *monster = self;

    // obj->_(func) == obj->proto.func
    printf("You attack %s!\n", monster->_(description));

    monster->hit_points -= damage;

    if (monster->hit_points > 0) {
        puts("It is still alive.");
        return 0;
    }
    else {
        puts("It is dead.");
        return 1;
    }
}

Object MonsterProto = {
    .init = Monster_init,
    .attack = Monster_attack
};


void *Room_move(void *self, Direction direction) {
    Room *room = self;
    Room *next = NULL;

    if (direction == NORTH && room->north) {
        puts("You go north, into:");
        next = room->north;
    }
    else if (direction == SOUTH && room->south) {
        puts("You go south, into:");
        next = room->south;
    }
    else if (direction == EAST && room->east) {
        puts("You go east, into:");
        next = room->east;
    }
    else if (direction == WEST && room->west) {
        puts("You go west, into:");
        next = room->west;
    }
    else {
        puts("You can't go that direction.");
        next = NULL;
    }

    if (next)
        next->_(describe)(next);

    return next;
}

int Room_attack(void *self, int damage) {
    Room *room = self;
    Monster *monster = room->bad_guy;

    if (monster) {
        monster->_(attack)(monster, damage);
        return 1;
    }
    else {
        puts("You flail in the air at nothing. Idiot.");
        return 0;
    }
}

Object RoomProto = {
    .move = Room_move,
    .attack = Room_attack,
};


int Map_init(void *self) {
    Map *map = self;

    // NEW(Obj, desc) == Object_new(sizeof(Obj), ObjProto, desc);
    // where ObjProto is a partially implemented prototype of the
    // Object struct.
    Room *hall = NEW(Room, "The Great Hall");
    Room *throne = NEW(Room, "The throne room");
    Room *arena = NEW(Room, "The arena, with the Minotaur");
    Room *kitchen = NEW(Room, "The kitchen, you have the knife now");

    arena->bad_guy = NEW(Monster, "The evil minotaur");

    hall->north = throne;
    throne->south = hall;
    throne->west = arena;
    throne->east = kitchen;
    
    arena->east = throne;
    kitchen->west = throne;

    map->start = hall;
    map->location = hall;

    return 1;
}

void *Map_move(void *self, Direction direction) {
    Map *map = self;
    Room *location = map->location;
    Room *next = NULL;

    next = location->_(move)(location, direction);

    if (next)
        map->location = next;

    return next;
}

int Map_attack(void *self, int damage) {
    Map *map = self;
    Room *location = map->location;
    return location->_(attack)(location, damage);
}

/* attempt at freeing up memory...
void Map_destroy(Map *map) {
    
    if (map) {
        assert(map != NULL);
        
        if (map->start->bad_guy)
            map->start->bad_guy->_(destroy)(map->start->bad_guy);
        if (map->location->bad_guy)
            map->location->bad_guy->_(destroy)(map->location->bad_guy);
        
        if (map->start)
            map->start->_(destroy)(map->start);
        if (map->location)
            map->location->_(destroy)(map->location);

        //map->_(destroy)(map);
        free(map);
    }
}
*/

Object MapProto = {
    .init = Map_init,
    .move = Map_move,
    .attack = Map_attack,
    //.destroy = Map_destroy
};


int process_input(Map *game, char input) {
    printf("\n> ");

    
    char ch;
    if (input)
        ch = input;
    else {
        ch = getchar();
        getchar(); // ignore ENTER
    }

    int damage = rand() % 4;

    switch (ch) {
        case 'q':
        case -1:
            puts("Giving up? You suck.");
            return 0;
            break;

        case 'n':
            game->_(move)(game, NORTH);
            break;
            
        case 's':
            game->_(move)(game, SOUTH);
            break;

        case 'e':
            game->_(move)(game, EAST);
            break;

        case 'w':
            game->_(move)(game, WEST);
            break;

        case 'a':
            game->_(attack)(game, damage);
            break;

        case 'l':
            puts("You can go:");
            if (game->location->north) puts("NORTH");
            if (game->location->south) puts("SOUTH");
            if (game->location->east) puts("EAST");
            if (game->location->west) puts("WEST");
            break;
            
        default:
            printf("What?: %d\n", ch);
    }

    return 1;
}



int main(int argc, char *argv[]) {
    // setup randomness
    srand(time(NULL));

    Map *game = NEW(Map, "Hall of the Minotaur");
    assert(game != NULL);
    
    printf("You enter the ");
    game->location->_(describe)(game->location);

    // process arguments as input to automate testing
    if (argc > 1) {
        int i = 1;
        for (i = 1; i < argc; i++) {
            process_input(game, argv[i][0]);
        }
    }

    // interactive
    else {
        while (process_input(game, '\0')) {}
    }

    //Map_destroy(game);
    return 0;
}
