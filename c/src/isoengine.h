#ifndef ISOENGINE_H_INCLUDED
#define ISOENGINE_H_INCLUDED

#include <SDL2/sdl.h>

#define SHORT_MAX 65535



typedef struct ISO_Tile{
    unsigned char visible;
    unsigned char transparent;
    unsigned char solid;//as in capable of placing things on top of this
    unsigned char walkable;
    unsigned char imgState;
    SDL_Surface*  img[16];
    SDL_Texture*  tex[16];
    void**        extend;
} ISO_Tile;

/*
typedef struct ISO_Sprite{
    unsigned char          visible;
    unsigned int           animFrames;
    unsigned int           curFrame;
    unsigned int           animSpeedMS;
    SDL_Rect*              clip;
    SDL_Surface*           img;
    SDL_Texture*           tex;
    short                  x,y,z;
    struct ISO_Sprite*     next;
    struct ISO_Sprite*     prev;
    struct ISO_SpriteList* _X_place;///where this exists in the spriteList -- DO NOT EDIT
    void (*renderSprite)(struct ISO_Sprite* sprite);
    void**                 extend;
} ISO_Sprite;
*/

typedef struct ISO_SpriteLayer{
    Uint8 r,g,b;
    SDL_Rect*              clip;
    SDL_Surface*           img;
    SDL_Texture*           tex;
    struct ISO_SpriteLayer*sprite;
} ISO_SpriteLayer;

typedef struct ISO_Sprite{
    ISO_SpriteLayer*       sprite;
    struct ISO_Sprite*     next;
    struct ISO_Sprite*     prev;
    struct ISO_SpriteList* _X_place;///where this exists in the spriteList -- DO NOT EDIT
    /*Custom User Data*/
    void (*renderSprite)(struct ISO_Sprite* sprite);
    void                 *extend;
    void (*freeExtension)(void*);
} ISO_Sprite;

typedef struct ISO_SpriteList{
    ISO_Sprite*            leaf;
    struct ISO_SpriteList* next;
    struct ISO_SpriteList* prev;
} ISO_SpriteList;

///You may edit these variables directly, but it is not advised

///start variables
SDL_Renderer*  ISO_defaultRenderer;
ISO_Tile***    ISO_TileSet;
unsigned short ISO_set;
unsigned int   ISO_sizes[2];

SDL_Rect*      ISO_baseRect;
SDL_Rect*      ISO_scaledRect;//DEPRECATED
unsigned short ISO_tileSize;

unsigned char  ISO_viewdir,ISO_gridCached;
unsigned short ISO_grid[256][256][128];
unsigned char  ISO_gridCache[256][256][128];
unsigned char  ISO_gridCached;
/*unsigned char  ISO_shadeCache[256][256];*/
ISO_Sprite*    ISO_spriteCache[256][256][128];//linked list fashion can exceed 256*256*128 sprites
ISO_SpriteList*ISO_sprites;//list of all sprites
unsigned short ISO_width;
unsigned short ISO_height;
unsigned short ISO_depth;

float          ISO_scale;
int            ISO_xscroll;
int            ISO_yscroll;
int            ISO_SCREEN_WIDTH;
int            ISO_SCREEN_HEIGHT;
int            ISO_slx;
int            ISO_sly;
int            ISO_slz;
int            ISO_mouseY;
int            ISO_mouseX;
unsigned short ISO_boundX;
unsigned short ISO_boundY;

///end variables


///set all rendering to this target and initialize systems
void ISO_init(SDL_Renderer* defaultRenderer, int threadSelectionDetection, int threadMapRendering, int targetFPS);

///release resources held by the system
void ISO_exit();

///creates and resets map data
int ISO_generateBlankMap(int w,int h, int dpth);

///sets map boundaries if possible while keeping map data
///useful for psuedo-clipping centered on 0,0 or not as much processing
int ISO_setMapBoundaries(int w,int h, int dpth);

///set map data in a region
///safe to use out of bounds coordinates
void ISO_setMapData(int x, int y, int z, int w, int h, int dpth, short blockID);

///set map data of a block
///safe to use out of bounds coordinates
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

///map edge detection -- 0 is true
int ISO_isEdgeTile(int x, int y, int z);

///whether or not to render this tile
int ISO_checkRender(int x, int y, int z);

///set all the images of a tile from file
int ISO_setTileImagesFromFile(ISO_Tile* tile, char* imageFile);

///Attempt to edit the selected location with this block data
void ISO_editDirSelect(unsigned short blockID);

///Sets which data set to render from
void ISO_setGraphicsSet(unsigned short setID);

///If Automatic, sets the target FPS of independent threads
void ISO_setTargetFPS(int FPS);

//FIXME
///Add a blank sprite
ISO_Sprite* ISO_extendSpriteSetBlank();
///Add a graphical layer for a sprite (reverse order, last layer add first)
int ISO_addSpriteLayer(ISO_Sprite* sprite, char *file, Uint8 r, Uint8 g, Uint8 b, int keepSurface);
///Renders a sprite at specified rect
void ISO_renderSprite(ISO_Sprite *sprite, SDL_Rect *scaledRect);
//FIXME
/*
///Copy last sprite in sprite set for recoloring
ISO_Sprite* ISO_extendSpriteSetCloneLast();
///Copy the sprite for recoloring
ISO_Sprite* ISO_extendSpriteSetClone(ISO_Sprite* sprite);
*/

///Various ways to make adding tiles easier to understand when browsing

///creates a solid, non walkable block
ISO_Tile* ISO_extendTileSet_solid(char* newImage);
///NOTE: Only way to automatically make a block walkable -- same as solid
///creates a walkable, solid, semi-transparent block
ISO_Tile* ISO_extendTileSet_path(char* newImage);
///creates a solid, semi-transparent block
ISO_Tile* ISO_extendTileSet_decoration(char* newImage);
///creates a non-solid, semi-transparent block
ISO_Tile* ISO_extendTileSet_gas(char* newImage);
///creates a non-solid, block
ISO_Tile* ISO_extendTileSet_liquid(char* newImage);
///creates a block without any capabilities (void block)
ISO_Tile* ISO_extendTileSet_none(char* newImage);

///Must manually set tile graphics

///creates a solid, non walkable block
ISO_Tile* ISO_extendTileSetBlank_solid();
///NOTE: Only way to automatically make a block walkable
///creates a walkable, solid, semi-transparent block due to sprite drawing 'inside' square
ISO_Tile* ISO_extendTileSetBlank_path();
///creates a solid, semi-transparent block
ISO_Tile* ISO_extendTileSetBlank_decoration();
///creates a non-solid, semi-transparent block
ISO_Tile* ISO_extendTileSetBlank_gas();
///creates a non-solid, block
ISO_Tile* ISO_extendTileSetBlank_liquid();
///creates a block without any capabilities (void block)
ISO_Tile* ISO_extendTileSetBlank_none();


#endif // ISOENGINE_H_INCLUDED
