#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL_image.h>
#include <SDL2/sdl.h>

#include "isoengine.h"

/*void* ISO_genMultiDArray(int nrows, int ncolumns, void* array){
    return malloc(nrows * sizeof *array + (nrows * (ncolumns * sizeof *array)));
}*/

static char ISO_inited = 0;
static char ISO_threadedSelector = 0;
static char ISO_threadedMap = 0;
static int ISO_FPS = 60;
static int ISO_FPS_DELAY = 16;
//static char ISO_softwareRender = 0;//not able to be set currently, defaults to GPU processing
///Not Advised To Use These: ISO_X_
void ISO_X_deleteTile(ISO_Tile* tile);
ISO_Tile* ISO_X_createTile(unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable);
int ISO_X_LoadImageIntoTileDataIndex(ISO_Tile* tile, char* file, int index);
int ISO_X_extendTileSet(unsigned char tileSet);
void ISO_X_autoReCache();
int ISO_inRect(int rx, int ry, int rw, int rh, int x, int y);
int ISO_inCircle(int rx, int ry, double rm, int x, int y);
int ISO_inDiamond(int x,int y,int xsize,int ysize,int px,int py);
int ISO_inCube(int x,int y,int xsize,int ysize,int px,int py);
int ISO_inRectD(double rx, double ry, double rw, double rh, double x, double y);
int ISO_inCircleD(double rx, double ry, double rm, double x, double y);
int ISO_inDiamondD(double x,double y,double xsize,double ysize,double px,double py);
int ISO_inCubeD(double x,double y,double xsize,double ysize,double px,double py);
int ISO_floatsEqual(float n0, float n1);
void ISO_X_detectSelectThreaded();
void ISO_rotate();
void ISO_unCache();






unsigned char ISO_viewdir = 0;
unsigned char ISO_gridCached=0;
unsigned short ISO_width=0,ISO_height=0,ISO_depth=0;
float ISO_scale=.5f;
int ISO_SCREEN_WIDTH=800,ISO_SCREEN_HEIGHT=600;
int ISO_xscroll=0,ISO_yscroll=0;
int ISO_mouseY=~0,ISO_mouseX=~0;
ISO_Tile*** ISO_TileSet;
unsigned short ISO_set=0;
unsigned int ISO_sizes[2]={0,0};
unsigned short ISO_tileSize = 128;
SDL_Renderer* ISO_defaultRenderer;

void ISO_init(SDL_Renderer* defaultRenderer, int threadSelectionDetection, int threadMapRendering, int targetFPS){

    ISO_defaultRenderer = defaultRenderer;
    ISO_threadedSelector = threadSelectionDetection;
    if(threadSelectionDetection){
        SDL_CreateThread(ISO_X_detectSelectThreaded,"ISO_DSTr",NULL);
    }
    ISO_threadedMap = threadMapRendering;
    ISO_FPS_DELAY = 1000/targetFPS;
    ISO_FPS = targetFPS;

    ISO_baseRect=malloc(sizeof(SDL_Rect));
    ISO_baseRect->x=0;
    ISO_baseRect->y=0;
    ISO_baseRect->w=ISO_tileSize;
    ISO_baseRect->h=ISO_tileSize;

    ISO_scaledRect=malloc(sizeof(SDL_Rect));
    ISO_scaledRect->x=0;
    ISO_scaledRect->y=0;
    ISO_scaledRect->w=ISO_tileSize;
    ISO_scaledRect->h=ISO_tileSize;


    ISO_TileSet = malloc(sizeof(ISO_Tile***));//length of 1 array containing 1 array of pointers
    ISO_TileSet[0] = malloc(sizeof(ISO_Tile**));//length of 1 data now

    //fast graphical processing hacks for air and tile selector
    ISO_TileSet[0][0] = malloc(sizeof(ISO_Tile*));
    ISO_TileSet[0][0] = ISO_X_createTile(0,0,0,1);
    ISO_setTileImagesFromFile(ISO_TileSet[0][0],"selectorMIN.png");

    ISO_sizes[0]=1;
    //ISO_sizes[1]=1;

    ISO_inited=1;

}

void ISO_exit(){

    ISO_threadedSelector = 0;

    int x;
    int i;
    for(i=0;i<ISO_sizes[0];i++){
        if(ISO_TileSet[0][i]){
            ISO_X_deleteTile(ISO_TileSet[0][i]);
        }
    }

}

