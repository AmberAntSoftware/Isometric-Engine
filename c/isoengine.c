#include <stdio.h>
#include <stdlib.h>
#include <SDL2/sdl.h>
#include <math.h>
#include "psc_resources.h"

#include "isoengine.h"

static char ISO_softwareRender = 0;//not able to be set currently, defaults to GPU processing

unsigned char ISO_viewdir = 0,ISO_gridCached=0;
unsigned short ISO_width=256,ISO_height=256,ISO_depth=128;
float ISO_scale=1;
int ISO_xscroll=0,ISO_yscroll=0;
ISO_Tile** ISO_TileSet[2];
unsigned short ISO_set=0;
unsigned int ISO_sizes[2]={0,0};
unsigned short ISO_tileSize = 128;

void ISO_init(){
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

    ISO_TileSet[0] = malloc(sizeof(ISO_Tile*));
    ISO_TileSet[1] = malloc(sizeof(ISO_Tile*));

    ISO_sizes[0]=1;
    ISO_sizes[1]=1;

}

void ISO_deleteTile(ISO_Tile* tile){
    int i;
    for(i=0;i<4;i++){
        if(tile->img[i]){
            SDL_FreeSurface(tile->img[i]);
        }
        if(tile->tex[i]){
            SDL_DestroyTexture(tile->tex[i]);
        }
    }
}

void ISO_exit(){

}

int ISO_generateBlankMap(int w,int h, int dpth){

    short x,y,z;

    if(w!=h){
        if(w<h)w=h;
        else h=w;
    }

    /*short* temp = (short*)malloc(sizeof(short)*w*(h+2)*dpth);
    if (temp == NULL)
    {
        printf("Cannot allocate more memory.\n");
        return 0;
    }
    else
    {
        ISO_grid = temp;
        ISO_width=w;
        ISO_height=h+2;
        ISO_depth=dpth;
    }*/
    //ISO_grid = tmp;
    if(w<0||w>256||h<0||h>256||dpth<0||dpth>128){
        w=0;
        h=0;
        dpth=0;
        printf(">>> Map Boundaries Out Of Bounds\n");
        return 1;
    }

    ISO_width=w;
    ISO_height=h;
    ISO_depth=dpth;

    for(x=0;x<w;x++){
        for(y=0;y<h;y++){
            for(z=0;z<dpth;z++){
                ISO_grid[x][y][z]=0;
            }
        }
    }

    return 0;
}

void ISO_renderIsoMap(SDL_Renderer* interfaceRenderer){
    //ISO_viewdir++;
    //ISO_viewdir&=3;
	switch(ISO_viewdir){
		case(0):
            if(ISO_gridCached)
            ;//ISO_unCacheI(g);
            else
            ISO_rotateI(interfaceRenderer);
		break;
		case(1):
            if(ISO_gridCached)
            ;//ISO_unCacheII(g);
            else
            ISO_rotateII(interfaceRenderer);
		break;
		case(2):
            if(ISO_gridCached)
            ;//ISO_unCacheIII(g);
            else
            ISO_rotateIII(interfaceRenderer);
		break;
		case(3):
            if(ISO_gridCached)
            ;//ISO_unCacheIV(g);
            else
            ISO_rotateIV(interfaceRenderer);
		break;/**/
	}
}

void ISO_detectSelect(){
    switch(ISO_viewdir){
		case(0):
            if(ISO_gridCached)
            ;//ISO_unCacheI(g);
            else
            ISO_rotateI(interfaceRenderer);
		break;
		case(1):
            if(ISO_gridCached)
            ;//ISO_unCacheII(g);
            else
            ISO_rotateII(interfaceRenderer);
		break;
		case(2):
            if(ISO_gridCached)
            ;//ISO_unCacheIII(g);
            else
            ISO_rotateIII(interfaceRenderer);
		break;
		case(3):
            if(ISO_gridCached)
            ;//ISO_unCacheIV(g);
            else
            ISO_rotateIV(interfaceRenderer);
		break;/**/
	}
}


