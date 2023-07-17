//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <AppKit/AppKit.hpp>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CGImage.h>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CTFont.h>
#include <CoreText/CTLine.h>
#include <CoreText/CoreText.h>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include "texture.h"
#include <array>
#include <fstream>
#include <iostream>
#include <jpeglib.h>

//
//
//
Texture::~Texture() { release(); }

//
//
//
bool
Texture::loadFromMemory(MTL::Device* dev, uint8_t* buffer, uint32_t width, uint32_t height)
{
    auto* pTextureDesc = MTL::TextureDescriptor::alloc()->init();
    pTextureDesc->setWidth(width);
    pTextureDesc->setHeight(height);
    pTextureDesc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
    pTextureDesc->setTextureType(MTL::TextureType2D);
    pTextureDesc->setStorageMode(MTL::StorageModeManaged);
    pTextureDesc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);

    auto* pTexture = dev->newTexture(pTextureDesc);
    tex_           = pTexture;
    width_         = width;
    height_        = height;

    tex_->replaceRegion(MTL::Region(0, 0, 0, width, height, 1), 0, buffer, width * 4);

    pTextureDesc->release();

    return true;
}

//
//
//
bool
Texture::loadFromJPG(MTL::Device* dev, std::string path, uint32_t toW, uint32_t toH)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr         jerr;

    /* More stuff */
    FILE*      infile;     /* source file */
    JSAMPARRAY buffer;     /* Output row buffer */
    int        row_stride; /* physical row width in output buffer */

    if ((infile = fopen(path.c_str(), "rb")) == nullptr)
    {
        std::cerr << "can't open: " << path << std::endl;
        return false;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;

    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    auto     baseW       = cinfo.output_width;
    auto     baseH       = cinfo.output_height;
    uint8_t* textureData = (uint8_t*)alloca(toW * toH * 4);

    auto rateW = (double)toW / (double)baseW;
    auto rateH = (double)toH / (double)baseH;

    auto lim = [](double n, size_t m) { return n <= m - 1 ? n : m - 1; };

    while (cinfo.output_scanline < baseH)
    {
        double y = lim(rateH * cinfo.output_scanline, toH);
        jpeg_read_scanlines(&cinfo, buffer, 1);
        for (size_t i = 0; i < baseW; ++i)
        {
            auto red   = buffer[0][i * 3 + 0];
            auto green = buffer[0][i * 3 + 1];
            auto blue  = buffer[0][i * 3 + 2];

            double x = lim(rateW * i, toW);
            // TODO: ちゃんと縮小処理をする
            size_t dstIdx           = (std::ceil(y) * toW + std::ceil(x)) * 4;
            textureData[dstIdx + 0] = red;
            textureData[dstIdx + 1] = green;
            textureData[dstIdx + 2] = blue;
            textureData[dstIdx + 3] = 0xff;
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return loadFromMemory(dev, textureData, toW, toH);
}

//
//
//
bool
Texture::loadFromPNG(MTL::Device* dev, std::string path, uint32_t toW, uint32_t toH)
{
    return true;
}

//
//
//
bool
Texture::buildByString(MTL::Device* dev, const StringDesc& strdesc)
{
    auto fSize      = strdesc.size;
    auto fontName   = CFStringCreateWithCString(kCFAllocatorDefault, strdesc.fontName.c_str(), kCFStringEncodingUTF8);
    auto font       = CTFontCreateWithName(fontName, fSize, nullptr);
    auto fcol       = CGColorCreateGenericRGB(strdesc.red, strdesc.green, strdesc.blue, strdesc.alpha);
    auto dcKey      = std::array<const void*, 2>({kCTFontAttributeName, kCTForegroundColorAttributeName});
    auto dcVal      = std::array<const void*, 2>({font, fcol});
    auto attributes = CFDictionaryCreate(kCFAllocatorDefault, dcKey.data(), dcVal.data(), dcKey.size(),
                                         &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    auto msg        = CFStringCreateWithCString(kCFAllocatorDefault, strdesc.message.c_str(), kCFStringEncodingUTF8);
    auto attrStr    = CFAttributedStringCreate(kCFAllocatorDefault, msg, attributes);
    auto colorSpace = CGColorSpaceCreateDeviceRGB();
    auto line       = CTLineCreateWithAttributedString(attrStr);
    auto rect       = CTLineGetImageBounds(line, nullptr);
    auto textWidth  = std::ceil(rect.size.width * 1.1);
    auto textHeight = std::ceil(rect.size.height * 1.1);
    auto ctx =
        CGBitmapContextCreate(nullptr, textWidth, textHeight, 8, 4 * textWidth, colorSpace, kCGImageAlphaPremultipliedLast);
    CGContextSetTextPosition(ctx, fSize * 0.04f, fSize * 0.1f);
    CTLineDraw(line, ctx);
    auto bmWidth  = CGBitmapContextGetWidth(ctx);
    auto bmHeight = CGBitmapContextGetHeight(ctx);
    // std::cout << "Text: " << textWidth << "(" << bmWidth << ")x" << textHeight << "(" << bmHeight << ")" << std::endl;
    auto* data   = CGBitmapContextGetData(ctx);
    auto* bitmap = static_cast<uint8_t*>(data);
    auto  ret    = loadFromMemory(dev, bitmap, bmWidth, bmHeight);

    CFRelease(fontName);
    CFRelease(msg);
    CFRelease(attributes);
    CFRelease(attrStr);
    CFRelease(colorSpace);
    CFRelease(font);
    CFRelease(line);
    CFRelease(ctx);

    return ret;
}

//
//
//
void
Texture::release()
{
    if (tex_)
    {
        tex_->release();
        tex_ = nullptr;
    }
}

//
