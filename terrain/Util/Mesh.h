#include "../World/terrain.h"

float fDistance ( float ax, float ay, float bx, float by ) 
{

    return sqrtf( (bx - ax) * (bx - ax) + (by - ay) * (by - ay) );

}

void CalculateVisibleFace ( std::array < vec3D, 8 > &Cube, sWorld & m ) 
{   

    auto CheckNormals  = [&] ( int v1, int v2, int v3 ) 
    {

        vf2d a = { Cube[v1].x, Cube[v1].y };
        vf2d b = { Cube[v2].x, Cube[v2].y };
        vf2d c = { Cube[v3].x, Cube[v3].y };

        vf2d vAB = { b.x - a.x, b.y - a.y};
        vf2d vCB = { c.x - a.x, c.y - a.y};

        float normal = (vAB.x * vCB.y) - (vAB.y * vCB.x);

        return normal > 0;

    };

    m.bVisible [ Face::Floor ] = CheckNormals( 4, 0, 1 );
    m.bVisible [ Face::South ] = CheckNormals( 3, 0, 1 );
    m.bVisible [ Face::North ] = CheckNormals( 6, 5, 4 );
    m.bVisible [ Face::East ] = CheckNormals( 7, 4, 0 );
    m.bVisible [ Face::West ] = CheckNormals( 2, 1, 5 );
    m.bVisible [ Face::Top ] = CheckNormals( 7, 3, 2 );

}

void Update_Mesh ( vecint3D size, sWorld &mWorld, std::vector< sQuad >& sQuads ) 
{   

    for( int y = 0; y < size.y * mWorld.Chunks_Sizes.y; y ++ ) 
    {

        for( int x = 0; x < size.x * mWorld.Chunks_Sizes.x; x ++ )
        {

            mWorld.getFaceQuad({ x, y } , mWorld.fOfset.x, mWorld.fOfset.y, mWorld.fCameraZoom, { mWorld.Camera.x, mWorld.Camera.z, mWorld.Camera.y }, sQuads, 0, true );

            for( int h = 0; h < size.z; h ++ ) 
            {
                
                if( mWorld.GetCell( {x, y, h} ).bisAir == false ) 
                {

                    mWorld.getFaceQuad({ x, y, h } , mWorld.fOfset.x, mWorld.fOfset.y, mWorld.fCameraZoom, { mWorld.Camera.x, mWorld.Camera.z, mWorld.Camera.y }, sQuads, h, true );

                }


            }

        }

    }

    //  **- Draw In Correct order -** From Farthest away to Closest//

    std::sort( sQuads.begin(), sQuads.end(), []( const sQuad &q1, const sQuad &q2 ) 
    {

        float z1 = ( q1.points[0].z + q1.points[1].z + q1.points[2].z + q1.points[3].z ) * 0.25f;
        float z2 = ( q2.points[0].z + q2.points[1].z + q2.points[2].z + q2.points[3].z ) * 0.25f;

        return z1 < z2;

    } );

}

void Select_Mouse ( vecint3D& cursor, vi2d vMouse, sWorld &mWorld, std::vector< sQuad >& sQuads ) 
{

    std::vector< select_t > select_quad;
    float dis_b;

    for ( auto &n : sQuads) 
    {

       for(int i = 0; i < 4; i ++) 
       {

            dis_b += fDistance ( vMouse.x, vMouse.y, n.points[i].x, n.points[i].y );    

       }

       select_quad.push_back( {dis_b, n.v_id} );
       dis_b = 0;

    }
    
    std::sort( select_quad.begin(), select_quad.end(), []( const select_t &q1, const select_t &q2 ) 
    {

        return q2.f_value > q1.f_value;

    } );

    cursor = select_quad[0].cell;

}
