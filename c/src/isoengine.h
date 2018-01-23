#ifndef ISOENGINE_H_INCLUDED
#define ISOENGINE_H_INCLUDED

#include <SDL2/sdl.h>

#define SHORT_MAX 65535

SDL_Renderer* ISO_defaultRenderer;

typedef struct {
    unsigned char visible;
    unsigned char transparent;
    unsigned char solid;
    unsigned char walkable;
    unsigned char imgState;
    SDL_Surface* img[4];
    SDL_Texture* tex[4];
    void** extend;
} ISO_Tile;

///You may edit these variables directly, but it is not advised
///start variables
ISO_Tile*** ISO_TileSet;
unsigned short ISO_set;
unsigned int ISO_sizes[2];

SDL_Rect* ISO_baseRect;
SDL_Rect* ISO_scaledRect;
unsigned short ISO_tileSize;

unsigned char  ISO_viewdir,ISO_gridCached;
unsigned short ISO_grid[256][256][128];
unsigned char  ISO_gridCache[256][256][128];
unsigned char  ISO_gridCached;
/*unsigned char  ISO_shadeCache[256][256];*/
unsigned short ISO_width;
unsigned short ISO_height;
unsigned short ISO_depth;
float ISO_scale;
int ISO_xscroll,ISO_yscroll;
int ISO_SCREEN_WIDTH,ISO_SCREEN_HEIGHT;

int ISO_slx,ISO_sly,ISO_slz;
int ISO_mouseY,ISO_mouseX;
///end variables

///set all rendering to this target and initialize systems
void ISO_init(SDL_Renderer* defaultRenderer, int threadSelectionDetection, int threadMapRendering, int targetFPS);

///release resources held by the systems
void ISO_exit();

///creates and resets map data
int ISO_generateBlankMap(int w,int h, int dpth);

///sets map boundaries if possible while keeping map data
///useful for psuedo-clipping centered on 0,0 or not as much processing
int ISO_setMapBoundaries(int w,int h, int dpth);

///set map data in a region
void ISO_setMapData(int x, int y, int z, int w, int h, int dpth, short blockID);

///set map data of a block
void ISO_setBlockData(int x, int y, int z, short blockID);

///render this map onto the renderer
void ISO_renderIsoMap();

///detection of selection position with cursor
///only calls after map has been cached
///auto threaded also calls this method, not advised if not manually threaded
void ISO_detectSelect();

///extend with 1 image for all sides
ISO_Tile* ISO_extendTileSet(char* newImage, unsigned char tileSet,unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable);

///extend without any graphical data
ISO_Tile* ISO_extendTileSetBlank(unsigned char tileSet,unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable);

///set images per side, NULL char* default to previous face
int ISO_setTileGraphics(ISO_Tile* tile, char* dir0, char* dir1, char* dir2, char*dir3);

///map edge detection for spawning things -- 0 is true
int ISO_isSpawnTile(int x, int y, int z);

///whether or not to render this tile
int ISO_checkRender(int x, int y, int z);

///set all the images of a tile from file
int ISO_setTileImagesFromFile(ISO_Tile* tile, char* imageFile);

///Attempt to edit the selected location with this block data
void ISO_editDirSelect(unsigned short blockID);




#endif // ISOENGINE_H_INCLUDED
