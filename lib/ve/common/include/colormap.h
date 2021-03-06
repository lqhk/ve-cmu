/*
 * Copyright (c) 2011. Shohei NOBUHARA, Kyoto University and Carnegie
 * Mellon University. This code may be used, distributed, or modified
 * only for research purposes or under license from Kyoto University or
 * Carnegie Mellon University. This notice must be retained in all copies.
 */
#ifndef TOOLS_COLORMAP_H
#define TOOLS_COLORMAP_H

namespace Tools {
  namespace JetColorMap {
    template<typename T2> inline void x2rgb(float x, float x_min, float x_max, T2 * r, T2 * g, T2 * b) {
      const static int colormap_n = 200;
      const static float colormap[200][3] = {
        {0,0,5.200000e-01},
        {0,0,5.400000e-01},
        {0,0,5.600000e-01},
        {0,0,5.800000e-01},
        {0,0,6.000000e-01},
        {0,0,6.200000e-01},
        {0,0,6.400000e-01},
        {0,0,6.600000e-01},
        {0,0,6.800000e-01},
        {0,0,7.000000e-01},
        {0,0,7.200000e-01},
        {0,0,7.400000e-01},
        {0,0,7.600000e-01},
        {0,0,7.800000e-01},
        {0,0,8.000000e-01},
        {0,0,8.200000e-01},
        {0,0,8.400000e-01},
        {0,0,8.600000e-01},
        {0,0,8.800000e-01},
        {0,0,9.000000e-01},
        {0,0,9.200000e-01},
        {0,0,9.400000e-01},
        {0,0,9.600000e-01},
        {0,0,9.800000e-01},
        {0,0,1},
        {0,2.000000e-02,1},
        {0,4.000000e-02,1},
        {0,6.000000e-02,1},
        {0,8.000000e-02,1},
        {0,1.000000e-01,1},
        {0,1.200000e-01,1},
        {0,1.400000e-01,1},
        {0,1.600000e-01,1},
        {0,1.800000e-01,1},
        {0,2.000000e-01,1},
        {0,2.200000e-01,1},
        {0,2.400000e-01,1},
        {0,2.600000e-01,1},
        {0,2.800000e-01,1},
        {0,3.000000e-01,1},
        {0,3.200000e-01,1},
        {0,3.400000e-01,1},
        {0,3.600000e-01,1},
        {0,3.800000e-01,1},
        {0,4.000000e-01,1},
        {0,4.200000e-01,1},
        {0,4.400000e-01,1},
        {0,4.600000e-01,1},
        {0,4.800000e-01,1},
        {0,5.000000e-01,1},
        {0,5.200000e-01,1},
        {0,5.400000e-01,1},
        {0,5.600000e-01,1},
        {0,5.800000e-01,1},
        {0,6.000000e-01,1},
        {0,6.200000e-01,1},
        {0,6.400000e-01,1},
        {0,6.600000e-01,1},
        {0,6.800000e-01,1},
        {0,7.000000e-01,1},
        {0,7.200000e-01,1},
        {0,7.400000e-01,1},
        {0,7.600000e-01,1},
        {0,7.800000e-01,1},
        {0,8.000000e-01,1},
        {0,8.200000e-01,1},
        {0,8.400000e-01,1},
        {0,8.600000e-01,1},
        {0,8.800000e-01,1},
        {0,9.000000e-01,1},
        {0,9.200000e-01,1},
        {0,9.400000e-01,1},
        {0,9.600000e-01,1},
        {0,9.800000e-01,1},
        {0,1,1},
        {2.000000e-02,1,9.800000e-01},
        {4.000000e-02,1,9.600000e-01},
        {6.000000e-02,1,9.400000e-01},
        {8.000000e-02,1,9.200000e-01},
        {1.000000e-01,1,9.000000e-01},
        {1.200000e-01,1,8.800000e-01},
        {1.400000e-01,1,8.600000e-01},
        {1.600000e-01,1,8.400000e-01},
        {1.800000e-01,1,8.200000e-01},
        {2.000000e-01,1,8.000000e-01},
        {2.200000e-01,1,7.800000e-01},
        {2.400000e-01,1,7.600000e-01},
        {2.600000e-01,1,7.400000e-01},
        {2.800000e-01,1,7.200000e-01},
        {3.000000e-01,1,7.000000e-01},
        {3.200000e-01,1,6.800000e-01},
        {3.400000e-01,1,6.600000e-01},
        {3.600000e-01,1,6.400000e-01},
        {3.800000e-01,1,6.200000e-01},
        {4.000000e-01,1,6.000000e-01},
        {4.200000e-01,1,5.800000e-01},
        {4.400000e-01,1,5.600000e-01},
        {4.600000e-01,1,5.400000e-01},
        {4.800000e-01,1,5.200000e-01},
        {5.000000e-01,1,5.000000e-01},
        {5.200000e-01,1,4.800000e-01},
        {5.400000e-01,1,4.600000e-01},
        {5.600000e-01,1,4.400000e-01},
        {5.800000e-01,1,4.200000e-01},
        {6.000000e-01,1,4.000000e-01},
        {6.200000e-01,1,3.800000e-01},
        {6.400000e-01,1,3.600000e-01},
        {6.600000e-01,1,3.400000e-01},
        {6.800000e-01,1,3.200000e-01},
        {7.000000e-01,1,3.000000e-01},
        {7.200000e-01,1,2.800000e-01},
        {7.400000e-01,1,2.600000e-01},
        {7.600000e-01,1,2.400000e-01},
        {7.800000e-01,1,2.200000e-01},
        {8.000000e-01,1,2.000000e-01},
        {8.200000e-01,1,1.800000e-01},
        {8.400000e-01,1,1.600000e-01},
        {8.600000e-01,1,1.400000e-01},
        {8.800000e-01,1,1.200000e-01},
        {9.000000e-01,1,1.000000e-01},
        {9.200000e-01,1,8.000000e-02},
        {9.400000e-01,1,6.000000e-02},
        {9.600000e-01,1,4.000000e-02},
        {9.800000e-01,1,2.000000e-02},
        {1,1,0},
        {1,9.800000e-01,0},
        {1,9.600000e-01,0},
        {1,9.400000e-01,0},
        {1,9.200000e-01,0},
        {1,9.000000e-01,0},
        {1,8.800000e-01,0},
        {1,8.600000e-01,0},
        {1,8.400000e-01,0},
        {1,8.200000e-01,0},
        {1,8.000000e-01,0},
        {1,7.800000e-01,0},
        {1,7.600000e-01,0},
        {1,7.400000e-01,0},
        {1,7.200000e-01,0},
        {1,7.000000e-01,0},
        {1,6.800000e-01,0},
        {1,6.600000e-01,0},
        {1,6.400000e-01,0},
        {1,6.200000e-01,0},
        {1,6.000000e-01,0},
        {1,5.800000e-01,0},
        {1,5.600000e-01,0},
        {1,5.400000e-01,0},
        {1,5.200000e-01,0},
        {1,5.000000e-01,0},
        {1,4.800000e-01,0},
        {1,4.600000e-01,0},
        {1,4.400000e-01,0},
        {1,4.200000e-01,0},
        {1,4.000000e-01,0},
        {1,3.800000e-01,0},
        {1,3.600000e-01,0},
        {1,3.400000e-01,0},
        {1,3.200000e-01,0},
        {1,3.000000e-01,0},
        {1,2.800000e-01,0},
        {1,2.600000e-01,0},
        {1,2.400000e-01,0},
        {1,2.200000e-01,0},
        {1,2.000000e-01,0},
        {1,1.800000e-01,0},
        {1,1.600000e-01,0},
        {1,1.400000e-01,0},
        {1,1.200000e-01,0},
        {1,1.000000e-01,0},
        {1,8.000000e-02,0},
        {1,6.000000e-02,0},
        {1,4.000000e-02,0},
        {1,2.000000e-02,0},
        {1,0,0},
        {9.800000e-01,0,0},
        {9.600000e-01,0,0},
        {9.400000e-01,0,0},
        {9.200000e-01,0,0},
        {9.000000e-01,0,0},
        {8.800000e-01,0,0},
        {8.600000e-01,0,0},
        {8.400000e-01,0,0},
        {8.200000e-01,0,0},
        {8.000000e-01,0,0},
        {7.800000e-01,0,0},
        {7.600000e-01,0,0},
        {7.400000e-01,0,0},
        {7.200000e-01,0,0},
        {7.000000e-01,0,0},
        {6.800000e-01,0,0},
        {6.600000e-01,0,0},
        {6.400000e-01,0,0},
        {6.200000e-01,0,0},
        {6.000000e-01,0,0},
        {5.800000e-01,0,0},
        {5.600000e-01,0,0},
        {5.400000e-01,0,0},
        {5.200000e-01,0,0},
        {5.000000e-01,0,0},
      };

      int idx = static_cast<int>(colormap_n * (x-x_min) / x_max);
      if(idx<0) {
        *r = 0.5;
        *g = 0.5;
        *b = 0.5;
      } else if(idx>=colormap_n) {
        *r = 1;
        *g = 1;
        *b = 1;
      } else {
        *r = colormap[idx][0];
        *g = colormap[idx][1];
        *b = colormap[idx][2];
      }
    }

    template<> inline void x2rgb<unsigned char>(float x, float x_min, float x_max, unsigned char * r, unsigned char * g, unsigned char * b) {
      float R, G, B;
      x2rgb(x, x_min, x_max, &R, &G, &B);
      *r = R*255;
      *g = G*255;
      *b = B*255;
    }
  }
}

#endif //TOOLS_COLORMAP_H
