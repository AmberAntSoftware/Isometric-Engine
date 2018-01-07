#ifndef ISOENGINE_H_INCLUDED
#define ISOENGINE_H_INCLUDED

/*
typedef struct {
    short length;
    short off;
    short* data;
} ISO_GridRow;

typedef struct {
    short length;
    ISO_GridRow* data;
} ISO_GridMap;

//ISO_GridMap* ISO_grid;
*/

typedef struct {
    unsigned char visible;
    unsigned char transparent;
    unsigned char solid;
    unsigned char walkable;
    SDL_Surface* img[4];
    SDL_Texture* tex[4];
} ISO_Tile;

ISO_Tile** ISO_TileSet[];
unsigned short ISO_set;
unsigned int ISO_sizes[];

SDL_Rect* ISO_baseRect;
SDL_Rect* ISO_scaledRect;
unsigned short ISO_tileSize;

unsigned char ISO_viewdir,ISO_gridCached;
unsigned short ISO_grid[256][256][128];
unsigned short ISO_width;
unsigned short ISO_height;
unsigned short ISO_depth;
float ISO_scale;
int ISO_xscroll,ISO_yscroll;

unsigned short ISO_slx,ISO_sly,ISO_slz;

void ISO_init();
void ISO_deleteTile(ISO_Tile* tile);
void ISO_exit();

int ISO_generateBlankMap(int w,int h, int dpth);
int ISO_inRect(int rx, int ry, int rw, int rh, int x, int y);
int ISO_inCircle(int rx, int ry, double rm, int x, int y);
int ISO_inDiamond(int x,int y,int xsize,int ysize,int px,int py);
int ISO_inCube(int x,int y,int xsize,int ysize,int px,int py);
int ISO_inRectD(double rx, double ry, double rw, double rh, double x, double y);
int ISO_inCircleD(double rx, double ry, double rm, double x, double y);
int ISO_inDiamondD(double x,double y,double xsize,double ysize,double px,double py);
int ISO_inCubeD(double x,double y,double xsize,double ysize,double px,double py);

//1 image for all sides
ISO_Tile* ISO_extendTileSet(SDL_Renderer* interfaceRenderer, char* newImage, unsigned char tileSet,unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable);
//1 image for all sides, delete inRAM SDL_Surface memory, or GPU textures if software rendering
void ISO_extendTileSet_MIN(SDL_Renderer* interfaceRenderer, char* newImage, unsigned char tileSet,unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable);


#endif // ISOENGINE_H_INCLUDED
