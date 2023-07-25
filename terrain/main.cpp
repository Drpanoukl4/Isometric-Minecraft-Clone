
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "./Util/Mesh.h"
bool Wireframe = 0;
float y_off = 0;

//float ftargetCameraPitch = 0.0f; // // xD
//float ftargetCameraAngle = 0.0f;

vecint3D vCursor = {0, 0, 0};

//Camera//

//Others
vi2d vTileCursor = {0, 0};
olc::vi2d fSartPan;

class Shadow : public olc::PixelGameEngine
{

public:
    struct Renderable
	{
		Renderable() {}

		void Load(const std::string& sFile)
		{
			sprite = new olc::Sprite(sFile);
			decal = new olc::Decal(sprite);
		}

		~Renderable()
		{
			delete decal;
			delete sprite;
		}

		olc::Sprite* sprite = nullptr;
		olc::Decal* decal = nullptr;
	};

    Renderable RenderAllWall;
    Renderable Select;

    sWorld mWorld;

public:
	Shadow()
	{
		sAppName = "Example";
	}

public:

	bool OnUserCreate() override
	{

        RenderAllWall.Load( "./Assets/Minecraft.png" );
        Select.Load( "./Assets/Selected.png" );

        mWorld.Create( 16, 16, 16 );
        //Cunking 16x16x16//

        mWorld.Initialize();

        mWorld.Terra_Form();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

        vi2d vMouse = { GetMouseX(), GetMouseY() };

		//For panning we need to capture the screen location when the user starts
		//to pan...
		if (GetMouse(0).bPressed)
		{

            fSartPan.x = vMouse.x;
            fSartPan.y = vMouse.y;

		}

		//... as the mouse move, the screen location changes. convert the this screen
		//coordinate change into world coordinates to implement the pan. Simples
		if (GetMouse(0).bHeld)
		{

			mWorld.fOfset.x -= -(vMouse.x - fSartPan.x) * 0.1f * fElapsedTime;
			mWorld.fOfset.y -= (vMouse.y - fSartPan.y) * 0.1f * fElapsedTime;

            if(mWorld.fOfset.x >= 6.2f) mWorld.fOfset.x = 0.0f;
            if(mWorld.fOfset.x < 0.0f) mWorld.fOfset.x = 6.1f;

            if(mWorld.fOfset.y >= 6.2f) mWorld.fOfset.y = 6.2f;
            if(mWorld.fOfset.y < 4.5f) mWorld.fOfset.y = 4.5f;

			//Start the "new" Pan for next epoch
			fSartPan.x = vMouse.x;
			fSartPan.y = vMouse.y;

		}

        //EditMode
        if( GetKey(olc::Key::T).bPressed )  
        {

            Wireframe = !Wireframe;

        }

        if( GetKey(olc::Key::B).bHeld ) 
        {

            Clear(olc::BLACK);
            olc::vi2d vMouse = { GetMouseX(), GetMouseY() };

            if( vTileCursor.x < 0 ) vTileCursor.x = 0;
            if( vTileCursor.y < 0 ) vTileCursor.y = 0;

            if( vTileCursor.x >= 5) vTileCursor.x = 5 -1;
            if( vTileCursor.y >= 6) vTileCursor.y = 6 -1;

            DrawSprite( {0, 0}, RenderAllWall.sprite );
            DrawRect(vTileCursor.x * mWorld.TileSize.x, vTileCursor.y * mWorld.TileSize.y, mWorld.TileSize.x, mWorld.TileSize.y, olc::WHITE );

            if( GetMouse(0).bPressed ) 
            {
                vTileCursor = {vMouse.x / mWorld.TileSize.x, vMouse.y / mWorld.TileSize.y};
            }
            DrawString( ScreenWidth() / 2 / 2, 20, "pos : " + std::to_string(vTileCursor.x) +" "+ std::to_string(vTileCursor.y), olc::WHITE );
            return true;

        }

        if( GetMouseWheel() > 1 ) mWorld.fCameraZoom += 16.0f * fElapsedTime;
        if( GetMouseWheel() < 0 ) mWorld.fCameraZoom -= 16.0f * fElapsedTime;

        //mWorld.fCameraAngle += ( ftargetCameraAngle - mWorld.fCameraAngle ) * fElapsedTime;
        //fCameraPitch += ( ftargetCameraPitch - fCameraPitch ) * fElapsedTime; //xDD

        if( GetKey(olc::Key::LEFT).bPressed ) vCursor.x --;
        if( GetKey(olc::Key::RIGHT).bPressed ) vCursor.x ++;  
        if( GetKey(olc::Key::UP).bPressed ) vCursor.y --;
        if( GetKey(olc::Key::DOWN).bPressed ) vCursor.y ++;

        if( GetKey(olc::Key::P).bReleased ) vCursor.z ++;
        if( GetKey(olc::Key::M).bReleased ) vCursor.z --;


        if( GetKey(olc::Key::R).bReleased ) y_off ++;
        if( GetKey(olc::Key::F).bReleased ) y_off --;

        if( vCursor.x < 0 ) vCursor.x = 0;
        if( vCursor.y < 0 ) vCursor.y = 0;
        if( vCursor.z < 0 ) vCursor.z = 0;

        if( vCursor.x >= mWorld.size.x * mWorld.Chunks_Sizes.x ) vCursor.x = mWorld.size.x * mWorld.Chunks_Sizes.x -1;
        if( vCursor.y >= mWorld.size.y * mWorld.Chunks_Sizes.y ) vCursor.y = mWorld.size.y * mWorld.Chunks_Sizes.y -1;
        if( vCursor.z >= mWorld.size.z ) vCursor.z = mWorld.size.z - 1;

        mWorld.Camera = { vCursor.x + 0.5f,  vCursor.y + 0.5f, y_off };
        mWorld.Camera.x *= mWorld.fCameraZoom; mWorld.Camera.y *= mWorld.fCameraZoom; mWorld.Camera.z *= mWorld.fCameraZoom;

        if( GetKey(olc::Key::SHIFT).bHeld && GetMouse(0).bPressed )
        {   


            mWorld.GetCell(vCursor ).bisAir = !mWorld.GetCell(vCursor ).bisAir ;
            mWorld.GetCell(vCursor ).bWall = !mWorld.GetCell(vCursor ).bWall;

            mWorld.place( vTileCursor, vCursor );

        }

        //Choose wich cube going to Draw
        std::array < vec3D, 8 > cullCube = mWorld.CreateCube( {0, 0}, mWorld.fOfset.x, mWorld.fOfset.y, mWorld.fCameraZoom, { mWorld.Camera.x, mWorld.Camera.z, mWorld.Camera.y }, 0, true );
        CalculateVisibleFace( cullCube, mWorld );

        std::vector < sQuad > sQuads;

        Update_Mesh( mWorld.size, mWorld, sQuads);

        if( GetMouse(1).bPressed ) 
        {

            Select_Mouse ( vCursor, vMouse, mWorld, sQuads );

        }

        Clear(olc::BLACK);

        //-DRAWING-//

        if( !Wireframe ) 
        {

            for( auto & q : sQuads ) 
            {   
                DrawPartialWarpedDecal
                (
                    RenderAllWall.decal,
                    { { q.points[0].x, q.points[0].y }, { q.points[1].x, q.points[1].y }, { q.points[2].x, q.points[2].y }, { q.points[3].x, q.points[3].y } },
                    {(float)q.tile.x, (float)q.tile.y},
                    {(float)mWorld.TileSize.x, (float)mWorld.TileSize.y}

                );

            }

            sQuads.clear();

            mWorld.getFaceQuad( vCursor , mWorld.fOfset.x, mWorld.fOfset.y, mWorld.fCameraZoom, { mWorld.Camera.x, mWorld.Camera.z, mWorld.Camera.y }, sQuads, vCursor.z, false );
        
            for( auto & q : sQuads ) 
                DrawWarpedDecal
                (
                    Select.decal,
                    { { q.points[0].x, q.points[0].y }, { q.points[1].x, q.points[1].y }, { q.points[2].x, q.points[2].y }, { q.points[3].x, q.points[3].y } }

                );

        }else 
        {

            for( auto & q : sQuads ) 
            {

                DrawLine( q.points[0].x, q.points[0].y, q.points[1].x, q.points[1].y, olc::WHITE );
                DrawLine( q.points[1].x, q.points[1].y, q.points[2].x, q.points[2].y, olc::WHITE );
                DrawLine( q.points[2].x, q.points[2].y, q.points[3].x, q.points[3].y, olc::WHITE );
                DrawLine( q.points[3].x, q.points[3].y, q.points[0].x, q.points[0].y, olc::WHITE );

                for( auto &r : q.points )
                {

                    FillCircle( r.x, r.y, 1.0f, olc::Pixel(129, 228, 220) );

                }
    
            }


        }

        DrawString( ScreenWidth() / 2 / 2, 20, "pos : " + std::to_string(vCursor.x) +" "+ std::to_string(vCursor.y) +" "+ std::to_string(vCursor.z), olc::WHITE );
        DrawString( ScreenWidth() / 2 / 2, 30, "air : " + std::to_string(mWorld.GetCell( vCursor).bisAir), olc::WHITE );
        DrawString( ScreenWidth() / 2 / 2, 40, "Chunk : " + std::to_string((vCursor.x / mWorld.Chunk_Const.x) / mWorld.Chunks_Sizes.x) +" "+ std::to_string((vCursor.y / mWorld.Chunk_Const.y ) / mWorld.Chunks_Sizes.y), olc::WHITE );
        DrawString( ScreenWidth() / 2 / 2, 50, "Chunk pos : " + std::to_string(vCursor.x % mWorld.size.x) +" "+ std::to_string(vCursor.y % mWorld.size.y), olc::WHITE );
        DrawString( ScreenWidth() / 2 / 2, 60, "vMouse x : " + std::to_string(mWorld.fOfset.x) + " "+ "y : " + std::to_string(mWorld.fOfset.y), olc::WHITE );

		return true;
	}

};

int main()
{
	Shadow demo;
	if (demo.Construct(400, 400, 4, 4))
		demo.Start();

	return 0;
}