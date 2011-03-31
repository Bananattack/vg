#ifndef VG_GRAPHICS_IMAGE_HPP
#define VG_GRAPHICS_IMAGE_HPP

#include <cmath>
#include <cstring>
#include <algorithm>
#include "blend.hpp"
#include "color.hpp"

namespace vg
{
    class Image
    {
        private:
            int width, height;
            int clipX, clipY, clipX2, clipY2;
            ColorChannel opacity;
            Color* data;

        public:
            Image(int width, int height):
                width(width), height(height),
                data(new Color[width * height]),
                opacity(255)
            {
                clear(ColorBlack);
                resetClip();
            }

            Image(Image* source):
                width(source->width), height(source->height),
                data(new Color[source->width * source->height]),
                opacity(source->opacity)
            {
                source->copyRawData(this);
                source->getClip(clipX, clipY, clipX2, clipY2);
            }

            ~Image()
            {
                delete[] data;
            }

            int getWidth() const
            {
                return width;
            }

            int getHeight() const
            {
                return height;
            }

            Color* getRawData() const
            {
                return data;
            }

            ColorChannel getOpacity() const
            {
                return opacity;
            }

            void setOpacity(ColorChannel opacity)
            {
                this->opacity = opacity;
            }

            int getPixel(int x, int y) const
            {
                if(x >= 0 && x < width && y >= 0 && y < height)
                {
                    return data[y * width + x];
                }
                else
                {
                    return 0;
                }
            }

            int setPixel(int x, int y, Color color)
            {
                if(x >= clipX && x <= clipX2 && y >= clipY && y <= clipY2)
                {
                    data[y * width + x] = color;
                }
            }

            void getClip(int& x, int& y, int& x2, int& y2) const
            {
                x = clipX;
                y = clipY;
                x2 = clipX2;
                y2 = clipY2;
            }

            void resetClip()
            {
                setClip(0, 0, width - 1, height - 1);
            }

            void setClip(int x, int y, int x2, int y2)
            {
                if(x > x2)
                {
                    std::swap(x, x2);
                }
                if(y > y2)
                {
                    std::swap(y, y2);
                }
                clipX = std::min(std::max(0, x), width - 1);
                clipY = std::min(std::max(0, y), height - 1);
                clipX2 = std::min(std::max(0, x2), width - 1);
                clipY2 = std::min(std::max(0, y2), height - 1);
            }

            void copyRawData(Image* dest)
            {
                if(width == dest->width && height == dest->height)
                {
                    std::memcpy(dest->data, data, width * height * sizeof(*data));
                }
            }

            void clear(Color color)
            {
                for(int i = 0; i < width * height; i++)
                {
                    data[i] = color;
                }
            }

            void replaceColor(Color find, Color replacement)
            {
                for(int i = 0; i < width * height; i++)
                {
                    if(data[i] == find)
                    {
                        data[i] = replacement;
                    }
                }
            }

            void flip(bool horizontal, bool vertical)
            {
                if(horizontal)
                {
                    for(int y = 0; y < height; y++)
                    {
                        for(int x = 0; x < width / 2; x++)
                        {
                            std::swap(data[y * width + x], data[y * width + (width - x - 1)]);
                        }
                    }
                }
                if(vertical)
                {
                    for(int y = 0; y < height / 2; y++)
                    {
                        for(int x = 0; x < width; x++)
                        {
                            std::swap(data[y * width + x], data[(height - y - 1) * width + x]);
                        }
                    }
                }
            }

            template<typename BlendFunction> void rect(int x, int y, int x2, int y2, Color color, BlendFunction f)
            {
                // Put the coordinates in order.
                if(x > x2)
                {
                    std::swap(x, x2);
                }
                if(y > y2)
                {
                    std::swap(y, y2);
                }
                // Don't draw if completely outside clipping regions.
                if(x > clipX2 || y > clipY2 || x2 < clipX || y2 < clipY)
                {
                    return;
                }
                // Keep rectangle inside clipping regions
                if(x < clipX)
                {
                    x = clipX;
                }
                if(x2 > clipX2)
                {
                    x2 = clipX2;
                }
                if(y < clipY)
                {
                    y = clipY;
                }
                if(y2 > clipY2)
                {
                    y2 = clipY2;
                }
                // Draw the horizontal lines of a rectangle.
                for(int i = x; i <= x2; i++)
                {
                    data[y * width + i] = f(color, data[y * width + i], opacity);
                    data[y2 * width + i] = f(color, data[y2 * width + i], opacity);
                }
                // Draw the vertical lines of a rectangle.
                for(int i = y; i <= y2; i++)
                {
                    data[i * width + x] = f(color, data[i * width + x], opacity);
                    data[i * width + x2] = f(color, data[i * width + x2], opacity);
                }
            }