void ISO_rotateI(SDL_Renderer* interfaceRenderer){
	register short shade = 0,x,y,z;

	ISO_slx=-1;
	ISO_sly=-1;
	ISO_slz=-1;
    //printf("SIZE: %i\n",(int)(ISO_tileSize*ISO_scale));
	register int TS = (int)(ISO_tileSize*ISO_scale);
	register int HTS = (int)(ISO_tileSize*ISO_scale/2.0);
	register int QTS = (int)(ISO_tileSize*ISO_scale/4.0);
	register int xI = SCREEN_WIDTH+TS;
	register int yI = SCREEN_HEIGHT+TS;
    ISO_scaledRect->w=TS;
    ISO_scaledRect->h=TS;

	for(x = ISO_width-1; x > -1;x--){
		for(y = 0; y < ISO_height;y++){
		    if(ISO_grid[x][y][0]){
                shade=0;
                for(z = ISO_depth-1; z>0;z--){
                    if(ISO_grid[x][y][z]){
                        shade=z;
                        //if(grid[x][y][z]!=17)
                        break;
                    }
                }
                for(z = 0; z < ISO_depth;z++){
                        if(ISO_grid[x][y][z]){
                            ISO_scaledRect->x=HTS*x+HTS*y+ISO_xscroll;
                            ISO_scaledRect->y=-QTS*x+QTS*y-z*HTS+ISO_yscroll;
                            if(ISO_inRect(-TS,-TS,xI,yI,ISO_scaledRect->x,ISO_scaledRect->y)){
                                SDL_RenderCopy(interfaceRenderer,ISO_TileSet[ISO_set][ISO_grid[x][y][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                            }
                        }
                    //if(ISO_inCube(TS*x+HTS*y+ISO_xscroll,-QTS*x+QTS*y-z*TS+ISO_yscroll,TS,TS,mx,my))
                    ///SDL_RenderCopy(interfaceRenderer,ISO_TileSet[ISO_set][ISO_grid[x][y][z]].img,ISO_baseRect,ISO_scaledRect);
                    //g.drawImage_Fast(tiles[grid[x][y][z]][viewdir+((z<shade&&z<grid[x][y].length-1&&!grid[x][y][z+1])?4:0)],


                    /**if(ISO_inRect(-t_width,-t_height,width+t_width,height+t_height,(h_width*x+h_width*y)*scale+xscroll,(-q_width*x+q_width*y-z*h_height)*scale+yscroll))
                    if(grid[x][y][z]&&checkRender(x,y,z)){
                    //if(Point.inRect(-t_width,-t_height,width,height,(h_width*x+h_width*y)*scale+xscroll+t_width,(-q_width*x+q_width*y-z*h_height)*scale+yscroll)+t_height)
                    g.drawImage_Fast(tiles[grid[x][y][z]][viewdir+((z<shade&&z<grid[x][y].length-1&&!grid[x][y][z+1])?4:0)],

                    (h_width*x+h_width*y)*scale+xscroll,
                    (-q_width*x+q_width*y-z*h_height)*scale+yscroll,

                    (t_width)*scale,(t_height)*scale);



                    if(ISO_slx!=-1&&
                    x==ISO_slx&&y==ISO_sly&&z==ISO_slz)
                    g.drawImage_Fast(tileselector,

                    (h_width*x+h_width*y)*scale+xscroll,
                    (-q_width*x+q_width*y-z*h_height)*scale+yscroll,

                    (t_width)*scale,(t_height)*scale);

                    }**/
                    ///renderPonies(x,y,z,x,y,z);
                }
            }//endif
		}
	}
}
void ISO_rotateII(SDL_Renderer *interfaceRenderer){
	register short shade = 0,x,y,z;

	ISO_slx=-1;
	ISO_sly=-1;
	ISO_slz=-1;

	register int TS = (int)(ISO_tileSize*ISO_scale);
	register int HTS = (int)(ISO_tileSize*ISO_scale/2.0);
	register int QTS = (int)(ISO_tileSize*ISO_scale/4.0);
	register int xI = SCREEN_WIDTH+TS;
	register int yI = SCREEN_HEIGHT+TS;
	ISO_scaledRect->w=TS;
    ISO_scaledRect->h=TS;

	for(x = ISO_width-1; x > -1;x--){
		for(y = 0; y < ISO_height;y++){
		    if(ISO_grid[y][ISO_width-1-x][0]){
                shade=0;
                for(z = ISO_depth-1; z>0;z--){
                    if(ISO_grid[y][ISO_width-1-x][z]){
                        shade=z;
                        break;
                    }
                }
                for(z = 0; z < ISO_depth;z++){
                    if(ISO_grid[y][ISO_width-1-x][z]){
                        ISO_scaledRect->x=HTS*x+HTS*y+ISO_xscroll;
                        ISO_scaledRect->y=-QTS*x+QTS*y-z*HTS+ISO_yscroll;
                        if(ISO_inRect(-TS,-TS,xI,yI,ISO_scaledRect->x,ISO_scaledRect->y)){
                            SDL_RenderCopy(interfaceRenderer,ISO_TileSet[ISO_set][ISO_grid[x][y][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                    }
                }
            }
		}
	}
}

void ISO_rotateIII(SDL_Renderer *interfaceRenderer){
	register short shade = 0,x,y,z;

	ISO_slx=-1;
	ISO_sly=-1;
	ISO_slz=-1;

	register int TS = (int)(ISO_tileSize*ISO_scale);
	register int HTS = (int)(ISO_tileSize*ISO_scale/2.0);
	register int QTS = (int)(ISO_tileSize*ISO_scale/4.0);
	register int xI = SCREEN_WIDTH+TS;
	register int yI = SCREEN_HEIGHT+TS;
	ISO_scaledRect->w=TS;
    ISO_scaledRect->h=TS;

	for(x = ISO_width-1; x > -1;x--){
		for(y = 0; y < ISO_height;y++){
		    if(ISO_grid[ISO_width-x-1][ISO_height-y-1][0]){
                shade=0;
                for(z = ISO_depth-1; z>0;z--){
                    if(ISO_grid[ISO_width-x-1][ISO_height-y-1][z]){
                        shade=z;
                        break;
                    }
                }
                for(z = 0; z < ISO_depth;z++){
                    if(ISO_grid[ISO_width-x-1][ISO_height-y-1][z]){
                        ISO_scaledRect->x=HTS*x+HTS*y+ISO_xscroll;
                        ISO_scaledRect->y=-QTS*x+QTS*y-z*HTS+ISO_yscroll;
                        if(ISO_inRect(-TS,-TS,xI,yI,ISO_scaledRect->x,ISO_scaledRect->y)){
                            SDL_RenderCopy(interfaceRenderer,ISO_TileSet[ISO_set][ISO_grid[ISO_width-x-1][ISO_height-y-1][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                    }
                }
            }
		}
	}
}

void ISO_rotateIV(SDL_Renderer *interfaceRenderer){
	register short shade = 0,x,y,z;

	ISO_slx=-1;
	ISO_sly=-1;
	ISO_slz=-1;

	register int TS = (int)(ISO_tileSize*ISO_scale);
	register int HTS = (int)(ISO_tileSize*ISO_scale/2.0);
	register int QTS = (int)(ISO_tileSize*ISO_scale/4.0);
	register int xI = SCREEN_WIDTH+TS;
	register int yI = SCREEN_HEIGHT+TS;
	ISO_scaledRect->w=TS;
    ISO_scaledRect->h=TS;

	for(x = ISO_width-1; x > -1;x--){
		for(y = 0; y < ISO_height;y++){
		    if(ISO_grid[ISO_height-1-y][x][0]){
                shade=0;
                for(z = ISO_depth-1; z>0;z--){
                    if(ISO_grid[ISO_height-1-y][x][z]){
                        shade=z;
                        break;
                    }
                }
                for(z = 0; z < ISO_depth;z++){
                    if(ISO_grid[ISO_height-1-y][x][z]){
                        ISO_scaledRect->x=HTS*x+HTS*y+ISO_xscroll;
                        ISO_scaledRect->y=-QTS*x+QTS*y-z*HTS+ISO_yscroll;
                        if(ISO_inRect(-TS,-TS,xI,yI,ISO_scaledRect->x,ISO_scaledRect->y)){
                            SDL_RenderCopy(interfaceRenderer,ISO_TileSet[ISO_set][ISO_grid[ISO_height-1-y][x][z]]->tex[ISO_viewdir],ISO_baseRect,ISO_scaledRect);
                        }
                    }
                }
            }
		}
	}
}
/*
function rotateII(g){
	shade = 0;
	ISO_slx=-1;
	ISO_sly=-1;
	ISO_slz=-1;

	for(x = 0; x < grid.length;x=~~(x+1)){
		for(y = grid[x].length-1; y>-1;y=~~(y-1)){
			for(z = grid[x][y].length-1; z>-1;z=~~(z-1)){
				if(grid[y][grid.length-1-x][z]&&checkRender(y,grid.length-1-x,z))
				if(Point.inCube(
					//x
					(h_width*x+h_width*y)*scale+xscroll,
					//y
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					//width
					t_width*scale,
					//height
					h_height*scale,
					mx,my
					)){
					ISO_slx=x;
					ISO_sly=y;
					ISO_slz=z;
					break;
				}
			}
			if(ISO_slx!=-1)break;
		}
		if(ISO_slx!=-1)break;
	}

	for(x = grid.length-1; x > -1;x=~~(x-1)){
		for(y = 0; y < grid[x].length;y=~~(y+1)){
			shade=0;
			for(z =grid[y][grid.length-1-x].length-1; z>0;z=~~(z-1)){
				if(grid[y][grid.length-1-x][z]){
					shade=z;
					break;
				}
			}
			for(z = 0; z < grid[y][grid.length-1-x].length;z=~~(z+1)){
				if(Point.inRect(-t_width,-t_height,width+t_width,height+t_height,(h_width*x+h_width*y)*scale+xscroll,(-q_width*x+q_width*y-z*h_height)*scale+yscroll))
				if(grid[y][grid.length-1-x][z]&&checkRender(y,grid.length-1-x,z)){
				g.drawImage_Fast(tiles[grid[y][grid.length-1-x][z]][viewdir+((z<shade&&z<grid[y][grid.length-1-x].length-1&&!grid[y][grid.length-1-x][z+1])?4:0)],

				(h_width*x+h_width*y)*scale+xscroll,
				(-q_width*x+q_width*y-z*h_height)*scale+yscroll,

				(t_width)*scale,(t_height)*scale);



				if(ISO_slx!=-1&&
				x==ISO_slx&&y==ISO_sly&&z==ISO_slz)
				g.drawImage_Fast(tileselector,

				(h_width*x+h_width*y)*scale+xscroll,
				(-q_width*x+q_width*y-z*h_height)*scale+yscroll,

				(t_width)*scale,(t_height)*scale);
				}
				renderPonies(y,grid.length-1-x,z,x,y,z);
			}
		}
	}
}

function rotateIII(g){
	shade = 0;
	ISO_slx=-1;
	ISO_sly=-1;
	ISO_slz=-1;
	for(x = 0; x < grid.length;x=~~(x+1)){
		for(y = grid[x].length-1; y>-1;y=~~(y-1)){
			for(z = grid[x][y].length-1; z>-1;z=~~(z-1)){
				if(grid[grid.length-x-1][grid[0].length-y-1][z]&&checkRender(grid.length-x-1,grid[0].length-y-1,z))
				if(Point.inCube(

					//x
					(h_width*x+h_width*y)*scale+xscroll,
					//y
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					//width
					t_width*scale,
					//height
					h_height*scale,
					mx,my
					)){
					ISO_slx=x;
					ISO_sly=y;
					ISO_slz=z;
					break;
				}
			}
			if(ISO_slx!=-1)break;
		}
		if(ISO_slx!=-1)break;
	}
	for(x = grid.length-1; x > -1;x=~~(x-1)){
		for(y = 0; y < grid[x].length;y=~~(y+1)){
			shade=0;
			for(z =grid[grid.length-x-1][grid[0].length-y-1].length-1; z>0;z=~~(z-1)){
				if(grid[grid.length-x-1][grid[0].length-y-1][z]){
					shade=z;
					break;
				}
			}
			for(z = 0; z < grid[x][y].length;z=~~(z+1)){
				if(Point.inRect(-t_width,-t_height,width+t_width,height+t_height,(h_width*x+h_width*y)*scale+xscroll,(-q_width*x+q_width*y-z*h_height)*scale+yscroll))
				if(grid[grid.length-x-1][grid[0].length-y-1][z]&&checkRender(grid.length-x-1,grid[0].length-y-1,z)){
				g.drawImage_Fast(tiles[grid[grid.length-x-1][grid[0].length-y-1][z]][viewdir+((z<shade&&z<grid[grid.length-x-1][grid[0].length-y-1].length-1&&!grid[grid.length-x-1][grid[0].length-y-1][z+1])?4:0)],

				(h_width*x+h_width*y)*scale+xscroll,
				(-q_width*x+q_width*y-z*h_height)*scale+yscroll,

				(t_width)*scale,(t_height)*scale);


				if(ISO_slx!=-1&&
				x==ISO_slx&&y==ISO_sly&&z==ISO_slz)
				g.drawImage_Fast(tileselector,

				(h_width*x+h_width*y)*scale+xscroll,
				(-q_width*x+q_width*y-z*h_height)*scale+yscroll,

				(t_width)*scale,(t_height)*scale);
				}
				renderPonies(grid.length-x-1,grid[0].length-y-1,z,x,y,z);
			}
		}
	}
}

function rotateIV(g){
	shade = 0;
	ISO_slx=-1;
	ISO_sly=-1;
	ISO_slz=-1;
	for(x = 0; x < grid.length;x=~~(x+1)){
		for(y = grid[x].length-1; y>-1;y=~~(y-1)){
			for(z = grid[x][y].length-1; z>-1;z=~~(z-1)){
				if(grid[grid[0].length-1-y][x][z]&&checkRender(grid[0].length-1-y,x,z))
				if(Point.inCube(

					//x
					(h_width*x+h_width*y)*scale+xscroll,
					//y
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					//width
					t_width*scale,
					//height
					h_height*scale,
					mx,my
					)){
					ISO_slx=x;
					ISO_sly=y;
					ISO_slz=z;
					break;
				}
			}
			if(ISO_slx!=-1)break;
		}
		if(ISO_slx!=-1)break;
	}
	for(x = grid.length-1; x > -1;x=~~(x-1)){
		for(y = 0; y < grid[x].length;y=~~(y+1)){
			shade=0;
			for(z =grid[grid[0].length-1-y][x].length-1; z>0;z=~~(z-1)){
				if(grid[grid[0].length-1-y][x][z]){
					shade=z;
					break;
				}
			}
			for(z = 0; z < grid[x][y].length;z=~~(z+1)){
				if(Point.inRect(-t_width,-t_height,width+t_width,height+t_height,(h_width*x+h_width*y)*scale+xscroll,(-q_width*x+q_width*y-z*h_height)*scale+yscroll))
				if(grid[grid[0].length-1-y][x][z]&&checkRender(grid[0].length-1-y,x,z)){
				g.drawImage_Fast(tiles[grid[grid[0].length-1-y][x][z]][viewdir+((z<shade&&z<grid[grid[0].length-1-y][x].length-1&&!grid[grid[0].length-1-y][x][z+1])?4:0)],

				(h_width*x+h_width*y)*scale+xscroll,
				(-q_width*x+q_width*y-z*h_height)*scale+yscroll,

				(t_width)*scale,(t_height)*scale);
				if(ISO_slx!=-1&&
				x==ISO_slx&&y==ISO_sly&&z==ISO_slz)
				g.drawImage_Fast(tileselector,

				(h_width*x+h_width*y)*scale+xscroll,
				(-q_width*x+q_width*y-z*h_height)*scale+yscroll,

				(t_width)*scale,(t_height)*scale);
				}
				renderPonies(grid[0].length-1-y,x,z,x,y,z);
			}
		}
	}
}*/


char side[4][2][2] = {
		{//0 ,x,y
			{//left
				-1,0
			},
			{//right
				0,1
			}
		},
		{//1
			{//left
				0,-1
			},
			{//right
				1,1
			}
		},
		{//2
			{//left
				1,0
			},
			{//right
				0,-1
			}
		},
		{//3
			{//left
				0,1
			},
			{//right
				-1,-1
			}
		}
};

int ISO_isSpawnTile(int x, int y, int z, int ox, int oy, int oz, int mx, int my, int mz){
	return (x-ox)*(x-mx)*(y-oy)*(y-my)*(z-oz)*(z-mz);
}

int ISO_checkRender(int x, int y, int z, int ox, int oy, int oz, int mx, int my, int mz){
	if(!ISO_isSpawnTile(x,y,z,ox,oy,oz,mx,my,mz))return 1;
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




void ISO_setTileGraphics(SDL_Renderer* interfaceRenderer, ISO_Tile* tile, char* face0, char* face1, char* face2, char* face3){

}
void ISO_setTileGraphics_MIN(SDL_Renderer* interfaceRenderer, ISO_Tile* tile, char* face0, char* face1, char* face2, char* face3){

}

ISO_Tile* ISO_extendTileSetBlank(SDL_Renderer* interfaceRenderer, unsigned char tileSet,unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable){
    ISO_Tile** tmp = realloc(ISO_TileSet[tileSet],(ISO_sizes[tileSet]*sizeof(ISO_Tile*)));
    if(tmp==NULL){
        return NULL;
    }
    ISO_TileSet[tileSet] = tmp;
    ISO_Tile* t = malloc(sizeof(ISO_Tile));
    if(t==NULL){
        console("Out Of Memory\n",0);
        return NULL;
    }
    int i;
    for(i=0i<4;i++){
        t->img[i]=NULL;
        t->tex[i]=NULL;
    }
    t->visible=visible;
    t->transparent=transparent;
    t->solid=solid;
    t->walkable=walkable;

    ISO_TileSet[tileSet][ISO_sizes[tileSet]]++;
    ISO_sizes[tileSet]++;
    return t;
}

void ISO_extendTileSet_MIN(SDL_Renderer* interfaceRenderer, char* newImage, unsigned char tileSet,unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable){
    ISO_Tile* tile = ISO_extendTileSet(interfaceRenderer, newImage, tileSet,visible,transparent,solid,walkable);
    int i;
    for(i=0;i<4;i++){
        if(ISO_softwareRender){
            if(tile->tex[i]){
                SDL_DestroyTexture(tile->tex[i]);
            }
        }else{
            if(tile->img[i]){
                SDL_FreeSurface(tile->img[i]);
            }
        }
    }
}
ISO_Tile* ISO_extendTileSet(SDL_Renderer* interfaceRenderer, char* newImage, unsigned char tileSet,unsigned char visible,unsigned char transparent,unsigned char solid,unsigned char walkable){
    ISO_Tile** tmp = realloc(ISO_TileSet[tileSet],(ISO_sizes[tileSet]*sizeof(ISO_Tile*)));
    if(tmp==NULL){
        return NULL;
    }
    ISO_TileSet[tileSet] = tmp;
    /*((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->img=NULL;
    ((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->img=IMG_Load(newImage);

    if(((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->img==NULL)return;
    ((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->tex=NULL;
    ((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->tex=SDL_CreateTextureFromSurface(interfaceRenderer,((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->img);

    ((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->visible=visible;
    ((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->transparent=transparent;
    ((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->solid=solid;
    ((ISO_Tile*)(&ISO_TileSet[tileSet][ISO_sizes[tileSet]]))->walkable=walkable;
    */
    ISO_Tile* t = malloc(sizeof(ISO_Tile));
    if(t==NULL){
        console("Out Of Memory\n",0);
        return;
    }
    auto int i;
    for(i=0;i<4;i++){
        t->img[i]=NULL;
        t->tex[i]=NULL;
    }

    t->img[0]=NULL;
    t->img[0]=IMG_Load(newImage);
    if(t->img[0]==NULL){
        console("Could not load: ",0);
        console(newImage,0);
        console("\n",0);
        return NULL;
    }

    for(i=1;i<4;i++){
        t->img[i]=t->img[0];
    }
    t->tex[0]=NULL;
    t->tex[0]=SDL_CreateTextureFromSurface(interfaceRenderer,t->img[0]);

    for(i=1;i<4;i++){
        t->tex[i]=t->tex[0];
    }
    /*for(i=0;i<4;i++){
        t->tex[i]=SDL_CreateTextureFromSurface(interfaceRenderer,t->img[i]);
        if(t->img[0]==NULL){
            console("Could not create SDL_Texture For: ",0);
            char tx[1]={i+32};
            console(tx,0);
            console("\n",0);
            return;
        }
    }*/
    t->visible=visible;
    t->transparent=transparent;
    t->solid=solid;
    t->walkable=walkable;

    ISO_TileSet[tileSet][ISO_sizes[tileSet]]=t;

    ISO_sizes[tileSet]++;

    return t;

}

/*
void ISO_loadImagesRelative(char* str, SDL_Surface* storeThis){
    storeThis = NULL;
    //storeThis = IMG_Load("/res/img/grass.png");
    storeThis = IMG_Load(str);

    if(storeThis==NULL)
        printf("Could not load IMAGE: [%s]",str);

    /*

    //*
    SDL_Surface* imgFnt = NULL;
    SDL_Texture* txtrFnt = NULL,*txtrFnt2 = NULL;
    imgFnt = IMG_Load("5bit_font_clear.png");

    if(imgFnt==NULL)
        exit_msg("Could not load pixel font");

    txtrFnt = SDL_CreateTextureFromSurface(renderer, imgFnt);
    txtrFnt2 = SDL_CreateTextureFromSurface(rendpop, imgFnt);
    //txtrFnt = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, imgFnt->w, imgFnt->h);
    if(txtrFnt==NULL||txtrFnt2==NULL)
        exit_msg("Could not generate texture based pixel font");
    //*/


//}