int ISO_setMapBoundaries(int w,int h, int dpth){

    if(w<0||w>256||h<0||h>256||dpth<0||dpth>128){
        w=0;
        h=0;
        dpth=0;
        printf(">>> Map Boundaries Out Of Bounds\n");
        return -1;
    }

    ISO_width=w;
    ISO_height=h;
    ISO_depth=dpth;

    return 0;

}
int ISO_generateBlankMap(int w,int h, int dpth){

    short x,y,z;

    if(w!=h){
        if(w<h)w=h;
        else h=w;
    }

    if(w<0||w>256||h<0||h>256||dpth<0||dpth>128){
        w=0;
        h=0;
        dpth=0;
        printf(">>> Map Boundaries Out Of Bounds\n");
        return -1;
    }

    ISO_width=w;
    ISO_height=h;
    ISO_depth=dpth;



    return 0;
}
void ISO_setMapData(int x, int y, int z, int w, int h, int dpth, short blockID){
    int xw=x+w;
    int yh=x+h;
    int zd=z+dpth;
    for(x=0;x<xw;x++){
        for(y=0;y<yh;y++){
            for(z=0;z<zd;z++){
                if(x>-1&&x<ISO_width){
                    if(y>-1&&y<ISO_height){
                        if(z>-1&&z<ISO_depth){
                            ISO_grid[x][y][z]=blockID;
                        }
                    }
                }
            }
        }
    }
}
void ISO_setBlockData(int x, int y, int z, short blockID){
    if(x>-1&&x<ISO_width){
        if(y>-1&&y<ISO_height){
            if(z>-1&&z<ISO_depth){
                ISO_grid[x][y][z]=blockID;
            }
        }
    }
}



void ISO_renderIsoMap(){

    ISO_X_autoReCache();

    if(ISO_gridCached){
            if(!ISO_threadedSelector){
                ISO_detectSelect();
            }
        ISO_unCache();
    }else{
        ISO_rotate();
    }
}

void ISO_editDirSelect(unsigned short blockID){
    //x,y,z are slx,sly,slz
    if(ISO_slx==-1||ISO_sly==-1||ISO_slz==-1){
        return;
    }
    int TS = (int)(ISO_tileSize*ISO_scale);
    int HTS = (int)(ISO_tileSize*ISO_scale/2.0);
	int QTS = (int)(ISO_tileSize*ISO_scale/4.0);
	ISO_scaledRect->x=HTS*ISO_slx+HTS*ISO_sly+ISO_xscroll;
    ISO_scaledRect->y=-QTS*ISO_slx+QTS*ISO_sly-ISO_slz*HTS+ISO_yscroll;


    if(ISO_inDiamond(ISO_scaledRect->x,ISO_scaledRect->y,
                     TS,HTS,ISO_mouseX,ISO_mouseY)){

            //if(block)
            ISO_slz++;
    }else{
        if(ISO_mouseX<ISO_scaledRect->x+HTS){
            ISO_slx--;
        }else{
            ISO_sly++;
        }
    }
    //make sure new selection is in bounds
    if(ISO_slx==-1||ISO_sly==-1||ISO_slz==-1){
        return;
    }
    if(ISO_slx==ISO_width||ISO_sly==ISO_height||ISO_slz==ISO_depth){
        return;
    }

    int dirSetX[4] = {ISO_slx,ISO_sly,ISO_width-ISO_slx-1,ISO_height-1-ISO_sly};
    int dirSetY[4] = {ISO_sly,ISO_width-1-ISO_slx,ISO_height-ISO_sly-1,ISO_slx};

    if(!ISO_grid[dirSetX[ISO_viewdir]][dirSetY[ISO_viewdir]][0]){//non square shaped maps have empty sections, do not allow in empty sections
        return;
    }
    if(ISO_grid[dirSetX[ISO_viewdir]][dirSetY[ISO_viewdir]][ISO_slz]){//no overwriting other tiles
        return;
    }

    ISO_grid[dirSetX[ISO_viewdir]][dirSetY[ISO_viewdir]][ISO_slz]=blockID;

    //printf("%i, %i, %i\n",ISO_slx,ISO_slx,ISO_slx);
    //printf("BLOCK: %i\n",ISO_grid[dirSetX[ISO_viewdir]][dirSetY[ISO_viewdir]][ISO_slz+1]);


    ISO_gridCached=0;
}