            template<typename BlendFunction> void rectFill(int x, int y, int x2, int y2, Color color, BlendFunction f)
            {
                // Put the coordinates in order.
                if(x > x2)
                {
                    std::swap(x, x2);
                }
                if(y > y2)
                {
                    std::swap(y, y2);
                }
                // Don't draw if completely outside clipping regions.
                if(x > clipX2 || y > clipY2 || x2 < clipX || y2 < clipY)
                {
                    return;
                }
                // Keep rectangle inside clipping regions
                if(x < clipX)
                {
                    x = clipX;
                }
                if(x2 > clipX2)
                {
                    x2 = clipX2;
                }
                if(y < clipY)
                {
                    y = clipY;
                }
                if(y2 > clipY2)
                {
                    y2 = clipY2;
                }
                // Draw a filled rectangle
                for(int i = y; i <= y2; i++)
                {
                    for(int j = x; j <= x2; j++)
                    {
                        data[i * width + j] = f(color, data[i * width + j], opacity);
                    }
                }
            }

            template<typename BlendFunction> void line(int x, int y, int x2, int y2, Color color, BlendFunction f)
            {
                enum
                {
                    None = 0,
                    Left = 1,
                    Right = 2,
                    Top = 4,
                    Bottom = 8
                };

                // Now we'll clip the line using Cohen-Sutherland clipping
                // (this source adapted from ika)
                unsigned int c1 = None;
                unsigned int c2 = None;

                // Silly region code things to start us off
                if(x < clipX)   c1 |= Left;
                if(x > clipX2)  c1 |= Right;
                if(y < clipY)   c1 |= Top;
                if(y > clipY2)  c1 |= Bottom;
                if(x2 < clipX)  c2 |= Left;
                if(x2 > clipX2) c2 |= Right;
                if(y2 < clipY)  c2 |= Top;
                if(y2 > clipY2) c2 |= Bottom;

                // Keep clipping until we either accept or reject this line.
                while(!(c1 & c2) && (c1 | c2))
                {
                    if(c1)
                    {
                        // Clip left.
                        if(c1 & Left)
                        {
                            y -= ((y - y2) * (clipX - x)) / (x2 - x + 1);
                            x = clipX;
                        }
                        // Clip right.
                        else if(c1 & Right)
                        {
                            y -= ((y - y2) * (x - clipX2)) / (x2 - x + 1);
                            x = clipX2;
                        }
                        // Clip above.
                        else if(c1 & Top)
                        {
                            x -= ((x - x2) * (clipY - y)) / (y2 - y + 1);
                            y = clipY;
                        }
                        // Clip below.
                        else
                        {
                            x -= ((x - x2) * (y - clipY2)) / (y2 - y + 1);
                            y = clipY2;
                        }
                        // Recheck all the region codes for this side of the line.
                        c1 = None;
                        if(x < clipX)   c1 |= Left;
                        if(x > clipX2)  c1 |= Right;
                        if(y < clipY)   c1 |= Top;
                        if(y > clipY2)  c1 |= Bottom;
                    }
                    else
                    {
                        // Clip left.
                        if(c2 & Left)
                        {
                            y2 -= ((y2 - y) * (clipX - x2)) / (x - x2 + 1);
                            x2 = clipX;
                        }
                        // Clip right.
                        else if(c2 & Right)
                        {
                            y2 -= ((y2 - y) * (x2 - clipX2)) / (x - x2 + 1);
                            x2 = clipX2;
                        }
                        // Clip above.
                        else if(c2 & Top)
                        {
                            x2 -= ((x2 - x) * (clipY - y2)) / (y - y2 + 1);
                            y2 = clipY;
                        }
                        // Clip below.
                        else
                        {
                            x2 -= ((x2 - x) * (y2 - clipY2)) / (y - y2 + 1);
                            y2 = clipY2;
                        }
                        // Recheck all the region codes for this side of the line.
                        c2 = None;
                        if(x2 < clipX)  c2 |= Left;
                        if(x2 > clipX2) c2 |= Right;
                        if(y2 < clipY)  c2 |= Top;
                        if(y2 > clipY2) c2 |= Bottom;
                    }
                }
                // Don't draw if completely outside clipping regions.
                if(c1 & c2)
                {
                    return;
                }
                // A single pixel
                if(x == x2 && y == y2)
                {
                    data[y * width + x] = f(color, data[y * width + x], opacity);
                }
                // Horizontal line
                else if(y == y2)
                {
                    // Put the coordinates in order.
                    if (x > x2)
                    {
                        std::swap(x, x2);
                    }
                    // Draw it.
                    for(int i = x; i <= x2; i++)
                    {
                        data[y * width + i] = f(color, data[y * width + i], opacity);
                    }
                }
                // Vertical line
                else if(x == x2)
                {
                    // Put the coordinates in order.
                    if (y > y2)
                    {
                        std::swap(y, y2);
                    }
                    // Draw it.
                    for(int i = y; i <= y2; i++)
                    {
                        data[i * width + x] = f(color, data[i * width + x], opacity);
                    }
                }
                else
                {
                    // Time for some Bresenham.
                    int differenceX = std::abs(x2 - x);
                    int differenceY = std::abs(y2 - y);
                    int deltaX = (x > x2) ? -1 : 1;
                    int deltaY = (y > y2) ? -1 : 1;

                    int errorX, errorY;
                    int incrementX, incrementY;
                    int resetX, resetY;

                    if(dx > dy)
                    {
                        errorX = 0;
                        errorY = differenceY * 2 - differenceX;
                        incrementX = 0;
                        incrementY = differenceY * 2;
                        resetX = 0;
                        resetY = (differenceY - differenceX) * 2;
                    }
                    else
                    {
                        errorX = differenceX * 2 - differenceY;
                        errorY = 0;
                        incrementX = differenceX * 2;
                        incrementY = 0;
                        resetX = (differenceX - differenceY) * 2;
                        resetY = 0;
                    }

                    do
                    {
                        if(errorX < 0)
                        {
                            errorX += incrementX;
                        }
                        else
                        {
                            x += deltaX;
                            errorX += resetX;
                        }

                        if(errorY < 0)
                        {
                            errorY += incrementY;
                        }
                        else
                        {
                            y += deltaY;
                            errorY += resetY;
                        }
                        data[y * width + x] = f(color, data[y * width + x], opacity);
                    } while((resetX || x != x2) && (resetY || y != y2))
                }
            }

