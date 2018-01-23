# Isometric-Engine

A simple way to display and interact with a 45,45,0 map


This will contain templates for java, javascript, C, and C++

The C and C++ version will be dependent on SDL_2.X for now, and SDL_Image

>>Before beginning:
>>The graphics are assumed to be already rendered in such a way, as to mimic a 45,45,0(x,y,z degree) cube
>>Everything is nearly automatic, just pass in or modify data and everything will be automgically updated seamlessly
>>The init function is very important, as it will define how the engine will work, threads and what to render to

How To Start:

C
#include isoengine.h

//in some function
ISO_init(Your SDL_Renderer,T/F alternateProcessThread,T/F multipleMapRenderingTheads,TargetFPS);

//then add tiles into your map into a tileSet
ISO_extendTileSet(...)//example of a texture is provided
//then setMap data by substituting values in the grid -- manually or by function
ISO_grid[x][y][z]=1;
ISO_setMapData(...)
ISO_setBlockData(...)
//then call this in a gameLoop
ISO_renderIsoMap()

//simple as that!


//to safely and accurately edit data, use this method
ISO_editDirSelect(blockID);
