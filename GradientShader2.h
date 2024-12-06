#include "include/GShader.h"
#include "include/GBitmap.h"
#include "include/GMatrix.h"
#include "include/GPixel.h"
#include "include/GPoint.h"
#include "include/GColor.h"
#include <iostream>

class Gradient2 : public GShader {
    public:
        Gradient2(GPoint p0, GPoint p1, const GColor colors[], const float pos[], int count) {

            for (int i=0; i<count; i++) {
                flatGradient.push_back(colors[i]);
                positions.push_back(pos[i]);
            }

            gradMat = GMatrix(p1.x-p0.x, -(p1.y-p0.y), p0.x,
                              p1.y-p0.y, p1.x-p0.x, p0.y);
        }

        bool setContext(const GMatrix& ctm) override {
            if (auto inverted = (ctm * gradMat).invert()) {
                fInverse = *inverted;
                return true;
            }
            return false;
        }
        bool isOpaque() override {
        }
        void shadeRow(int x, int y, int count, GPixel out[]) override {
            GPoint dstPix;
            GPoint srcPix;
            int index;
            float t;
            GColor c;

            for (int i = 0; i < count; i++) {

                dstPix.x = x + i + (0.5f);
                dstPix.y = y + (0.5f);

                srcPix = fInverse * dstPix;

                srcPix.x = (srcPix.x > 1) ? 1 : srcPix.x;
                srcPix.x = (srcPix.x < 0) ? 0 : srcPix.x;

                GColor color1, color2;
                float position1, position2;

                if (srcPix.x == 0) {
                    out[i] = color2Pix(flatGradient[0]);
                }
                else if (srcPix.x == 1) {
                    out[i] = color2Pix(flatGradient[flatGradient.size()-1]);
                }
                else {
                    for (int i = 0; i < positions.size(); i++) {
                        if (positions[i] > srcPix.x) {
                            position1 = positions[i-1];
                            position2 = positions[i];
                            color1 = flatGradient[i-1];
                            color2 = flatGradient[i];
                            break;
                        }
                    }

                    assert(position1 <= srcPix.x);
                    assert(position2 >= srcPix.x);

                    float leng = position2-position1;
                    float newpos = srcPix.x-position1;

                    index = floor(srcPix.x * (flatGradient.size()-1));
                    t = newpos/leng;
                    c = (1 - t)*color1 + (t*color2);
                    out[i] = color2Pix(c);
                }
            }
        }

        GPixel color2Pix(const GColor& color) {

            unsigned int newA = GRoundToInt(color.a * 255);
            unsigned int newR = GRoundToInt(color.r * color.a * 255);
            unsigned int newG = GRoundToInt(color.g * color.a * 255);
            unsigned int newB = GRoundToInt(color.b * color.a * 255);

            return GPixel_PackARGB(newA, newR, newG, newB);
        }


    private:
        std::vector<GColor> flatGradient;
        std::vector<float> positions;
        GMatrix gradMat;
        GMatrix fInverse;
        GTileMode tileMode;
};

std::shared_ptr<GShader> GCreateGradient2(GPoint p0, GPoint p1, const GColor colors[], int count, const float pos[]) {
    if (count < 1) {
        return nullptr;
    }

    return std::unique_ptr<GShader>(new Gradient2(p0, p1, colors, pos, count));
}