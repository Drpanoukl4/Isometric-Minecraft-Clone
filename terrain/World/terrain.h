
#include ".././noise/perlin.h"

struct chunk 
{

    vi2d pos;
    std::vector<block> blocks;

};

class sWorld 
{

    public:

        bool bVisible[6];

        sWorld () 
        {

        

        }
        

        void Create ( int Height, int Width, int Depth );

        block &GetCell ( const vecint3D& v ); 
        chunk &GetChunk ( const vi2d& v );

        bool is_Air_Face ( int x, int y, int z ); 

        void getFaceQuad( const vecint3D  &vCell, const float fAngle, const float Pitch, const float fScale, const vec3D &Camera, std::vector < sQuad >& render, int Height, const bool bBlock);
        std::array < vec3D, 8 > CreateCube ( const vecint3D & vCell, const float fAngle, const float Pitch, const float fScale, const vec3D &Camera, int Height, const bool bBlock);

        void Terra_Form();
        void Initialize ();

        void place ( vi2d vTileCursor, vecint3D & vCursor );

    public:

        vecint3D size;
        //Camera//

        vec3D Camera = { 0.0f, 0.0f, 0.0f };
        //float fCameraAngle = 0.0f;
        //float fCameraPitch = 5.0f;
        float fCameraZoom = 16.0f;

        vi2d TileSize = {16, 16};

        //Number of Chunks//
        vi2d Chunks_Sizes = { 2, 2};
        vi2d Chunk_Const = { 8, 8 };
        
        // Chunks_Sizes * 2 ==> Chunk_Const / 2   //
        // Chunks_Sizes == 1 ==> Chunk_Const = 16 //

                //       _______________     //
                //      /             / |    //
                //     /             /  |    //
                //    /_____________/  /|    // 
                //    |             | / |    //
                //    |___        __|/  /    //
                //    |   |____ _|  |  /     //
                //    |             | /      //
                //    |_____________|/       //
        
        //Camera Angle
        vf2d fOfset = {5.9, 5.8};

    private:    

        //std::vector<block> World;
        std::vector<chunk> s_Chunks;
        block  NullCell;
        chunk  NullChunk;

};