            template<typename BlendFunction> void ellipse(int cx, int cy, int radiusX, int radiusY, Color color, BlendFunction f)
            {
                // Algorithm based on "A Fast Bresenham Type Algorithm For Drawing Ellipses" by John Kennedy <rkennedy@ix.netcom.com>
                int plotX, plotY;

                int lastX = -1;
                int lastY = -1;
                int a = 2 * radiusX * radiusX;
                int b = 2 * radiusY * radiusY;
                int x = radiusX;
                int y = 0;
                int deltaX = radiusY * radiusY * (1 - 2 * radiusX);
                int deltaY = radiusX * radiusX;
                int ellipseError = 0;
                int stoppingX = b * radiusX;
                int stoppingY = 0;

                // First set of points, y' > -1
                while(stoppingX >= stoppingY)
                {
                    if(lastX != x || lastY != y)
                    {
                        int plotY = cy - y;
                        if(plotY >= clipY && plotY <= clipY2)
                        {
                            int plotX = cx - x;
                            if(plotX >= clipX && plotX <= clipX2)
                            {
                                data[plotY * width + plotX] = f(color, data[plotY * width + plotX], opacity);
                            }
                            plotX = cx + x;
                            if(plotX >= clipX && plotX <= clipX2)
                            {
                                data[plotY * width + plotX] = f(color, data[plotY * width + plotX], opacity);
                            }
                        }
                        if(y)
                        {
                            plotY = cy + y;
                            if(plotY >= clipY && plotY <= clipY2)
                            {
                                int plotX = cx - x;
                                if(plotX >= clipX && plotX <= clipX2)
                                {
                                    data[plotY * width + plotX] = f(color, data[plotY * width + plotX], opacity);
                                }
                                plotX = cx + x;
                                if(plotX >= clipX && plotX <= clipX2)
                                {
                                    data[plotY * width + plotX] = f(color, data[plotY * width + plotX], opacity);
                                }
                            }
                        }
                        lastX = x;
                        lastY = y;
                    }

                    y++;
                    stoppingY += a;
                    ellipseError += deltaY;
                    deltaY += a;
                    if(ellipseError * 2 + deltaX > 0)
                    {
                        x--;
                        stoppingX -= b;
                        ellipseError += deltaX;
                        deltaX += b;
                    }
                }

                // First point set is done; start the second set of points
                x = 0;
                y = radiusY;
                deltaX = radiusY * radiusY;
                deltaY = radiusX * radiusX * (1 - radiusY * 2);
                ellipseError = 0;
                stoppingX = 0;
                stoppingY = a * radiusY;

                // Second set of points, y' < -1
                while(stoppingX <= stoppingY)
                {
                    if(lastX != x || lastY != y)
                    {
                        int plotY = cy - y;
                        if(plotY >= clipY && plotY <= clipY2)
                        {
                            int plotX = cx - x;
                            if(plotX >= clipX && plotX <= clipX2)
                            {
                                data[plotY * width + plotX] = f(color, data[plotY * width + plotX], opacity);
                            }
                            if(x)
                            {
                                plotX = cx + x;
                                if(plotX >= clipX && plotX <= clipX2)
                                {
                                    data[plotY * width + plotX] = f(color, data[plotY * width + plotX], opacity);
                                }
                            }
                        }
                        if(y)
                        {
                            plotY = cy + y;
                            if(plotY >= clipY && plotY <= clipY2)
                            {
                                int plotX = cx - x;
                                if(plotX >= clipX && plotX <= clipX2)
                                {
                                    data[plotY * width + plotX] = f(color, data[plotY * width + plotX], opacity);
                                }
                                if(x)
                                {
                                    plotX = cx + x;
                                    if(plotX >= clipX && plotX <= clipX2)
                                    {
                                        data[plotY * width + plotX] = f(color, data[plotY * width + plotX], opacity);
                                    }
                                }
                            }
                        }
                        lastX = x;
                        lastY = y;
                    }

                    x++;
                    ellipseError += deltaX;
                    deltaX += b;
                    stoppingX += b;
                    if(ellipseError * 2 + deltaY > 0)
                    {
                        y--;
                        ellipseError += deltaY;
                        deltaY += a;
                        stoppingY -= a;
                    }
                }
            }

