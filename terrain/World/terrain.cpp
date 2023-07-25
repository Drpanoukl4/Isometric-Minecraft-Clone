   #include "terrain.h"
   #include <iostream>

    void sWorld::Create ( int Height, int Width, int Depth ) 
    {

        //Noise fnoise;
        s_Chunks.resize(Chunks_Sizes.x * Chunks_Sizes.y);
        s_Chunks.clear();
        srand(time(0));
        
        for( int x = 0; x < Chunks_Sizes.x; x ++ )
        {

            for( int y = 0; y < Chunks_Sizes.y; y ++ )
            {

                s_Chunks[ (y * Chunks_Sizes.x) + x].blocks.resize( Height * Width * Depth );
                s_Chunks[ (y * Chunks_Sizes.x) + x].pos = {x, y};
                
            }

        }

        size = { Width , Height, Depth };

    }

    chunk& sWorld::GetChunk ( const vi2d& v )
    {   
        
        vi2d c_posi = {v.x / Chunk_Const.x, v.y / Chunk_Const.y};
        vi2d vec2 = { c_posi.x / Chunks_Sizes.x, c_posi.y / Chunks_Sizes.y };

        if( vec2.x >= 0.0f && vec2.x < Chunks_Sizes.x && vec2.y >= 0.0f && vec2.y < Chunks_Sizes.y )
        {

            return s_Chunks[ (vec2.y * Chunks_Sizes.x) + vec2.x];

        }else 
        {

            return NullChunk;

        }

    }

    block& sWorld::GetCell ( const vecint3D& v ) 
    {

        vecint3D vCur = { v.x % size.x, v.y % size.y, v.z };

        if( v.x >= 0.0f && v.x < size.x * Chunks_Sizes.x && v.y >= 0.0f && v.y < size.y * Chunks_Sizes.y )
        {

            return GetChunk ( {v.x, v.y} ).blocks[ (size.x * size.y * vCur.z) + (size.x * vCur.y) + vCur.x ];

        }else 
        {

            return NullCell;

        }

    }

    bool sWorld::is_Air_Face ( int x, int y, int z ) 
    {

        return GetCell({x, y, z}).bisAir;

    }

    void sWorld::getFaceQuad( const vecint3D &vCell, const float fAngle, const float Pitch, const float fScale, const vec3D &Camera, std::vector < sQuad >& render, int Height, const bool bBlock) 
    {

        auto &cell = GetCell( vCell);

        std::array < vec3D, 8 > projCube = CreateCube( vCell, fAngle, Pitch, fScale, Camera, Height, true );

        auto MakeFaces = [&] ( int v1, int v2, int v3, int v4, Face f, std::array<vec3D, 8> &projCube, vecint3D Cell)
        {
            
            sQuad x = {projCube[v1], projCube[v2], projCube[v3], projCube[v4], cell.id[f], Cell};
            render.push_back(x);

        };

        if( !cell.bWall ) 
        {

            if(bVisible[ Face::Floor ])
            {
                MakeFaces( 4, 0, 1, 5, Face::Floor, projCube, vCell);
            };

        }else 
        {

            if(bVisible[ Face::South ] && is_Air_Face ( vCell.x, vCell.y - 1, vCell.z )) 
                MakeFaces( 3, 0, 1, 2, Face::South, projCube, vCell);

            if(bVisible[ Face::North ] && is_Air_Face ( vCell.x, vCell.y + 1, vCell.z ))
                MakeFaces( 6, 5, 4, 7, Face::North, projCube, vCell);

            if(bVisible[ Face::East ] && is_Air_Face ( vCell.x - 1, vCell.y, vCell.z ))
                MakeFaces( 7, 4, 0, 3, Face::East, projCube, vCell);

            if(bVisible[ Face::West ] && is_Air_Face ( vCell.x + 1, vCell.y, vCell.z ))
                MakeFaces( 2, 1, 5, 6, Face::West, projCube, vCell);

            if(bVisible[ Face::Top ] && is_Air_Face ( vCell.x, vCell.y, vCell.z + 1 ))
                MakeFaces( 7, 3, 2, 6, Face::Top, projCube, vCell);

        }

    }

    std::array < vec3D, 8 > sWorld::CreateCube (const vecint3D & vCell, const float fAngle, const float Pitch, const float fScale, const vec3D &Camera, int Height, const bool bBlock) 
    {

        std::array < vec3D, 8 > unitCube, rotCube, worldCube, projCube;
        auto &cell = GetCell(vCell);

        unitCube[0] = { 0.0f, -fScale * Height, 0.0f };
        unitCube[1] = { fScale, -fScale * Height, 0.0f };
        unitCube[2] = { fScale, -fScale -fScale * Height, 0.0f };
        unitCube[3] = { 0.0f, -fScale -fScale* Height, 0.0f };
        unitCube[4] = { 0.0f, -fScale * Height, fScale };
        unitCube[5] = { fScale, -fScale * Height, fScale };
        unitCube[6] = { fScale, -fScale -fScale * Height, fScale };
        unitCube[7] = { 0.0f, -fScale -fScale * Height, fScale };

        // Translate Cube in x-z Plane  //

        for ( int i = 0; i < 8; i++ ) 
        {

            unitCube[i].x += ( vCell.x * fScale - Camera.x );
            unitCube[i].y +=  -Camera.y;
            unitCube[i].z += ( vCell.y * fScale - Camera.z );

        }
        // Rotate Cube in Y-Axis around Origin  //
        for ( int i = 0; i < 8; i++ ) 
        {

            rotCube[i].x = unitCube[i].x * cosf( fAngle ) + unitCube[i].z * sinf( fAngle ) ;
            rotCube[i].y = unitCube[i].y;
            rotCube[i].z = unitCube[i].x * -sinf( fAngle ) + unitCube[i].z * cosf( fAngle );

        }

        // Rotate Cube in X-Axis around Origin  //

        for ( int i = 0; i < 8; i++ ) 
        {

            worldCube[i].x = rotCube[i].x;
            worldCube[i].y = rotCube[i].y * cosf( Pitch ) - rotCube[i].z * sinf( Pitch ) ;
            worldCube[i].z = rotCube[i].y * sinf( Pitch ) + rotCube[i].z * cosf( Pitch );

        }

        // Ortographic Projection  //

        for ( int i = 0; i < 8; i++ ) 
        {
            //400 = (ScreenWidth, ScreenHeight)
            projCube[i].x = worldCube[i].x + 400 * 0.5f;
            projCube[i].y = worldCube[i].y + 400 * 0.5f;
            projCube[i].z = worldCube[i].z;

        }

        return projCube;
    }

    void sWorld::Terra_Form() 
    {

        //Terrain
        uint64_t seed = rand();

        std::vector<f_vNoise::Octave> s_Octos;
        std::vector<f_vNoise::Combined> com_octos;
        float t_Height;


        for(int i = 0; i < 6; i ++) 
        {
            f_vNoise::Octave n(seed, 2, i);
            s_Octos.push_back(n);

        }
        for(int i = 1; i < 3; i ++) 
        {

            f_vNoise::Combined m(s_Octos[i + i], s_Octos[(i + i) + 1]);
            com_octos.push_back(m);

        }

        for( int y = 0; y < size.y * Chunks_Sizes.y; y ++ ) 
        {

            for( int x = 0; x < size.x * Chunks_Sizes.x; x ++ )
            {
                
                //-- We Can Access Properties Because Function Return A Refrence & --//
                GetCell({x, y} ).bWall = true;

                for( int z = 0; z < size.z; z ++ ) 
                {   


                    block& c_cell = GetCell({x, y, z} );

                    t_Height += (com_octos[0].sample({(int)(x * 0.0065f), int(y * 0.0065f)})) * 0.095f;
                    t_Height += (com_octos[1].sample({(int)(x * 0.5f), int(y)})) * 0.00095f;
                    t_Height += (com_octos[0].sample({(int)(y * 2), int(x * 2)})) * 0.0035f;
                    t_Height += (com_octos[1].sample({(int)(x * 2), int(y * 4)})) * 0.0035f;

                    int Y_Surface = 4 + t_Height;

                    if(z < Y_Surface)
                    {                    
                        c_cell.bWall = true;
                        c_cell.bisAir = false;
                    }

                    if(z < Y_Surface - 1)
                    {                    
                        c_cell.Block_Type = Blocks_Types::Dirt;

                        c_cell.id[ Face::Floor ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y};
                        c_cell.id[ Face::Top   ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y};
                        c_cell.id[ Face::North ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y };
                        c_cell.id[ Face::South ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y };
                        c_cell.id[ Face::West  ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y };
                        c_cell.id[ Face::East  ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y };

                    }


                    if(z < Y_Surface - 2)
                    {                    
                        c_cell.Block_Type = Blocks_Types::Stone;

                        c_cell.id[ Face::Floor ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y};
                        c_cell.id[ Face::Top   ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y};
                        c_cell.id[ Face::North ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y };
                        c_cell.id[ Face::South ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y };
                        c_cell.id[ Face::West  ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y };
                        c_cell.id[ Face::East  ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y };

                    }


                    if(c_cell.bWall == 1)
                    {
                        
                        c_cell.bisAir = false;

                    }

                }

            }
        } 



    }

    void sWorld::Initialize () 
    {


        for( int y = 0; y < size.y * Chunks_Sizes.y; y ++ ) 
        {

            for( int x = 0; x < size.x * Chunks_Sizes.x; x ++ )
            {

                for( int z = 0; z < size.z; z ++ ) 
                {

                    //-- We Can Access Properties Because Function Return A Refrence & --//
                    block& c_block = GetCell( {x, y, z} );
                    c_block.bWall = false;

                    c_block.Block_Type = 0;

                    c_block.id[ Face::Floor ] = vi2d{ 0, 0 };
                    c_block.id[ Face::Top   ] = vi2d{ 0, 0 };
                    c_block.id[ Face::North ] = vi2d{ 4 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 4 * TileSize.x, 0 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 4 * TileSize.x, 3 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 4 * TileSize.x, 2 * TileSize.y };

                }

            }

        } 

    }

    void sWorld::place (vi2d vTileCursor, vecint3D & vCursor) 
    {

            if(vTileCursor.x == 0 && vTileCursor.y == 0)
                GetCell( vCursor ).Block_Type = Blocks_Types::Grass;
            if(vTileCursor.x == 1 && vTileCursor.y == 0)
                GetCell( vCursor ).Block_Type = Blocks_Types::Dirt;               
            if(vTileCursor.x == 2 && vTileCursor.y == 0)
                GetCell( vCursor ).Block_Type = Blocks_Types::Diamond;
            if(vTileCursor.x == 3 && vTileCursor.y == 0 || vTileCursor.x == 3 && vTileCursor.y == 1)
                GetCell( vCursor ).Block_Type = Blocks_Types::Wood_log;
            if(vTileCursor.x == 0 && vTileCursor.y == 1)
                GetCell( vCursor ).Block_Type = Blocks_Types::Stone;
            if(vTileCursor.x == 1 && vTileCursor.y == 1)
                GetCell( vCursor ).Block_Type = Blocks_Types::Oak_Plank;
            if(vTileCursor.x == 2 && vTileCursor.y == 1)
                GetCell( vCursor ).Block_Type = Blocks_Types::Andesite;
            if(vTileCursor.x == 0 && vTileCursor.y == 2)
                GetCell( vCursor ).Block_Type = Blocks_Types::Gold_Block;
            if(vTileCursor.x == 1 && vTileCursor.y == 2)
                GetCell( vCursor ).Block_Type = Blocks_Types::Cobblestone;
            if(vTileCursor.x == 2 && vTileCursor.y == 2)
                GetCell( vCursor ).Block_Type = Blocks_Types::Brick;
            if(vTileCursor.x == 3 && vTileCursor.y == 2)
                GetCell( vCursor ).Block_Type = Blocks_Types::Leaf;
            if(vTileCursor.x == 0 && vTileCursor.y == 3)
                GetCell( vCursor ).Block_Type = Blocks_Types::Glass;
            if(vTileCursor.x == 3 && vTileCursor.y == 3)
                GetCell( vCursor ).Block_Type = Blocks_Types::Spruce_Plank;
            if(vTileCursor.x == 0 && vTileCursor.y == 4)
                GetCell( vCursor ).Block_Type = Blocks_Types::Stone_Brick;
            if(vTileCursor.x == 0 && vTileCursor.y == 5)
                GetCell( vCursor ).Block_Type = Blocks_Types::Broken_Stone_Brick;
            if(vTileCursor.x == 3 && vTileCursor.y == 5)
                GetCell( vCursor ).Block_Type = Blocks_Types::Grave;
            if(vTileCursor.x == 1 && vTileCursor.y == 5)
                GetCell( vCursor ).Block_Type = Blocks_Types::Sand;
            if(vTileCursor.x == 2 && vTileCursor.y == 5)
                GetCell( vCursor ).Block_Type = Blocks_Types::Sand_Brick;
            if(vTileCursor.x == 3 && vTileCursor.y == 4)
                GetCell( vCursor ).Block_Type = Blocks_Types::Water;

            block& c_block = GetCell(vCursor );

            switch( c_block.Block_Type )
            {

                case Blocks_Types::Grass:

                    c_block.id[ Face::Floor ] = vi2d{ 0, 0 };
                    c_block.id[ Face::Top   ] = vi2d{ 0, 0 };
                    c_block.id[ Face::North ] = vi2d{ 4 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 4 * TileSize.x, 0 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 4 * TileSize.x, 3 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 4 * TileSize.x, 2 * TileSize.y };
                    break;
                case Blocks_Types::Dirt:

                    c_block.id[ Face::Floor ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 1 * TileSize.x, 0 * TileSize.y };
                    break;
                case Blocks_Types::Diamond:

                    c_block.id[ Face::Floor ] = vi2d{ 2 * TileSize.x, 0 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 2 * TileSize.x, 0 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 2 * TileSize.x, 0 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 2 * TileSize.x, 0 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 2 * TileSize.x, 0 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 2 * TileSize.x, 0 * TileSize.y };
                    break;
                case Blocks_Types::Wood_log:

                    c_block.id[ Face::Floor ] = vi2d{ 3 * TileSize.x, 1 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 3 * TileSize.x, 0 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 3 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 3 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 3 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 3 * TileSize.x, 1 * TileSize.y };
                    break;
                case Blocks_Types::Stone:

                    c_block.id[ Face::Floor ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 0 * TileSize.x, 1 * TileSize.y };
                    break;
                case Blocks_Types::Oak_Plank:

                    c_block.id[ Face::Floor ] = vi2d{ 1 * TileSize.x, 1 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 1 * TileSize.x, 1 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 1 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 1 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 1 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 1 * TileSize.x, 1 * TileSize.y };
                    break;
                case Blocks_Types::Andesite:

                    c_block.id[ Face::Floor ] = vi2d{ 2 * TileSize.x, 1 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 2 * TileSize.x, 1 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 2 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 2 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 2 * TileSize.x, 1 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 2 * TileSize.x, 1 * TileSize.y };
                    break;
                case Blocks_Types::Gold_Block:

                    c_block.id[ Face::Floor ] = vi2d{ 0 * TileSize.x, 2 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 0 * TileSize.x, 2 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 0 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 0 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 0 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 0 * TileSize.x, 2 * TileSize.y };
                    break;
                case Blocks_Types::Cobblestone:

                    c_block.id[ Face::Floor ] = vi2d{ 1 * TileSize.x, 2 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 1 * TileSize.x, 2 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 1 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 1 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 1 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 1 * TileSize.x, 2 * TileSize.y };
                    break;
                case Blocks_Types::Brick:

                    c_block.id[ Face::Floor ] = vi2d{ 2 * TileSize.x, 2 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 2 * TileSize.x, 2 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 2 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 2 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 2 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 2 * TileSize.x, 2 * TileSize.y };
                    break;
                case Blocks_Types::Leaf:

                    c_block.id[ Face::Floor ] = vi2d{ 3 * TileSize.x, 2 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 3 * TileSize.x, 2 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 3 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 3 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 3 * TileSize.x, 2 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 3 * TileSize.x, 2 * TileSize.y };
                    break;
                case Blocks_Types::Glass:

                    c_block.id[ Face::Floor ] = vi2d{ 0 * TileSize.x, 3 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 0 * TileSize.x, 3 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 0 * TileSize.x, 3 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 0 * TileSize.x, 3 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 0 * TileSize.x, 3 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 0 * TileSize.x, 3 * TileSize.y };
                    break;
                case Blocks_Types::Spruce_Plank:

                    c_block.id[ Face::Floor ] = vi2d{ 3 * TileSize.x, 3 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 3 * TileSize.x, 3 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 3 * TileSize.x, 3 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 3 * TileSize.x, 3 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 3 * TileSize.x, 3 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 3 * TileSize.x, 3 * TileSize.y };
                    break;
                case Blocks_Types::Stone_Brick:

                    c_block.id[ Face::Floor ] = vi2d{ 0 * TileSize.x, 4 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 0 * TileSize.x, 4 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 0 * TileSize.x, 4 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 0 * TileSize.x, 4 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 0 * TileSize.x, 4 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 0 * TileSize.x, 4 * TileSize.y };
                    break;
                case Blocks_Types::Broken_Stone_Brick:

                    c_block.id[ Face::Floor ] = vi2d{ 0 * TileSize.x, 5 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 0 * TileSize.x, 5 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 0 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 0 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 0 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 0 * TileSize.x, 5 * TileSize.y };
                    break;
                case Blocks_Types::Grave:

                    c_block.id[ Face::Floor ] = vi2d{ 3 * TileSize.x, 5 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 3 * TileSize.x, 5 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 3 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 3 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 3 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 3 * TileSize.x, 5 * TileSize.y };
                    break;
                case Blocks_Types::Sand:

                    c_block.id[ Face::Floor ] = vi2d{ 1 * TileSize.x, 5 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 1 * TileSize.x, 5 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 1 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 1 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 1 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 1 * TileSize.x, 5 * TileSize.y };
                    break;
                case Blocks_Types::Water:

                    c_block.id[ Face::Floor ] = vi2d{ 3 * TileSize.x, 4 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 3 * TileSize.x, 4 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 3 * TileSize.x, 4 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 3 * TileSize.x, 4 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 3 * TileSize.x, 4 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 3 * TileSize.x, 4 * TileSize.y };
                    break;
                case Blocks_Types::Sand_Brick:

                    c_block.id[ Face::Floor ] = vi2d{ 2 * TileSize.x, 5 * TileSize.y};
                    c_block.id[ Face::Top   ] = vi2d{ 2 * TileSize.x, 5 * TileSize.y};
                    c_block.id[ Face::North ] = vi2d{ 2 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::South ] = vi2d{ 2 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::West  ] = vi2d{ 2 * TileSize.x, 5 * TileSize.y };
                    c_block.id[ Face::East  ] = vi2d{ 2 * TileSize.x, 5 * TileSize.y };
                    break;
            }

    }