void ISO_detectSelect(){
    SDL_Rect dump;
    ISO_slx=-1;
	ISO_sly=-1;
	ISO_slz=-1;
	int TS = (int)(ISO_tileSize*ISO_scale);
	int HTS = (int)(ISO_tileSize*ISO_scale/2.0);
	int QTS = (int)(ISO_tileSize*ISO_scale/4.0);
    dump.w=TS;
    dump.h=TS;
    short x,y,z;
    for(x = 0; x < ISO_width ;x++){
		for(y = ISO_height-1; y+1 ;y--){
            dump.x=HTS*x+HTS*y+ISO_xscroll;
            for(z = ISO_depth-1; z+1 ;z--){
                    dump.y=-QTS*x+QTS*y-z*HTS+ISO_yscroll;
                    switch(ISO_viewdir){
                        case(0):
                            if(ISO_grid[x][y][0]&&ISO_gridCache[x][y][z]){
                                if(ISO_inCube(dump.x,dump.y,TS,HTS,ISO_mouseX,ISO_mouseY)){
                                    ISO_slx=x;ISO_sly=y;ISO_slz=z;
                                    return;
                                }
                            }
                        break;
                        case(1):
                            if(ISO_grid[y][ISO_width-1-x][0]&&ISO_gridCache[y][ISO_width-1-x][z]){
                                if(ISO_inCube(dump.x,dump.y,TS,HTS,ISO_mouseX,ISO_mouseY)){
                                    ISO_slx=x;ISO_sly=y;ISO_slz=z;
                                    return;
                                }
                            }
                        break;
                        case(2):
                            if(ISO_grid[ISO_width-1-x][ISO_height-1-y][0]&&ISO_gridCache[ISO_width-1-x][ISO_height-1-y][z]){
                                if(ISO_inCube(dump.x,dump.y,TS,HTS,ISO_mouseX,ISO_mouseY)){
                                    ISO_slx=x;ISO_sly=y;ISO_slz=z;
                                    return;
                                }
                            }
                        break;
                        case(3):
                            if(ISO_grid[ISO_height-1-y][x][0]&&ISO_gridCache[ISO_height-1-y][x][z]){
                                if(ISO_inCube(dump.x,dump.y,TS,HTS,ISO_mouseX,ISO_mouseY)){
                                    ISO_slx=x;ISO_sly=y;ISO_slz=z;
                                    return;
                                }
                            }
                       //break;
                    }
            }
		}
    }
}

