function IsoMetricEngine(){
	
	var grid = [];
	var t_width=128,h_width=64,q_width=32;
	var t_height=128,h_height=64,q_height=32;
	var scale = 1;
	var xscroll=-grid.length*t_width*scale,yscroll=300;
	var viewdir=0;

	var slx=-1,sly=-1,slz=-1;

	//http://stackoverflow.com/questions/966225/how-can-i-create-a-two-dimensional-array-in-javascript
	function fillArray(arr){//make sure there are values (doubles) in the array
		for(var i = 0; i < arr.length;i++){
			if(Array.isArray(arr[i])){
				fillArray(arr[i]);
			}else arr[i]=0;
		}
	}
	function createArray(length) {
		var arr = new Array(length || 0),
			i = length;

		if (arguments.length > 1) {
			var args = Array.prototype.slice.call(arguments, 1);
			while(i--) arr[length-1 - i] = createArray.apply(this, args);
		}
		
		fillArray(arr);
		
		return arr;
	}
	//
	
	var g_base=64,g_height=16;
	grid = createArray(g_base,g_base,g_height);

	gridReset();
	function gridReset(){
	for(var x = 0; x <grid.length;x++){
		for(var y = 0; y < grid[x].length;y++){
			for(var z = 0; z < grid[x][y].length;z++){
				grid[x][y][z]=0;
			}
		}
	}
	}

	function renderIsoMap(g){
		switch(viewdir){
			case(0):
			rotateI(g);
			break;
			case(1):
			rotateII(g);
			break;
			case(2):
			rotateIII(g);
			break;
			case(3):
			rotateIV(g);
		}
	}

	function editBock(block){
		if(''+block=='undefined'||block==null)return false;
		if(Point.inDiamond((h_width*slx+h_width*sly)*scale+xscroll,
						(-q_width*slx+q_width*sly-slz*h_height)*scale+yscroll,
				t_width*scale,h_height*scale,
				px, py)){
				
				if(block)
				slz++;
		}else{
			if(block)
			if(px<(h_width*slx+h_width*sly+h_width)*scale+xscroll)
				slx--;
			else
				sly++;

		}
		if(slx<0||sly<0||slz<0||slx==grid.length||sly==grid[0].length||slz==grid[0][0].length)return false;
		//grid[slx][sly][slz]=1;
		switch(viewdir){
			case(0):
			
			if(block==0&&grid[slx][sly][slz]==10&&isSpawnTile(slx,sly,slz)){
				removeEntryRoad(slx,sly,slz);
			}
			modBlock(slx,sly,slz,block?block:grid[slx][sly][slz],block?1:-1);
			var b = blockConditions(slx,sly,slz,block);//intital
			if(b&&block==10&&isSpawnTile(slx,sly,slz)){
				addEntryRoad(slx,sly,slz);
			}
			while(block==0&&slz>0&&slz+1<grid[0][0].length){
				slz++;
				if(!BlockBlock(slx,sly,slz,grid[slx][sly][slz])){
					modBlock(slx,sly,slz,grid[slx][sly][slz],-1);
					grid[slx][sly][slz]=0;
				}
				
			}
			return b;
			case(1):
			
			if(block==0&&grid[sly][grid.length-1-slx][slz]==10&&isSpawnTile(slx,sly,slz)){
				removeEntryRoad(sly,grid.length-1-slx,slz);
			}
			modBlock(sly,grid.length-1-slx,slz,block?block:grid[sly][grid.length-1-slx][slz],block?1:-1);
			var b = blockConditions(sly,grid.length-1-slx,slz,block)
			if(b&&block==10&&isSpawnTile(slx,sly,slz)){
				addEntryRoad(sly,grid.length-1-slx,slz);
			}
			while(block==0&&slz>0&&slz+1<grid[0][0].length){
				slz++;
				if(!BlockBlock(sly,grid.length-1-slx,slz,grid[sly][grid.length-1-slx][slz])){
					modBlock(sly,grid.length-1-slx,slz,grid[sly][grid.length-1-slx][slz],-1);
					grid[sly][grid.length-1-slx][slz]=0;
				}
			}
			return b;
			case(2):
			
			if(block==0&&grid[grid.length-slx-1][grid[0].length-sly-1][slz]==10&&isSpawnTile(grid.length-slx-1,grid[0].length-sly-1,slz)){
				removeEntryRoad(grid.length-slx-1,grid[0].length-sly-1,slz);
			}
			modBlock(grid.length-slx-1,grid[0].length-sly-1,slz,block?block:grid[grid.length-slx-1][grid[0].length-sly-1][slz],block?1:-1);
			var b = blockConditions(grid.length-slx-1,grid[0].length-sly-1,slz,block)
			if(b&&block==10&&isSpawnTile(grid.length-slx-1,grid[0].length-sly-1,slz)){
				addEntryRoad(grid.length-slx-1,grid[0].length-sly-1,slz);
			}
			while(block==0&&slz>0&&slz+1<grid[0][0].length){
				slz++;
				if(!BlockBlock(grid.length-slx-1,grid[0].length-sly-1,slz,grid[grid.length-slx-1][grid[0].length-sly-1][slz])){
					modBlock(grid.length-slx-1,grid[0].length-sly-1,slz,grid[grid.length-slx-1][grid[0].length-sly-1][slz],-1);
					grid[grid.length-slx-1][grid[0].length-sly-1][slz]=0;
				}
			}
			return b;
			case(3):
			if(block==0&&grid[grid[0].length-1-sly][slx][slz]==10&&isSpawnTile(grid[0].length-1-sly,slx,slz)){
				removeEntryRoad(grid[0].length-1-sly,slx,slz);
			}
			modBlock(grid[0].length-1-sly,slx,slz,block?block:grid[grid[0].length-1-sly][slx][slz],block?1:-1);
			var b = blockConditions(grid[0].length-1-sly,slx,slz,block);
			if(b&&block==10&&isSpawnTile(grid[0].length-1-sly,slx,slz)){
				addEntryRoad(grid[0].length-1-sly,slx,slz);
			}
			while(block==0&&slz>0&&slz+1<grid[0][0].length){
				slz++;
				if(!BlockBlock(grid[0].length-1-sly,slx,slz,grid[grid[0].length-1-sly][slx][slz])){
					modBlock(grid[0].length-1-sly,slx,slz,grid[grid[0].length-1-sly][slx][slz],-1);
					grid[grid[0].length-1-sly][slx][slz]=0;
				}
			}
			
			return b;
		}
	}

	function BlockBlock(x,y,z,block){
		switch(block){
			case(0)://empty space
			if(z==0)return false;
		}
		return true;
	}

	function blockConditions(x,y,z,block){
		if(grid[x][y][z]&&block||!BlockBlock(x,y,z,block))return false;
		grid[x][y][z]=block;
		return true;
	}

	function modBlock(x,y,z,block,dir){
		switch(block){
			case(0)://empty space
			break;
		}
		return true;
	}

	var shade = 0;
	function rotateI(g){
		slx=-1;
		sly=-1;
		slz=-1;
		for(var x = 0; x < grid.length;x=~~(x+1)){
			for(var y = grid[x].length-1; y>-1;y=~~(y-1)){
				for(var z = grid[x][y].length-1; z>-1;z=~~(z-1)){
					if(grid[x][y][z]&&checkRender(x,y,z))
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
						slx=x;
						sly=y;
						slz=z;
						break;
					}
				}
				if(slx!=-1)break;
			}
			if(slx!=-1)break;
		}
		
		for(var x = grid.length-1; x > -1;x=~~(x-1)){
			for(var y = 0; y < grid[x].length;y=~~(y+1)){
				shade=0;
				for(var z = grid[x][y].length-1; z>0;z=~~(z-1)){
					if(grid[x][y][z]){
						shade=z;
						break;
					}
				}
				for(var z = 0; z < grid[x][y].length;z=~~(z+1)){
					if(Point.inRect(-t_width,-t_height,width+t_width,height+t_height,(h_width*x+h_width*y)*scale+xscroll,(-q_width*x+q_width*y-z*h_height)*scale+yscroll))
					if(grid[x][y][z]&&checkRender(x,y,z)){
						
					g.drawImage_Fast(PLACEHOLDER[grid[x][y][z]],
					
					(h_width*x+h_width*y)*scale+xscroll,
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					
					(t_width)*scale,(t_height)*scale);
					
					if(slx!=-1&&
					x==slx&&y==sly&&z==slz)
					g.drawImage_Fast(PLACEHOLDER_SELECT,
					
					(h_width*x+h_width*y)*scale+xscroll,
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					
					(t_width)*scale,(t_height)*scale);
					
					}
					
				}
			}
		}
	}

	function rotateII(g){
		slx=-1;
		sly=-1;
		slz=-1;
		
		for(var x = 0; x < grid.length;x=~~(x+1)){
			for(var y = grid[x].length-1; y>-1;y=~~(y-1)){
				for(var z = grid[x][y].length-1; z>-1;z=~~(z-1)){
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
						slx=x;
						sly=y;
						slz=z;
						break;
					}
				}
				if(slx!=-1)break;
			}
			if(slx!=-1)break;
		}
		
		for(var x = grid.length-1; x > -1;x=~~(x-1)){
			for(var y = 0; y < grid[x].length;y=~~(y+1)){
				shade=0;
				for(var z =grid[y][grid.length-1-x].length-1; z>0;z=~~(z-1)){
					if(grid[y][grid.length-1-x][z]){
						shade=z;
						break;
					}
				}
				for(var z = 0; z < grid[y][grid.length-1-x].length;z=~~(z+1)){
					if(Point.inRect(-t_width,-t_height,width+t_width,height+t_height,(h_width*x+h_width*y)*scale+xscroll,(-q_width*x+q_width*y-z*h_height)*scale+yscroll))
					if(grid[y][grid.length-1-x][z]&&checkRender(y,grid.length-1-x,z)){
					g.drawImage_Fast(tiles[grid[y][grid.length-1-x][z]][viewdir+((z<shade&&z<grid[y][grid.length-1-x].length-1&&!grid[y][grid.length-1-x][z+1])?4:0)],
					
					(h_width*x+h_width*y)*scale+xscroll,
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					
					(t_width)*scale,(t_height)*scale);
					
					
					
					if(slx!=-1&&
					x==slx&&y==sly&&z==slz)
					g.drawImage_Fast(tileselector,
					
					(h_width*x+h_width*y)*scale+xscroll,
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					
					(t_width)*scale,(t_height)*scale);
					}
				}
			}
		}
	}

	function rotateIII(g){
		slx=-1;
		sly=-1;
		slz=-1;
		for(var x = 0; x < grid.length;x=~~(x+1)){
			for(var y = grid[x].length-1; y>-1;y=~~(y-1)){
				for(var z = grid[x][y].length-1; z>-1;z=~~(z-1)){
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
						slx=x;
						sly=y;
						slz=z;
						break;
					}
				}
				if(slx!=-1)break;
			}
			if(slx!=-1)break;
		}
		for(var x = grid.length-1; x > -1;x=~~(x-1)){
			for(var y = 0; y < grid[x].length;y=~~(y+1)){
				shade=0;
				for(var z =grid[grid.length-x-1][grid[0].length-y-1].length-1; z>0;z=~~(z-1)){
					if(grid[grid.length-x-1][grid[0].length-y-1][z]){
						shade=z;
						break;
					}
				}
				for(var z = 0; z < grid[x][y].length;z=~~(z+1)){
					if(Point.inRect(-t_width,-t_height,width+t_width,height+t_height,(h_width*x+h_width*y)*scale+xscroll,(-q_width*x+q_width*y-z*h_height)*scale+yscroll))
					if(grid[grid.length-x-1][grid[0].length-y-1][z]&&checkRender(grid.length-x-1,grid[0].length-y-1,z)){
					g.drawImage_Fast(PLACEHOLDER[grid[grid.length-x-1][grid[0].length-y-1][z]],
					
					(h_width*x+h_width*y)*scale+xscroll,
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					
					(t_width)*scale,(t_height)*scale);
					
					
					if(slx!=-1&&
					x==slx&&y==sly&&z==slz)
					g.drawImage_Fast(PLACEHOLDER_SELECT,
					
					(h_width*x+h_width*y)*scale+xscroll,
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					
					(t_width)*scale,(t_height)*scale);
					}
				}
			}
		}
	}

	function rotateIV(g){
		slx=-1;
		sly=-1;
		slz=-1;
		for(var x = 0; x < grid.length;x=~~(x+1)){
			for(var y = grid[x].length-1; y>-1;y=~~(y-1)){
				for(var z = grid[x][y].length-1; z>-1;z=~~(z-1)){
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
						slx=x;
						sly=y;
						slz=z;
						break;
					}
				}
				if(slx!=-1)break;
			}
			if(slx!=-1)break;
		}
		for(var x = grid.length-1; x > -1;x=~~(x-1)){
			for(var y = 0; y < grid[x].length;y=~~(y+1)){
				shade=0;
				for(var z =grid[grid[0].length-1-y][x].length-1; z>0;z=~~(z-1)){
					if(grid[grid[0].length-1-y][x][z]){
						shade=z;
						break;
					}
				}
				for(var z = 0; z < grid[x][y].length;z=~~(z+1)){
					if(Point.inRect(-t_width,-t_height,width+t_width,height+t_height,(h_width*x+h_width*y)*scale+xscroll,(-q_width*x+q_width*y-z*h_height)*scale+yscroll))
					if(grid[grid[0].length-1-y][x][z]&&checkRender(grid[0].length-1-y,x,z)){
					g.drawImage_Fast(PLACEHOLDER[grid[grid[0].length-1-y][x][z]],
					
					(h_width*x+h_width*y)*scale+xscroll,
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					
					(t_width)*scale,(t_height)*scale);
					if(slx!=-1&&
					x==slx&&y==sly&&z==slz)
					g.drawImage_Fast(PLACEHOLDER_SELECT,
					
					(h_width*x+h_width*y)*scale+xscroll,
					(-q_width*x+q_width*y-z*h_height)*scale+yscroll,
					
					(t_width)*scale,(t_height)*scale);
					}
				}
			}
		}
		
	}

	var side = [
			[//0 ,x,y
				[//left
					-1,0
				],
				[//right
					0,1
				]
			],
			[//1
				[//left
					0,-1
				],
				[//right
					1,1
				]
			],
			[//2
				[//left
					1,0
				],
				[//right
					0,-1
				]
			],
			[//3
				[//left
					0,1
				],
				[//right
					-1,-1
				]
			]
	];

	function isSpawnTile(x, y, z){
		switch(x){//map sizes -- of a 64x64x16 map
		case(0):return true;
		case(63):return true;
		}
		switch(y){
		case(0):return true;
		case(63):return true;
		}
		switch(z){
		case(0):return true;
		case(15):return true;
		}
		return false;
	}

	function checkRender(x, y, z){
		switch(x){//map sizes -- of a 64x64x16 map
		case(0):return true;
		case(63):return true;
		}
		switch(y){
		case(0):return true;
		case(63):return true;
		}
		switch(z){
		case(0):return true;
		case(15):return true;
		}//end map sizes -- of a 64x64x16 map
		switch(grid[x+side[viewdir][0][0]][y+side[viewdir][0][1]][z]){
		case(0):return true;
		case(10):return true;
		case(13):return true;
		case(11):return true;
		case(12):return true;
		case(21):return true;
		}
		switch(grid[x+side[viewdir][1][0]][y+side[viewdir][1][1]][z]){
		case(0):return true;
		case(10):return true;
		case(13):return true;
		case(11):return true;
		case(12):return true;
		case(21):return true;
		}
		switch(grid[x][y][z+1]){
		case(0):return true;
		case(10):return true;
		case(13):return true;
		case(11):return true;
		case(12):return true;
		case(21):return true;
		}/**/
		return false;
	}
}