#include <cstdint>
#include ".././World/block.h"

namespace f_vNoise 
{

    struct f_Noise 
    {

        virtual double sample (vi2d pos) const = 0;

    };

    struct Octave : f_Noise
    {

        uint64_t seed;
        uint32_t o_samp;
        uint32_t n;

        Octave( uint64_t f_seed, uint32_t f_o_samp, uint32_t f_n) : seed(f_seed), o_samp(f_o_samp), n(f_n){}
        double sample (vi2d pos) const override;

    };

    struct Combined : f_Noise
    {

        f_Noise *n_1;
        f_Noise *m_2;
        uint32_t n;

        Combined( f_Noise &f_n, f_Noise &f_m) : n_1(&f_n), m_2(&f_m){}
        double sample (vi2d pos) const override;

    };

};