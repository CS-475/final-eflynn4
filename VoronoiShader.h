#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"
#include "include/GPixel.h"
#include "include/GPoint.h"
#include "include/GColor.h"
#include <iostream>
#include <typeinfo>
#include "stdlib.h"

class VoronoiShader : public GShader {
    public:
        VoronoiShader(const GPoint ps[], const GColor cs[], int c) : count(c) {
            for (int i=0; i<count; i++) {
                colors.push_back(cs[i]);
                points.push_back(ps[i]);
            }
        }

        void shadeRow(int x, int y, int len, GPixel out[]) override {
            int currentX = x;
            int currentY = y;

            for (int z = 0; z < len; z++) {
                GPoint dstPix;
                dstPix.x = currentX;
                dstPix.y = currentY;

                GPoint srcPix = inv * dstPix;
                
                double currentDist = 99999999999;
                GColor currentColor;

                for (int i = 0; i < points.size(); i++) {

                    GPoint p = points[i];

                    double dist = std::sqrt(((p.x-srcPix.x)*(p.x-srcPix.x))+((p.y-srcPix.y)*(p.y-srcPix.y)));

                    if (dist < currentDist) {
                        currentDist = dist;
                        currentColor = colors[i];
                    }
                }

                out[z] = color2Pix(currentColor);
                currentX++;
            }
        }

        bool isOpaque() override { return true; }

        bool setContext(const GMatrix& ctm) override {
            mat = ctm;
            if (auto inverted = ctm.invert()) {
                inv = *inverted;
                return true;
            }
            return false;
        }


        GPixel color2Pix(const GColor& color) {

            unsigned int newA = GRoundToInt(color.a * 255);
            unsigned int newR = GRoundToInt(color.r * color.a * 255);
            unsigned int newG = GRoundToInt(color.g * color.a * 255);
            unsigned int newB = GRoundToInt(color.b * color.a * 255);

            return GPixel_PackARGB(newA, newR, newG, newB);
        }

    private:
        GMatrix mat;
        GMatrix inv;
        std::vector<GPoint> points;
        std::vector<GColor> colors;
        int count;
};