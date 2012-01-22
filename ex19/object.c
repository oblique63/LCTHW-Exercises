#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "object.h"


int Object_init(void *self) {
    return 1;
}

void Object_describe(void *self) {
    Object *obj = self;
    printf("%s.\n", obj->description);
}

void *Object_move(void *self, Direction direction) {
    puts("You can't go that way.");
    return NULL;
}

int Object_attack(void *self, int damage) {
    puts("You can't attack that.");
    return 0;
}

void Object_destroy(void *self) {
    Object *obj = self;

    if (obj) {
        assert(obj != NULL);
        if (obj->description)
            free(obj->description);
        free(obj);
    }
}


void *Object_new(size_t size, Object proto, char *description) {
    if (!proto.init)
        proto.init = Object_init;
    if (!proto.describe)
        proto.describe = Object_describe;
    if (!proto.move)
        proto.move = Object_move;
    if (!proto.attack)
        proto.attack = Object_attack;
    if (!proto.destroy)
        proto.destroy = Object_destroy;

    Object *el = calloc(size, 1);
    assert(el != NULL);
    *el = proto;

    assert(description != NULL);
    el->description = strdup(description);

    if(!el->init(el)) {
        el->destroy(el);
        return NULL;
    }
    
    else {
        assert(el != NULL);
        return el;
    }
}
