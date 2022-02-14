#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Game.h"
#include "Store.h"

int checkFileExistence(FILE* indexTable, FILE* database, char* error)
{
	if (indexTable == NULL || database == NULL)
	{
		strcpy(error, "database files are not created yet");
		return 0;
	}

	return 1;
}

int checkIndexExistence(FILE* indexTable, char* error, int id)
{
	fseek(indexTable, 0, SEEK_END);

	long indexTableSize = ftell(indexTable);

	if (indexTableSize == 0 || id * sizeof(struct Indexer) > indexTableSize)
	{
		strcpy(error, "no such ID in table");
		return 0;
	}

	return 1;
}

int checkRecordExistence(struct Indexer indexer, char* error)
{
	if (!indexer.exists)
	{
		strcpy(error, "the record you\'re looking for has been removed");
		return 0;
	}

	return 1;
}

int checkKeyPairUniqueness(struct Store store, int productId)
{
	FILE* gameDB = fopen(GAMES_DATA, "r+b");
	struct Game game;

	fseek(gameDB, store.firstGameIdx, SEEK_SET);

	for (int i = 0; i < store.gamesCount; i++)
	{
		fread(&game, GAME_SIZE, 1, gameDB);
		fclose(gameDB);

		if (game.genreId == productId)
		{
			return 0;
		}

		gameDB = fopen(GAMES_DATA, "r+b");
		fseek(gameDB, game.nextIdx, SEEK_SET);
	}

	fclose(gameDB);

	return 1;
}

void info()
{
	FILE* indexTable = fopen("master.ind", "rb");

	if (indexTable == NULL)
	{
		printf("Error: database files are not created yet\n");
		return;
	}

	int storeCount = 0;
	int gamesCount = 0;

	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Indexer);

	struct Store store;

	char dummy[51];

	for (int i = 1; i <= indAmount; i++)
	{
		if (getStore(&store, i, dummy))
		{
			storeCount++;
			gamesCount += store.gamesCount;

			printf("Store #%d has %d game(s)\n", i, store.gamesCount);
		}
	}

	fclose(indexTable);

	printf("Total stores: %d\n", storeCount);
	printf("Total games: %d\n", gamesCount);
}