            template<typename BlendFunction> void ellipseFill(int cx, int cy, int radiusX, int radiusY, Color color, BlendFunction f)
            {
                // Algorithm based on "A Fast Bresenham Type Algorithm For Drawing Ellipses" by John Kennedy <rkennedy@ix.netcom.com>
                int lastY = -1;
                int a = 2 * radiusX * radiusX;
                int b = 2 * radiusY * radiusY;
                int x = radiusX;
                int y = 0;
                int deltaX = radiusY * radiusY * (1 - 2 * radiusX);
                int deltaY = radiusX * radiusX;
                int ellipseError = 0;
                int stoppingX = b * radiusX;
                int stoppingY = 0;

                // First set of points, y' > -1
                while(stoppingX >= stoppingY)
                {
                    if(lastY != y)
                    {
                        int plotX = std::max(cx - x, clipX);
                        int plotX2 = std::min(cx + x, clipX2);
                        int plotY = cy - y;
                        if(plotY >= clipY && plotY <= clipY2)
                        {
                            for(int i = plotX; i <= plotX2; i++)
                            {
                                data[plotY * width + i] = f(color, data[plotY * width + i], opacity);
                            }
                        }
                        if(y)
                        {
                            plotY = cy + y;
                            if(plotY >= clipY && plotY <= clipY2)
                            {
                                for(int i = plotX; i <= plotX2; i++)
                                {
                                    data[plotY * width + i] = f(color, data[plotY * width + i], opacity);
                                }
                            }
                            lastY = y;
                        }
                    }

                    y++;
                    stoppingY += a;
                    ellipseError += deltaY;
                    deltaY += a;
                    if(ellipseError * 2 + deltaX > 0)
                    {
                        x--;
                        stoppingX -= b;
                        ellipseError += deltaX;
                        deltaX += b;
                    }
                }

                // First point set is done; start the second set of points
                x = 0;
                y = radiusY;
                deltaX = radiusY * radiusY;
                deltaY = radiusX * radiusX * (1 - radiusY * 2);
                ellipseError = 0;
                stoppingX = 0;
                stoppingY = a * radiusY;

                // Second set of points, y' < -1
                while(stoppingX <= stoppingY)
                {
                    if(lastY != y)
                    {
                        int plotX = std::max(cx - x, clipX);
                        int plotX2 = std::min(cx + x, clipX2);
                        int plotY = cy - y;
                        if(plotY >= clipY && plotY <= clipY2)
                        {
                            for(int i = plotX; i <= plotX2; i++)
                            {
                                data[plotY * width + i] = f(color, data[plotY * width + i], opacity);
                            }
                        }
                        plotY = cy + y;
                        if(plotY >= clipY && plotY <= clipY2)
                        {
                            for(int i = plotX; i <= plotX2; i++)
                            {
                                data[plotY * width + i] = f(color, data[plotY * width + i], opacity);
                            }
                        }
                        lastY = y;
                    }

                    x++;
                    ellipseError += deltaX;
                    deltaX += b;
                    stoppingX += b;
                    if(ellipseError * 2 + deltaY > 0)
                    {
                        y--;
                        ellipseError += deltaY;
                        deltaY += a;
                        stoppingY -= a;
                    }
                }
            }

