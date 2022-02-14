#pragma once
#include <stdio.h>
#include <string.h>
#include "Structures.h"
#include "Store.h"

void readStore(struct Store* store)
{
	char name[16];
	int status;

	name[0] = '\0';

	printf("Enter store\'s name: ");
	scanf("%s", name);

	strcpy(store->name, name);
}

void readGames(struct Game* game)
{
	int x;
	int price;

	char name[32];
	name[0] = '\0';

	printf("Enter game name: ");
	scanf("%s", name);
	strcpy(game->gameName , name);

	printf("Enter price: ");
	scanf("%d", &price);
	game->price = price;
}