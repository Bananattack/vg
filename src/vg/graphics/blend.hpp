#ifndef VG_GRAPHICS_BLEND_HPP
#define VG_GRAPHICS_BLEND_HPP

#include <cmath>
#include <algorithm>
#include "color.hpp"

namespace vg
{
    enum BlendMode
    {
        BlendCopy,
        BlendPreserve,
        BlendMerge,
        BlendAdd,
        BlendSubtract,
        BlendScreen,
        BlendMultiply,
        BlendLighten,
        BlendDarken,
        BlendDifference,
    };

    struct CopyBlender
    {
        Color operator()(Color source, Color dest, ColorChannel opacity) const
        {
            return source;
        }
    };

    struct PreserveBlender
    {
		Color operator()(Color source, Color dest, ColorChannel opacity) const
		{
            int sourceAlpha = source[AlphaChannel] * opacity / 255;

            Color result;
            result[RedChannel] = (sourceAlpha * (source[RedChannel] - dest[RedChannel])) / 255 + dest[RedChannel];
            result[GreenChannel] = (sourceAlpha * (source[GreenChannel] - dest[GreenChannel])) / 255 + dest[GreenChannel];
            result[BlueChannel] = (sourceAlpha * (source[BlueChannel] - dest[BlueChannel])) / 255 + dest[BlueChannel];
            result[AlphaChannel] = dest[AlphaChannel];

            return result;
		}
    };

    struct MergeBlender
    {
        Color operator()(Color source, Color dest, ColorChannel opacity) const
        {
            int sourceAlpha = source[AlphaChannel] * opacity / 255;
            int finalAlpha = sourceAlpha + ((255 - sourceAlpha) * dest[AlphaChannel]) / 255;
            sourceAlpha = (finalAlpha == 0) ? 0 : sourceAlpha * 255 / finalAlpha;

            Color result;
            result[RedChannel] = (source[RedChannel] * sourceAlpha + dest[RedChannel] * (255 - sourceAlpha)) / 255;
            result[GreenChannel] = (source[GreenChannel] * sourceAlpha + dest[GreenChannel] * (255 - sourceAlpha)) / 255;
            result[BlueChannel] = (source[BlueChannel] * sourceAlpha + dest[BlueChannel] * (255 - sourceAlpha)) / 255;
            result[AlphaChannel] = finalAlpha;

            return result;
        }
    };

    struct AddBlender
    {
        Color operator()(Color source, Color dest, ColorChannel opacity) const
        {
            int sourceAlpha = source[AlphaChannel] * opacity / 255;

            Color result;
            result[RedChannel] = std::min((sourceAlpha * source[RedChannel]) / 255 + dest[RedChannel], 255);
            result[GreenChannel] = std::min((sourceAlpha * source[GreenChannel]) / 255 + dest[GreenChannel], 255);
            result[BlueChannel] = std::min((sourceAlpha * source[BlueChannel]) / 255 + dest[BlueChannel], 255);
            result[AlphaChannel] = dest[AlphaChannel];

            return result;
        }
    };

    struct SubtractBlender
    {
        Color operator()(Color source, Color dest, ColorChannel opacity) const
        {
            int sourceAlpha = source[AlphaChannel] * opacity / 255;

            Color result;
            result[RedChannel] = std::max((sourceAlpha * -source[RedChannel]) / 255 + dest[RedChannel], 0);
            result[GreenChannel] = std::max((sourceAlpha * -source[GreenChannel]) / 255 + dest[GreenChannel], 0);
            result[BlueChannel] = std::max((sourceAlpha * -source[BlueChannel]) / 255 + dest[BlueChannel], 0);
            result[AlphaChannel] = dest[AlphaChannel];

            return result;
        }
    };