            template<typename BlendFunction> void draw(int x, int y, Image* dest, BlendFunction f)
            {
                drawRegion(0, 0, width - 1, height - 1, x, y, dest, f);
            }

        private:
            template<typename BlendFunction> void baseDrawRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, Image* dest, BlendFunction f)
            {
                // Ensure that the source coordinates stay inside the image.
                sourceX = std::min(std::max(0, sourceX), width - 1);
                sourceY = std::min(std::max(0, sourceY), height - 1);
                sourceX2 = std::min(std::max(0, sourceX2), width - 1);
                sourceY2 = std::min(std::max(0, sourceY2), height - 1);

                // Keep source rectangle coordinates in order.
                if (sourceX > sourceX2)
                {
                    std::swap(sourceX, sourceX2);
                }
                if (sourceY > sourceY2)
                {
                    std::swap(sourceY, sourceY2);
                }

                int destX2 = destX + sourceX2;
                int destY2 = destY + sourceY2;

                // Don't draw if completely outside clipping regions.
                if(destX > dest->clipX2 || destX2 < dest->clipX || destY > dest->clipY2 || destY2 < dest->clipY)
                {
                    return;
                }

                // Keep sample rectangle inside clipping regions.
                if(destX < dest->clipX)
                {
                    sourceX += dest->clipX - destX;
                }
                if(destX2 > dest->clipX2)
                {
                    sourceX2 -= destX2 - dest->clipX2;
                }
                if(destY < dest->clipY)
                {
                    sourceY += dest->clipY - destY;
                }
                if(destY2 > dest->clipY2)
                {
                    sourceY2 -= destY2 - dest->clipY2;
                }

                // Draw the image, pixel for pixel.
                for(int i = sourceY; i <= sourceY2; i++)
                {
                    for(int j = sourceX; j <= sourceX2; j++)
                    {
                        int srcPixel = i * width + j;
                        int destPixel = (i + destX) * dest->width + (j + destY);
                        dest->data[destPixel] = f(data[srcPixel], dest->data[destPixel], opacity);
                    }
                }
            }

        public:
            void drawRegion(int sourceX, int sourceY, int sourceX2, int sourceY2, int destX, int destY, Image* dest, CopyBlender f)
            {
                // No clipping, and drawing the full source in copy blend? Just copy it raw.
                if(sourceX == 0 && sourceY == 0 && sourceX2 == width - 1 && sourceY2 == height - 1
                    && dest->clipX == 0 && dest->clipY == 0 && dest->clipX2 == width - 1 && dest->clipY2 == height - 1)
                {
                    copyRawData(dest);
                }
                else
                {
                    baseDrawRegion(sourceX, sourceY, sourceX2, sourceY2, destX, destY, dest, f);
                }
            }

