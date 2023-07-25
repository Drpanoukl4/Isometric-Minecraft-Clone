#include "perlin.h"
#include "noise1234.h"

Noise n_nois;

using namespace f_vNoise;

double Octave::sample (vi2d pos) const
{

    double u = 1.0f, v = 0.0f;

    for( double x = 0; x < n; x++)
    {   

        v += n_nois.noise3( pos.x / u, pos.y / u, seed + x + (o_samp * 32) * u );
        u *= 2.0f;

    }

    return v;

}

double Combined::sample (vi2d pos) const
{

    return n_1->sample ({ (int)(pos.x + m_2->sample(pos)), pos.y });

}