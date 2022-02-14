#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "Structures.h"
#include "Checks.h"
#include "Store.h"

#define GAMES_DATA "games.fl"
#define GAMES_GARBAGE "games_garbage.txt"
#define GAME_SIZE sizeof(struct Game)


void reopenDatabase(FILE* database)
{
	fclose(database);
	database = fopen(GAMES_DATA, "r+b");
}

void linkAddresses(FILE* database, struct Store store, struct Game game)
{
	reopenDatabase(database);								

	struct Game previous;

	fseek(database, store.firstGameIdx, SEEK_SET);

	for (int i = 0; i < store.gamesCount; i++)		    
	{
		fread(&previous, GAME_SIZE, 1, database);
		fseek(database, previous.nextIdx, SEEK_SET);
	}

	previous.nextIdx = game.selfIdx;				
	fwrite(&previous, GAME_SIZE, 1, database);		
}

void relinkAddresses(FILE* database, struct Game previous, struct Game game, struct Store* store)
{
	if (game.selfIdx == store->firstGameIdx)		
	{
		if (game.selfIdx == game.nextIdx)			
		{
			store->firstGameIdx = -1;					
		}
		else                                            
		{
			store->firstGameIdx = game.nextIdx;  
		}
	}
	else                                         
	{
		if (game.selfIdx == game.nextIdx)		
		{
			previous.nextIdx = previous.selfIdx; 
		}
		else                                     
		{
			previous.nextIdx = game.nextIdx;	
		}

		fseek(database, previous.selfIdx, SEEK_SET);
		fwrite(&previous, GAME_SIZE, 1, database);	
	}
}

void noteDeletedGame(long address)
{
	FILE* garbageZone = fopen(GAMES_GARBAGE, "rb");		

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	long* delAddresses = malloc(garbageCount * sizeof(long)); 

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%ld", delAddresses + i);
	}

	fclose(garbageZone);							
	garbageZone = fopen(GAMES_GARBAGE, "wb");				
	fprintf(garbageZone, "%ld", garbageCount + 1);			

	for (int i = 0; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %ld", delAddresses[i]);		
	}

	fprintf(garbageZone, " %d", address);					
	free(delAddresses);										
	fclose(garbageZone);									
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Game* record)
{
	long* delIds = malloc(garbageCount * sizeof(long));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);				
	}

	record->selfIdx = delIds[0];						
	record->nextIdx = delIds[0];

	fclose(garbageZone);									
	fopen(GAMES_GARBAGE, "wb");							    
	fprintf(garbageZone, "%d", garbageCount - 1);			

	for (int i = 1; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	free(delIds);											
	fclose(garbageZone);									
}

int insertGame(struct Store store, struct Game game, char* error)
{
	game.exists = 1;

	FILE* database = fopen(GAMES_DATA, "a+b");
	FILE* garbageZone = fopen(GAMES_GARBAGE, "rb");

	int garbageCount;											

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount)											
	{
		overwriteGarbageAddress(garbageCount, garbageZone, &game);
		reopenDatabase(database);								
		fseek(database, game.selfIdx, SEEK_SET);			
	}
	else                                                    
	{
		fseek(database, 0, SEEK_END);

		int dbSize = ftell(database);

		game.selfIdx = dbSize;
		game.nextIdx = dbSize;
	}

	fwrite(&game, GAME_SIZE, 1, database);					

	if (!store.gamesCount)								    
	{
		store.firstGameIdx = game.selfIdx;
	}
	else                                                       
	{
		linkAddresses(database, store, game);
	}

	fclose(database);										

	store.gamesCount++;										
	updateStore(store, error);								

	return 1;
}

int getSlave(struct Store store, struct Game* game, int productId, char* error)
{
	if (!store.gamesCount)									
	{
		strcpy(error, "This master has no slaves yet");
		return 0;
	}

	FILE* database = fopen(GAMES_DATA, "rb");


	fseek(database, store.firstGameIdx, SEEK_SET);		
	fread(game, GAME_SIZE, 1, database);

	for (int i = 0; i < store.gamesCount; i++)			
	{
		if (game->genreId == productId)					
		{
			fclose(database);
			return 1;
		}

		fseek(database, game->nextIdx, SEEK_SET);
		fread(game, GAME_SIZE, 1, database);
	}
	
	strcpy(error, "No such game in database");			
	fclose(database);
	return 0;
}

int updateSlave(struct Game game, int productId)
{
	FILE* database = fopen(GAMES_DATA, "r+b");

	fseek(database, game.selfIdx, SEEK_SET);
	fwrite(&game, GAME_SIZE, 1, database);
	fclose(database);
	
	return 1;
}

int deleteSlave(struct Store store, struct Game game, int productId, char* error)
{
	FILE* database = fopen(GAMES_DATA, "r+b");
	struct Game previous;

	fseek(database, store.firstGameIdx, SEEK_SET);

	do		                                                    
	{															
		fread(&previous, GAME_SIZE, 1, database);
		fseek(database, previous.nextIdx, SEEK_SET);
	}
	while (previous.nextIdx != game.selfIdx && game.selfIdx != store.firstGameIdx);

	relinkAddresses(database, previous, game, &store);
	noteDeletedGame(game.selfIdx);						

	game.exists = 0;											

	fseek(database, game.selfIdx, SEEK_SET);				
	fwrite(&game, GAME_SIZE, 1, database);					
	fclose(database);

	store.gamesCount--;										
	updateStore(store, error);

}