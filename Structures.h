#pragma once

/*
"Structures.h": master (supplier), product (detail), slave (supplement), indexer.
*/

struct Store
{
	int id;
	char name[16];
	long firstGameIdx;
	int gamesCount;
};

struct Game
{
	int storeId;
	char gameName[32];
	int genreId;
	int price;
	int exists;
	long selfIdx;
	long nextIdx;
};

struct Indexer
{
	int id;	
	int address;
	int exists;
};