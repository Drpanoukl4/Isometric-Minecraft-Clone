#include ".././Util/vec.h"

// Backface Culling //

struct block 
{

    bool bWall = false;
    bool bisAir = true;
    int Block_Type;
    vi2d id[6]{  };

};

struct sQuad 
{

    vec3D points[4];
    vi2d tile;
    vecint3D v_id;

};

enum Face 
{

    Floor = 0,
    North = 1,
    East = 2,
    South = 3,
    West = 4,
    Top = 5

};

enum Blocks_Types
{

    Grass = 0,
    Dirt = 1,
    Diamond = 2,
    Wood_log = 3,
    Stone = 4,
    Oak_Plank = 5,
    Andesite = 6,
    Gold_Block = 7,
    Cobblestone = 8,
    Brick = 9,
    Leaf = 10,
    Glass = 11,
    Spruce_Plank = 12,
    Stone_Brick = 13,
    Broken_Stone_Brick = 14,
    Grave = 15,
    Sand = 16,
    Sand_Brick = 17,
    Water = 18,

};