void ISO_unCache(){
	register short x,y,z,gheight,gdepth,gwidth1,gheight1;//shade = 0
	gwidth1=ISO_width-1;
	gheight=ISO_height;
	gheight1=ISO_height-1;
	gdepth=ISO_depth;
	register int TS = (int)(ISO_tileSize*ISO_scale);
	register int HTS = (int)(ISO_tileSize*ISO_scale/2.0);
	register int QTS = (int)(ISO_tileSize*ISO_scale/4.0);
    ISO_scaledRect->w=TS;
    ISO_scaledRect->h=TS;
	for(x = gwidth1; x+1 ;x--){
		for(y = 0; y < gheight;y++){
            ISO_scaledRect->x=HTS*x+HTS*y+ISO_xscroll;
            for(z = 0; z < gdepth;z++){
                ISO_scaledRect->y=-QTS*x+QTS*y-z*HTS+ISO_yscroll;
                switch(ISO_viewdir){
                    case(0):
                        if(ISO_grid[x][y][0]&&ISO_gridCache[x][y][z]){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][ISO_grid[x][y][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);

                        }
                        if((x==ISO_slx)&&(z==ISO_slz)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((y==ISO_sly)&&(z==ISO_slz)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((x==ISO_slx)&&(y==ISO_sly)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                    break;
                    case(1):
                        if(ISO_grid[y][gwidth1-x][0]&&ISO_gridCache[y][gwidth1-x][z]){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][ISO_grid[y][gwidth1-x][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((x==ISO_slx)&&(z==ISO_slz)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((y==ISO_sly)&&(z==ISO_slz)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((x==ISO_slx)&&(y==ISO_sly)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                    break;
                    case(2):
                        if(ISO_grid[gwidth1-x][gheight1-y][0]&&ISO_gridCache[gwidth1-x][gheight1-y][z]){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][ISO_grid[gwidth1-x][gheight1-y][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((x==ISO_slx)&&(z==ISO_slz)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((y==ISO_sly)&&(z==ISO_slz)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((x==ISO_slx)&&(y==ISO_sly)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                    break;
                    case(3):
                        if(ISO_grid[gheight1-y][x][0]&&ISO_gridCache[gheight1-y][x][z]){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][ISO_grid[gheight1-y][x][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((x==ISO_slx)&&(z==ISO_slz)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((y==ISO_sly)&&(z==ISO_slz)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                        if((x==ISO_slx)&&(y==ISO_sly)){
                            SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][0]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                   // break;
                }
            }
		}
	}
}
void ISO_rotate(){
    static SDL_Rect autoSize;
    SDL_RenderGetViewport(ISO_defaultRenderer,&autoSize);

	register short x,y,z;//shade = 0

	ISO_slx=-1;
	ISO_sly=-1;
	ISO_slz=-1;

	register int TS = (int)(ISO_tileSize*ISO_scale);
	register int HTS = (int)(ISO_tileSize*ISO_scale/2.0);
	register int QTS = (int)(ISO_tileSize*ISO_scale/4.0);
	register int xI = autoSize.w+TS;
	register int yI = autoSize.h+TS;
    ISO_scaledRect->w=TS;
    ISO_scaledRect->h=TS;

	for(x = ISO_width-1; x > -1;x--){
		for(y = 0; y < ISO_height;y++){
            ISO_scaledRect->x=HTS*x+HTS*y+ISO_xscroll;
            for(z = 0; z < ISO_depth;z++){
                ISO_scaledRect->y=-QTS*x+QTS*y-z*HTS+ISO_yscroll;
                switch(ISO_viewdir){
                    case(0):
                        ISO_gridCache[x][y][z]=0;
                        if(ISO_grid[x][y][z]){
                            if(ISO_inRect(-TS,-TS,xI,yI,ISO_scaledRect->x,ISO_scaledRect->y)&&ISO_checkRender(x,y,z)){
                                SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][ISO_grid[x][y][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                                ISO_gridCache[x][y][z]=1;
                            }
                        }
                    break;
                    case(1):
                        ISO_gridCache[y][ISO_width-1-x][z]=0;
                        if(ISO_grid[y][ISO_width-1-x][z]){
                            if(ISO_inRect(-TS,-TS,xI,yI,ISO_scaledRect->x,ISO_scaledRect->y)&&ISO_checkRender(x,y,z)){
                                SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][ISO_grid[y][ISO_width-1-x][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                                ISO_gridCache[y][ISO_width-1-x][z]=1;
                            }
                        }
                    break;
                    case(2):
                        ISO_gridCache[ISO_width-x-1][ISO_height-y-1][z]=0;
                        if(ISO_grid[ISO_width-x-1][ISO_height-y-1][z]){
                            if(ISO_inRect(-TS,-TS,xI,yI,ISO_scaledRect->x,ISO_scaledRect->y)&&ISO_checkRender(x,y,z)){
                                SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][ISO_grid[ISO_width-x-1][ISO_height-y-1][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                                ISO_gridCache[ISO_width-x-1][ISO_height-y-1][z]=1;
                            }
                        }
                    break;
                    case(3):
                        ISO_gridCache[ISO_height-1-y][x][z]=0;
                        if(ISO_grid[ISO_height-1-y][x][z]){
                            if(ISO_inRect(-TS,-TS,xI,yI,ISO_scaledRect->x,ISO_scaledRect->y)&&ISO_checkRender(x,y,z)){
                                SDL_RenderCopy(ISO_defaultRenderer,ISO_TileSet[ISO_set][ISO_grid[ISO_height-1-y][x][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                                ISO_gridCache[ISO_height-1-y][x][z]=1;
                            }
                        }
                    break;
                }

                }
            }//endif
		}
	ISO_gridCached=1;
}

char ISO_xDir[4] = {-1,1,1,-1};
char ISO_yDir[4] = {1,1,-1,-1};

///map edge detection for spawning things
int ISO_isSpawnTile(int x, int y, int z){
	return (x-ISO_width+1)*(y-ISO_height+1)*(z-ISO_depth+1)*x*y;//0 is true -- tests bottom clipping
}
///whether or not to render this tile -- empty tiles and non-square maps ignored
int ISO_checkRender(int x, int y, int z){

    if(!ISO_isSpawnTile(x,y,z)){
        return 1;//if it is on the edge, render it, due to clipping rules
	}

    int dirSetX[4] = {x,y,ISO_width-x-1,ISO_height-1-y};
    int dirSetY[4] = {y,ISO_width-1-x,ISO_height-y-1,x};

    return
    (
    (!ISO_TileSet[ISO_set][ISO_grid[dirSetX[ISO_viewdir]][dirSetY[ISO_viewdir]][z+1]]->visible||
     ISO_TileSet[ISO_set][ISO_grid[dirSetX[ISO_viewdir]][dirSetY[ISO_viewdir]][z+1]]->transparent)
    ||
    (!ISO_TileSet[ISO_set][ISO_grid[dirSetX[ISO_viewdir]+ISO_xDir[ISO_viewdir]][dirSetY[ISO_viewdir]][z]]->visible||
     ISO_TileSet[ISO_set][ISO_grid[dirSetX[ISO_viewdir]+ISO_xDir[ISO_viewdir]][dirSetY[ISO_viewdir]][z]]->transparent)
    ||
    (!ISO_TileSet[ISO_set][ISO_grid[dirSetX[ISO_viewdir]][dirSetY[ISO_viewdir]+ISO_yDir[ISO_viewdir]][z]]->visible||
     ISO_TileSet[ISO_set][ISO_grid[dirSetX[ISO_viewdir]][dirSetY[ISO_viewdir]+ISO_yDir[ISO_viewdir]][z]]->transparent)
    )
    &&
    ISO_TileSet[ISO_set][ISO_grid[dirSetX[ISO_viewdir]][dirSetY[ISO_viewdir]][z]]->visible;

	return 0;
}





int ISO_floatsEqual(float n0, float n1){
    if(n0<0){
        n0=-n0;
    }
    if(n1<0){
        n1=-n1;
    }
    n0-=n1;
    if(n0<0){
        n0=-n0;
    }
    if(n0<.001){//delta value
        return 1;
    }
    return 0;
}

int ISO_inRect(int rx, int ry, int rw, int rh, int x, int y) {
    return (x > rx) && (x < rx + rw) && (y > ry) && (y < ry + rh);
}

int ISO_inCircle(int rx, int ry, double rm, int x, int y) {
    return (sqrt((x - rx - rm) * (x - rx - rm) + (y - ry - rm) * (y - ry - rm)) <= rm);
}

int ISO_inDiamond(int x,int y,int xsize,int ysize,int px,int py){
    return ISO_inRect(0,0,xsize,ysize,px-x-xsize/2+(py-y)*xsize/ysize,py-y)&&ISO_inRect(0,0,xsize,ysize,px-x,py-y+ysize/2-(px-x)*ysize/xsize);
}

int ISO_inCube(int x,int y,int xsize,int ysize,int px,int py){
    return (ISO_inDiamond(x,y,xsize,ysize,px,py)||ISO_inDiamond(x,y+ysize,xsize,ysize,px,py)||ISO_inRect(x,y+ysize/2,xsize,ysize,px,py));
}

int ISO_inRectD(double rx, double ry, double rw, double rh, double x, double y) {
    return (x > rx) && (x < rx + rw) && (y > ry) && (y < ry + rh);
}

int ISO_inCircleD(double rx, double ry, double rm, double x, double y) {
    return (sqrt((x - rx - rm) * (x - rx - rm) + (y - ry - rm) * (y - ry - rm)) <= rm);
}

int ISO_inDiamondD(double x,double y,double xsize,double ysize,double px,double py){
    return ISO_inRect(0,0,xsize,ysize,px-x-xsize/2+(py-y)*xsize/ysize,py-y)&&ISO_inRect(0,0,xsize,ysize,px-x,py-y+ysize/2-(px-x)*ysize/xsize);
}

int ISO_inCubeD(double x,double y,double xsize,double ysize,double px,double py){
    return (ISO_inDiamond(x,y,xsize,ysize,px,py)||ISO_inDiamond(x,y+ysize,xsize,ysize,px,py)||ISO_inRect(x,y+ysize/2,xsize,ysize,px,py));
}





int ISO_setTileGraphics(ISO_Tile* tile, char* dir0, char* dir1, char* dir2, char*dir3){
    if(tile==NULL){
        printf("Initialization had invalid ISO_Tile\n");
        return 1;
    }

    ISO_X_LoadImageIntoTileDataIndex(tile,dir0,0);
    ISO_X_LoadImageIntoTileDataIndex(tile,dir1,1);
    ISO_X_LoadImageIntoTileDataIndex(tile,dir2,2);
    ISO_X_LoadImageIntoTileDataIndex(tile,dir3,3);

    return 0;
}

ISO_Tile* ISO_extendTileSet(char* newImage, unsigned char tileSet,unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable){

    ISO_Tile* tile = ISO_X_createTile(visible,transparent,solid,walkable);
    if(tile==NULL){
        return NULL;
    }

    if(ISO_setTileImagesFromFile(tile, newImage)){
        printf("Failed To Add Tile Graphics\n");
        ISO_X_deleteTile(tile);
        return NULL;
    }

    if(ISO_X_extendTileSet(tileSet)){
        ISO_X_deleteTile(tile);
        return NULL;
    }

    ISO_TileSet[tileSet][ISO_sizes[tileSet]-1]=tile;

    return tile;

}

ISO_Tile* ISO_extendTileSetBlank(unsigned char tileSet,unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable){

    ISO_Tile* tile = ISO_X_createTile(visible,transparent,solid,walkable);
    if(tile==NULL){
        return NULL;
    }

    if(ISO_X_extendTileSet(tileSet)){
        ISO_X_deleteTile(tile);
        return NULL;
    }

    return tile;
}

int ISO_setTileImagesFromFile(ISO_Tile* tile, char* imageFile){
    if(!tile){
        printf("Tile Selected To Add Images To Did Not Exist\n");
        return 1;
    }
    tile->img[0]=IMG_Load(imageFile);
    if(tile->img[0]==NULL){
        printf("Could not load %s\n",imageFile);
        return 1;
    }
    int i;
    for(i=1;i<4;i++){
        tile->img[i]=tile->img[0];
    }

    tile->tex[0]=SDL_CreateTextureFromSurface(ISO_defaultRenderer,tile->img[0]);
    if(tile->tex[0]==NULL){
        SDL_FreeSurface(tile->img[0]);
        printf("Could not change surface into a Texture %s, %p\n",imageFile,tile->img[0]);
        return 1;
    }
    for(i=1;i<4;i++){
        tile->tex[i]=tile->tex[0];
    }
    return 0;
}




void ISO_X_autoReCache(){
    static int scrollX = 0,scrollY = 0,dir=0;
    static int vW = 800,vH = 600;
    static SDL_Rect view;
    static float zoom = 1.0f;
    SDL_RenderGetViewport(ISO_defaultRenderer,&view);

    //check for changes and if map needs to be recached
    if(zoom!=ISO_scale||//okay to check floats here as exact numbers
       scrollX!=ISO_xscroll||
       scrollY!=ISO_yscroll||
       vW!=view.w||
       vH!=view.h||
       dir!=ISO_viewdir){//auto recache and render -- may be undesirable in some situations, will be alt later

        ISO_gridCached=0;//recache due to changes
        ISO_slx=-1;//remove selected tile to prevent errors
        ISO_sly=-1;
        ISO_slz=-1;

        //auto resiszing view size
        vW=view.w;
        vH=view.h;
        ISO_SCREEN_WIDTH=vW;
        ISO_SCREEN_HEIGHT=vH;

        //no crash scale handler
        if(ISO_scale<.1f){
            ISO_scale=.1f;
        }
        if(ISO_scale>2.0f){
            ISO_scale=2.0f;
        }
        //safe to cache now
        zoom=ISO_scale;

        //auto correcting overscroll or ridiculous coordinates
        if(ISO_xscroll>vW-(int)(ISO_tileSize*ISO_scale)){
            ISO_xscroll=vW-(int)(ISO_tileSize*ISO_scale);
        }
        if(ISO_yscroll>vH+(int)(ISO_tileSize/2*ISO_scale*(ISO_depth))){
            ISO_yscroll=vH+(int)(ISO_tileSize/2*ISO_scale*(ISO_depth));
        }
        if(ISO_xscroll<-(int)(ISO_tileSize*ISO_scale*(ISO_width-1))){
            ISO_xscroll=-(int)(ISO_tileSize*ISO_scale*(ISO_width-1));
        }
        if(ISO_yscroll<-(int)(ISO_tileSize/4*ISO_scale*(ISO_height-1))){
            ISO_yscroll=-(int)(ISO_tileSize/4*ISO_scale*(ISO_height-1));
        }
        //safe to cache now
        scrollX=ISO_xscroll;
        scrollY=ISO_yscroll;

        //prevent unsupported views
        if(dir<0){
            dir=0;
        }else{
            dir&=3;
        }
        //safe to cache now
        dir=ISO_viewdir;

    }
}

void ISO_X_deleteTile(ISO_Tile* tile){
    if(!tile){
        return;
    }

    int i,z;
    for(i=0;i<4;i++){
        if(tile->img[i]){
            for(z=0;z<4;z++){
                if(tile->img[i]==tile->img[z]&&z!=i){
                    tile->img[i]=NULL;
                }
            }
            SDL_FreeSurface(tile->img[i]);
        }
        if(tile->tex[i]){
            for(z=0;z<4;z++){
                if(tile->tex[i]==tile->tex[z]&&z!=i){
                    tile->tex[i]=NULL;
                }
            }
            SDL_DestroyTexture(tile->tex[i]);
        }
    }

}
ISO_Tile* ISO_X_createTile(unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable){
    ISO_Tile* t = malloc(sizeof(ISO_Tile));
    if(t==NULL){
        printf("Could not create new Tile\n");
        return NULL;
    }
    int i;
    for(i=0;i<4;i++){
        t->img[i]=NULL;
        t->tex[i]=NULL;
    }
    t->visible=visible;
    t->transparent=transparent;
    t->solid=solid;
    t->walkable=walkable;
    t->imgState=0;
    t->extend=NULL;
    return t;
}

int ISO_X_LoadImageIntoTileDataIndex(ISO_Tile* tile, char* file, int index){
    if(index>3||index<0){
        printf("Index is out of bounds: %i\n",index);
        return 1;
    }
    if(file==NULL){
        if(index>0){
            int i;
            for(i=index-1;i+1;i--){
                if(tile->img[i]||tile->tex[i]){
                    tile->img[index]=tile->img[i];
                    tile->tex[index]=tile->tex[i];
                    return 0;
                }
            }
        }
        printf("Could Not Allocate Image To Tile");
        return 1;
    }

    tile->img[index]=IMG_Load(file);
    if(tile->img[index]==NULL){
        printf("Could not load %s\n",file);
        return 1;
    }
    tile->tex[index]=SDL_CreateTextureFromSurface(ISO_defaultRenderer,tile->img[index]);
    if(tile->tex[index]==NULL){
        SDL_FreeSurface(tile->img[index]);
        printf("Could not change surface into a Texture %s, %p\n",file,tile->img[index]);
        return 1;
    }
    return 0;
}

int ISO_X_extendTileSet(unsigned char tileSet){
    if(ISO_sizes[tileSet]==SHORT_MAX){
        printf("No Extension, max length\n");
        return 1;
    }
    void* tmp = realloc(ISO_TileSet[tileSet],((ISO_sizes[tileSet]+1)*sizeof(ISO_Tile*)));
    if(tmp==NULL){
        printf("No Extension, expansion error\n");
        return 1;
    }
    ISO_TileSet[tileSet] = tmp;
    ISO_sizes[tileSet]++;
    return 0;
}



void ISO_X_detectSelectThreaded(){
    Uint32 last;
    Sint32 count;
    int lx=ISO_mouseX,ly=ISO_mouseY;
    while(ISO_threadedSelector){
        last = SDL_GetTicks();

        if(ISO_inited&&ISO_gridCached){
            if(ISO_mouseX!=lx||ISO_mouseY!=ly){
                ISO_detectSelect();
            }
        }

        lx=ISO_mouseX;
        ly=ISO_mouseY;

        count = ISO_FPS_DELAY-(SDL_GetTicks()-last);
        if(count>0){
            SDL_Delay(count);
        }//if less than 0 no delay, max performance to keep frames


    }
}
