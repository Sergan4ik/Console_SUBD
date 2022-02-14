#pragma once

#include <stdio.h>
#include "Store.h"
#include "Structures.h"

void printStore(struct Store store)
{
	printf("Store\'s name: %s\n", store.name);
	printf("Store\'s games count: %d\n", store.gamesCount);
}

void printGame(struct Game game, struct Store store)
{
	printf("Store: %s\n", store.name);
	printf("Name : %s\n", game.gameName);
	printf("Price: %d\n", game.price);
}