    struct ScreenBlender
    {
		Color operator()(Color source, Color dest, ColorChannel opacity) const
		{
            int sourceAlpha = source[AlphaChannel] * opacity / 255;

            Color result;
            result[RedChannel] = (sourceAlpha * (255 - (((255 - source[RedChannel]) * (255 - dest[RedChannel])) / 255) - dest[RedChannel])) / 255 + dest[RedChannel];
            result[GreenChannel] = (sourceAlpha * (255 - (((255 - source[GreenChannel]) * (255 - dest[GreenChannel])) / 255) - dest[GreenChannel])) / 255 + dest[GreenChannel];
            result[BlueChannel] = (sourceAlpha * (255 - (((255 - source[BlueChannel]) * (255 - dest[BlueChannel])) / 255) - dest[BlueChannel])) / 255 + dest[BlueChannel];
            result[AlphaChannel] = dest[AlphaChannel];

            return result;
		}
    };

    struct MultiplyBlender
    {
		Color operator()(Color source, Color dest, ColorChannel opacity) const
		{
            int sourceAlpha = source[AlphaChannel] * opacity / 255;

            Color result;
            result[RedChannel] = (sourceAlpha * ((source[RedChannel] * dest[RedChannel]) / 256 - dest[RedChannel])) / 255 + dest[RedChannel];
            result[GreenChannel] = (sourceAlpha * ((source[GreenChannel] * dest[GreenChannel]) / 256 - dest[GreenChannel])) / 255 + dest[GreenChannel];
            result[BlueChannel] = (sourceAlpha * ((source[BlueChannel] * dest[BlueChannel]) / 256 - dest[BlueChannel])) / 255 + dest[BlueChannel];
            result[AlphaChannel] = dest[AlphaChannel];

            return result;
		}
    };

    struct LightenBlender
    {
		Color operator()(Color source, Color dest, ColorChannel opacity) const
		{
            int sourceAlpha = source[AlphaChannel] * opacity / 255;

            Color result;
            result[RedChannel] = (sourceAlpha * (std::max(source[RedChannel], dest[RedChannel]) - dest[RedChannel])) / 255 + dest[RedChannel];
            result[GreenChannel] = (sourceAlpha * (std::max(source[GreenChannel], dest[GreenChannel]) - dest[GreenChannel])) / 255 + dest[GreenChannel];
            result[BlueChannel] = (sourceAlpha * (std::max(source[BlueChannel], dest[BlueChannel]) - dest[BlueChannel])) / 255 + dest[BlueChannel];
            result[AlphaChannel] = dest[AlphaChannel];

            return result;
		}
    };

    struct DarkenBlender
    {
		Color operator()(Color source, Color dest, ColorChannel opacity) const
		{
            int sourceAlpha = source[AlphaChannel] * opacity / 255;

            Color result;
            result[RedChannel] = (sourceAlpha * (std::min(source[RedChannel], dest[RedChannel]) - dest[RedChannel])) / 255 + dest[RedChannel];
            result[GreenChannel] = (sourceAlpha * (std::min(source[GreenChannel], dest[GreenChannel]) - dest[GreenChannel])) / 255 + dest[GreenChannel];
            result[BlueChannel] = (sourceAlpha * (std::min(source[BlueChannel], dest[BlueChannel]) - dest[BlueChannel])) / 255 + dest[BlueChannel];
            result[AlphaChannel] = dest[AlphaChannel];

            return result;
		}
    };


    struct DifferenceBlender
    {
		Color operator()(Color source, Color dest, ColorChannel opacity) const
		{
            int sourceAlpha = source[AlphaChannel] * opacity / 255;

            Color result;
            result[RedChannel] = (sourceAlpha * (std::abs(source[RedChannel] - dest[RedChannel]) - dest[RedChannel])) / 255 + dest[RedChannel];
            result[GreenChannel] = (sourceAlpha * (std::abs(source[GreenChannel] - dest[GreenChannel]) - dest[GreenChannel])) / 255 + dest[GreenChannel];
            result[BlueChannel] = (sourceAlpha * (std::abs(source[BlueChannel] - dest[BlueChannel]) - dest[BlueChannel])) / 255 + dest[BlueChannel];
            result[AlphaChannel] = dest[AlphaChannel];

            return result;
		}
    };
}

#endif