            template<typename BlendFunction> void drawRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                    int destX, int destY, Image* dest, BlendFunction f)
            {
                baseDrawRegion(sourceX, sourceY, sourceX2, sourceY2, destX, destY, dest, f);
            }

            template<typename BlendFunction> void scaleDraw(int destX, int destY, double scaleX, double scaleY, Image* dest, BlendFunction f)
            {
                scaleDrawRegion(0, 0, width - 1, height - 1, destX, destY, scaleX, scaleY, dest, f);
            }

            template<typename BlendFunction> void scaleDrawRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                int destX, int destY, double scaleX, double scaleY, Image* dest, BlendFunction f)
            {
                // Ensure that the source coordinates stay inside the image.
                sourceX = std::min(std::max(0, sourceX), width - 1);
                sourceY = std::min(std::max(0, sourceY), height - 1);
                sourceX2 = std::min(std::max(0, sourceX2), width - 1);
                sourceY2 = std::min(std::max(0, sourceY2), height - 1);

                // Keep source rectangle coordinates in order.
                if (sourceX > sourceX2)
                {
                    std::swap(sourceX, sourceX2);
                }
                if (sourceY > sourceY2)
                {
                    std::swap(sourceY, sourceY2);
                }

                int sampleX = 0;
                int sampleY = 0;
                int sampleX2 = int(scaleX * (sourceX2 - sourceX)) - 1;
                int sampleY2 = int(scaleY * (sourceY2 - sourceY)) - 1;
                int destX2 = dx + sampleX2;
                int destY2 = dy + sampleY2;
                int fixedScaleX = int(double(sourceX2 - sourceX + 1) * 65536.0 * scaleX);
                int fixedScaleY = int(double(sourceY2 - sourceY + 1) * 65536.0 * scaleY);

                // Don't draw if completely outside clipping regions.
                if(destX > dest->clipX2 || destX2 < dest->clipX || destY > dest->clipY2 || destY2 < dest->clipY)
                {
                    return;
                }
                // Keep sample rectangle inside clipping regions.
                if(destX < dest->clipX)
                {
                    sampleX += dest->clipX - destX;
                }
                if(destX2 > dest->clipX2)
                {
                    sampleX2 -= destX2 - dest->clipX2;
                }
                if(destY < dest->clipY)
                {
                    sampleY += dest->clipY - destY;
                }
                if(destY2 > dest->clipY2)
                {
                    sampleY2 -= destY2 - dest->clipY2;
                }
                // Draw the scaled image, pixel for pixel.
                for(int i = sampleY; i <= sampleY2; i++)
                {
                    for(int j = sampleX; j <= sampleX2; j++)
                    {
                        int srcPixel = (((i * fixedScaleY + sourceY) >> 16) + sourceY) * width + ((j * fixedScaleX + sourceX) >> 16) + sourceX;
                        int destPixel = (i + destX) * dest->width + (j + destY);
                        dest->data[destPixel] = f(data[srcPixel], dest->data[destPixel], opacity);
                    }
                }        
            }

            template<typename BlendFunction> void rotateBlit(int x, int y, double angle, Image* dest, BlendFunction f)
            {
                rotateScaleBlitRegion(0, 0, occupiedWidth, occupiedHeight, x, y, angle, 1, dest, f);
            }

            template<typename BlendFunction> void rotateScaleBlit(int x, int y, double angle, double scale, Image* dest, BlendFunction f)
            {
                rotateScaleBlitRegion(0, 0, occupiedWidth, occupiedHeight, x, y, angle, scale, dest, f);
            }

            template<typename BlendFunction> void rotateBlitRegion(int sourceX, int sourceY, int sourceX2, int sourceY2,
                int destX, int destY, double angle, Image* dest, BlendFunction blend)
            {
                rotateScaleBlitRegion(sourceX, sourceY, sourceX2, sourceY2, destX, destY, angle, 1.0, dest, f);
            }
    };
}

#endif