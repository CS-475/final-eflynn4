#include "include/GFinal.h"
#include <iostream>
#include "VoronoiShader.h"
#include "GradientShader2.h"


class MyFinal : public GFinal {
    /**
     * The vornoi shader is defined by an array of points, each with an associated color.
     * The color any any (x,y) is the color of the closest point from the array.
     */
    virtual std::shared_ptr<GShader> createVoronoiShader(const GPoint points[],
                                                         const GColor colors[],
                                                         int count) {

        return std::unique_ptr<GShader>(new VoronoiShader(points, colors, count));;
    }

      virtual std::shared_ptr<GShader> createLinearPosGradient(GPoint p0, GPoint p1,
                                                             const GColor colors[],
                                                             const float pos[],
                                                             int count) {
                                                                
        return std::unique_ptr<GShader>(new Gradient2(p0, p1, colors, pos, count));;
    }

};

std::unique_ptr<GFinal> GCreateFinal() {
    return std::unique_ptr<MyFinal>(new MyFinal());
}
