#pragma once

#include <cstdint>
#include <vector>

#ifndef BASISRS_INTERFACE_HAS_STRUCTS
struct basisu_transcoder_state;
struct basisu_transcoder;

// Low-level formats directly supported by the transcoder (other supported texture formats are combinations of these low-level block formats).
// You probably don't care about these enum's unless you are going pretty low-level and calling the transcoder to decode individual slices.
enum class block_format
{
    cETC1,								// ETC1S RGB
    cETC2_RGBA,							// full ETC2 EAC RGBA8 block
    cBC1,									// DXT1 RGB
    cBC3,									// BC4 block followed by a four color BC1 block
    cBC4,									// DXT5A (alpha block only)
    cBC5,									// two BC4 blocks
    cPVRTC1_4_RGB,						// opaque-only PVRTC1 4bpp
    cPVRTC1_4_RGBA,					// PVRTC1 4bpp RGBA
    cBC7,									// Full BC7 block, any mode
    cBC7_M5_COLOR,						// RGB BC7 mode 5 color (writes an opaque mode 5 block)
    cBC7_M5_ALPHA,						// alpha portion of BC7 mode 5 (cBC7_M5_COLOR output data must have been written to the output buffer first to set the mode/rot fields etc.)
    cETC2_EAC_A8,						// alpha block of ETC2 EAC (first 8 bytes of the 16-bit ETC2 EAC RGBA format)
    cASTC_4x4,							// ASTC 4x4 (either color-only or color+alpha). Note that the transcoder always currently assumes sRGB is not enabled when outputting ASTC
    // data. If you use a sRGB ASTC format you'll get ~1 LSB of additional error, because of the different way ASTC decoders scale 8-bit endpoints to 16-bits during unpacking.

    cATC_RGB,
    cATC_RGBA_INTERPOLATED_ALPHA,
    cFXT1_RGB,							// Opaque-only, has oddball 8x4 pixel block size

    cPVRTC2_4_RGB,
    cPVRTC2_4_RGBA,

    cETC2_EAC_R11,
    cETC2_EAC_RG11,

    cIndices,							// Used internally: Write 16-bit endpoint and selector indices directly to output (output block must be at least 32-bits)

    cRGB32,								// Writes RGB components to 32bpp output pixels
    cRGBA32,								// Writes RGB255 components to 32bpp output pixels
    cA32,									// Writes alpha component to 32bpp output pixels

    cRGB565,
    cBGR565,

    cRGBA4444_COLOR,
    cRGBA4444_ALPHA,
    cRGBA4444_COLOR_OPAQUE,
    cRGBA4444,

    cTotalBlockFormats
};

enum class basis_tex_format
{
    cETC1S = 0,
    cUASTC4x4 = 1
};

enum basis_texture_type
{
    cBASISTexType2D = 0,					// An arbitrary array of 2D RGB or RGBA images with optional mipmaps, array size = # images, each image may have a different resolution and # of mipmap levels
    cBASISTexType2DArray = 1,			// An array of 2D RGB or RGBA images with optional mipmaps, array size = # images, each image has the same resolution and mipmap levels
    cBASISTexTypeCubemapArray = 2,	// an array of cubemap levels, total # of images must be divisable by 6, in X+, X-, Y+, Y-, Z+, Z- order, with optional mipmaps
    cBASISTexTypeVideoFrames = 3,		// An array of 2D video frames, with optional mipmaps, # frames = # images, each image has the same resolution and # of mipmap levels
    cBASISTexTypeVolume = 4,			// A 3D texture with optional mipmaps, Z dimension = # images, each image has the same resolution and # of mipmap levels

    cBASISTexTypeTotal
};

// High-level composite texture formats supported by the transcoder.
// Each of these texture formats directly correspond to OpenGL/D3D/Vulkan etc. texture formats.
// Notes:
// - If you specify a texture format that supports alpha, but the .basis file doesn't have alpha, the transcoder will automatically output a
// fully opaque (255) alpha channel.
// - The PVRTC1 texture formats only support power of 2 dimension .basis files, but this may be relaxed in a future version.
// - The PVRTC1 transcoders are real-time encoders, so don't expect the highest quality. We may add a slower encoder with improved quality.
// - These enums must be kept in sync with Javascript code that calls the transcoder.
enum class transcoder_texture_format
{
    // Compressed formats

    // ETC1-2
    cTFETC1_RGB = 0,							// Opaque only, returns RGB or alpha data if cDecodeFlagsTranscodeAlphaDataToOpaqueFormats flag is specified
    cTFETC2_RGBA = 1,							// Opaque+alpha, ETC2_EAC_A8 block followed by a ETC1 block, alpha channel will be opaque for opaque .basis files

    // BC1-5, BC7 (desktop, some mobile devices)
    cTFBC1_RGB = 2,							// Opaque only, no punchthrough alpha support yet, transcodes alpha slice if cDecodeFlagsTranscodeAlphaDataToOpaqueFormats flag is specified
    cTFBC3_RGBA = 3, 							// Opaque+alpha, BC4 followed by a BC1 block, alpha channel will be opaque for opaque .basis files
    cTFBC4_R = 4,								// Red only, alpha slice is transcoded to output if cDecodeFlagsTranscodeAlphaDataToOpaqueFormats flag is specified
    cTFBC5_RG = 5,								// XY: Two BC4 blocks, X=R and Y=Alpha, .basis file should have alpha data (if not Y will be all 255's)
    cTFBC7_RGBA = 6,							// RGB or RGBA, mode 5 for ETC1S, modes (1,2,3,5,6,7) for UASTC

    // PVRTC1 4bpp (mobile, PowerVR devices)
    cTFPVRTC1_4_RGB = 8,						// Opaque only, RGB or alpha if cDecodeFlagsTranscodeAlphaDataToOpaqueFormats flag is specified, nearly lowest quality of any texture format.
    cTFPVRTC1_4_RGBA = 9,					// Opaque+alpha, most useful for simple opacity maps. If .basis file doesn't have alpha cTFPVRTC1_4_RGB will be used instead. Lowest quality of any supported texture format.

    // ASTC (mobile, Intel devices, hopefully all desktop GPU's one day)
    cTFASTC_4x4_RGBA = 10,					// Opaque+alpha, ASTC 4x4, alpha channel will be opaque for opaque .basis files. Transcoder uses RGB/RGBA/L/LA modes, void extent, and up to two ([0,47] and [0,255]) endpoint precisions.

    // ATC (mobile, Adreno devices, this is a niche format)
    cTFATC_RGB = 11,							// Opaque, RGB or alpha if cDecodeFlagsTranscodeAlphaDataToOpaqueFormats flag is specified. ATI ATC (GL_ATC_RGB_AMD)
    cTFATC_RGBA = 12,							// Opaque+alpha, alpha channel will be opaque for opaque .basis files. ATI ATC (GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD)

    // FXT1 (desktop, Intel devices, this is a super obscure format)
    cTFFXT1_RGB = 17,							// Opaque only, uses exclusively CC_MIXED blocks. Notable for having a 8x4 block size. GL_3DFX_texture_compression_FXT1 is supported on Intel integrated GPU's (such as HD 630).
    // Punch-through alpha is relatively easy to support, but full alpha is harder. This format is only here for completeness so opaque-only is fine for now.
    // See the BASISU_USE_ORIGINAL_3DFX_FXT1_ENCODING macro in basisu_transcoder_internal.h.

    cTFPVRTC2_4_RGB = 18,					// Opaque-only, almost BC1 quality, much faster to transcode and supports arbitrary texture dimensions (unlike PVRTC1 RGB).
    cTFPVRTC2_4_RGBA = 19,					// Opaque+alpha, slower to encode than cTFPVRTC2_4_RGB. Premultiplied alpha is highly recommended, otherwise the color channel can leak into the alpha channel on transparent blocks.

    cTFETC2_EAC_R11 = 20,					// R only (ETC2 EAC R11 unsigned)
    cTFETC2_EAC_RG11 = 21,					// RG only (ETC2 EAC RG11 unsigned), R=opaque.r, G=alpha - for tangent space normal maps

    // Uncompressed (raw pixel) formats
    cTFRGBA32 = 13,							// 32bpp RGBA image stored in raster (not block) order in memory, R is first byte, A is last byte.
    cTFRGB565 = 14,							// 166pp RGB image stored in raster (not block) order in memory, R at bit position 11
    cTFBGR565 = 15,							// 16bpp RGB image stored in raster (not block) order in memory, R at bit position 0
    cTFRGBA4444 = 16,							// 16bpp RGBA image stored in raster (not block) order in memory, R at bit position 12, A at bit position 0

    cTFTotalTextureFormats = 22,

    // Old enums for compatibility with code compiled against previous versions
    cTFETC1 = cTFETC1_RGB,
    cTFETC2 = cTFETC2_RGBA,
    cTFBC1 = cTFBC1_RGB,
    cTFBC3 = cTFBC3_RGBA,
    cTFBC4 = cTFBC4_R,
    cTFBC5 = cTFBC5_RG,

    // Previously, the caller had some control over which BC7 mode the transcoder output. We've simplified this due to UASTC, which supports numerous modes.
    cTFBC7_M6_RGB = cTFBC7_RGBA,			// Opaque only, RGB or alpha if cDecodeFlagsTranscodeAlphaDataToOpaqueFormats flag is specified. Highest quality of all the non-ETC1 formats.
    cTFBC7_M5_RGBA = cTFBC7_RGBA,			// Opaque+alpha, alpha channel will be opaque for opaque .basis files
    cTFBC7_M6_OPAQUE_ONLY = cTFBC7_RGBA,
    cTFBC7_M5 = cTFBC7_RGBA,
    cTFBC7_ALT = 7,

    cTFASTC_4x4 = cTFASTC_4x4_RGBA,

    cTFATC_RGBA_INTERPOLATED_ALPHA = cTFATC_RGBA,
};

struct basisu_slice_info
{
    uint32_t m_orig_width;
    uint32_t m_orig_height;

    uint32_t m_width;
    uint32_t m_height;

    uint32_t m_num_blocks_x;
    uint32_t m_num_blocks_y;
    uint32_t m_total_blocks;

    uint32_t m_compressed_size;

    uint32_t m_slice_index;	// the slice index in the .basis file
    uint32_t m_image_index;	// the source image index originally provided to the encoder
    uint32_t m_level_index;	// the mipmap level within this image

    uint32_t m_unpacked_slice_crc16;

    bool m_alpha_flag;		// true if the slice has alpha data
    bool m_iframe_flag;		// true if the slice is an I-Frame
};

struct basisu_image_info
{
    uint32_t m_image_index;
    uint32_t m_total_levels;

    uint32_t m_orig_width;
    uint32_t m_orig_height;

    uint32_t m_width;
    uint32_t m_height;

    uint32_t m_num_blocks_x;
    uint32_t m_num_blocks_y;
    uint32_t m_total_blocks;

    uint32_t m_first_slice_index;

    bool m_alpha_flag;		// true if the image has alpha data
    bool m_iframe_flag;		// true if the image is an I-Frame
};

struct basisu_image_level_info
{
    uint32_t m_image_index;
    uint32_t m_level_index;

    uint32_t m_orig_width;
    uint32_t m_orig_height;

    uint32_t m_width;
    uint32_t m_height;

    uint32_t m_num_blocks_x;
    uint32_t m_num_blocks_y;
    uint32_t m_total_blocks;

    uint32_t m_first_slice_index;

    bool m_alpha_flag;		// true if the image has alpha data
    bool m_iframe_flag;		// true if the image is an I-Frame
};

struct basisu_file_info
{
    uint32_t m_version;
    uint32_t m_total_header_size;

    uint32_t m_total_selectors;
    uint32_t m_selector_codebook_size;

    uint32_t m_total_endpoints;
    uint32_t m_endpoint_codebook_size;

    uint32_t m_tables_size;
    uint32_t m_slices_size;

    basis_texture_type m_tex_type;
    uint32_t m_us_per_frame;

    // Low-level slice information (1 slice per image for color-only basis files, 2 for alpha basis files)
    std::vector<basisu_slice_info> m_slice_info;

    uint32_t m_total_images;	 // total # of images
    std::vector<uint32_t> m_image_mipmap_levels; // the # of mipmap levels for each image

    uint32_t m_userdata0;
    uint32_t m_userdata1;

    basis_tex_format m_tex_format; // ETC1S, UASTC, etc.

    bool m_y_flipped;				// true if the image was Y flipped
    bool m_etc1s;					// true if the file is ETC1S
    bool m_has_alpha_slices;	// true if the texture has alpha slices (for ETC1S: even slices RGB, odd slices alpha)
};
#else
#include "../basisu/transcoder/basisu_transcoder.h"
using namespace basist;
#endif

extern "C" {
    void basisrs_init();
    void basisrs_deinit();

    struct basisrs_vector_u32 {
        const uint32_t *values;
        size_t size;
    };

    basisrs_vector_u32 basisrs_file_info_get_mipmap_levels(const basisu_file_info *data);

    struct basisrs_vector_slice_info {
        const basisu_slice_info *values;
        size_t size;
    };
    basisrs_vector_slice_info basisrs_file_info_get_slice_info(const basisu_file_info *data);

    basisu_transcoder* basisrs_create_transcoder();
    void basisrs_destroy_transcoder(basisu_transcoder* me);

    // Validates the .basis file. This computes a crc16 over the entire file, so it's slow.
    bool basisrs_validate_file_checksums(const basisu_transcoder *me, const void *pData, uint32_t data_size,
                                         bool full_validation);

    // Quick header validation - no crc16 checks.
    bool basisrs_validate_header(const basisu_transcoder *me, const void *pData, uint32_t data_size);

    basis_texture_type get_texture_type(const basisu_transcoder *me, const void *pData, uint32_t data_size);
    bool basisrs_get_userdata(const basisu_transcoder *me, const void *pData, uint32_t data_size, uint32_t &userdata0,
                              uint32_t &userdata1);

    // Returns the total number of images in the basis file (always 1 or more).
    // Note that the number of mipmap levels for each image may differ, and that images may have different resolutions.
    uint32_t basisrs_get_total_images(const basisu_transcoder *me, const void *pData, uint32_t data_size);

    basis_tex_format basisrs_get_tex_format(const basisu_transcoder *me, const void *pData, uint32_t data_size);

    // Returns the number of mipmap levels in an image.
    uint32_t basisrs_get_total_image_levels(const basisu_transcoder *me, const void *pData, uint32_t data_size,
                                            uint32_t image_index);

    // Returns basic information about an image. Note that orig_width/orig_height may not be a multiple of 4.
    bool
    basisrs_get_image_level_desc(const basisu_transcoder *me, const void *pData, uint32_t data_size, uint32_t image_index,
                                 uint32_t level_index, uint32_t &orig_width, uint32_t &orig_height, uint32_t &total_blocks);

    // Returns information about the specified image.
    bool basisrs_get_image_info(const basisu_transcoder *me, const void *pData, uint32_t data_size,
                                basisu_image_info &image_info, uint32_t image_index);

    // Returns information about the specified image's mipmap level.
    bool basisrs_get_image_level_info(const basisu_transcoder *me, const void *pData, uint32_t data_size,
                                      basisu_image_level_info &level_info, uint32_t image_index, uint32_t level_index);

    // Get a description of the basis file and low-level information about each slice.
    bool
    basisrs_get_file_info(const basisu_transcoder *me, const void *pData, uint32_t data_size, basisu_file_info &file_info);

    // start_transcoding() must be called before calling transcode_slice() or transcode_image_level().
    // For ETC1S files, this call decompresses the selector/endpoint codebooks, so ideally you would only call this once per .basis file (not each image/mipmap level).
    bool basisrs_start_transcoding(basisu_transcoder *me, const void *pData, uint32_t data_size);

    bool basisrs_stop_transcoding(basisu_transcoder *me);

    // Returns true if start_transcoding() has been called.
    bool basisrs_get_ready_to_transcode(const basisu_transcoder *me);

    // transcode_image_level() decodes a single mipmap level from the .basis file to any of the supported output texture formats.
    // It'll first find the slice(s) to transcode, then call transcode_slice() one or two times to decode both the color and alpha texture data (or RG texture data from two slices for BC5).
    // If the .basis file doesn't have alpha slices, the output alpha blocks will be set to fully opaque (all 255's).
    // Currently, to decode to PVRTC1 the basis texture's dimensions in pixels must be a power of 2, due to PVRTC1 format requirements.
    // output_blocks_buf_size_in_blocks_or_pixels should be at least the image level's total_blocks (num_blocks_x * num_blocks_y), or the total number of output pixels if fmt==cTFRGBA32.
    // output_row_pitch_in_blocks_or_pixels: Number of blocks or pixels per row. If 0, the transcoder uses the slice's num_blocks_x or orig_width (NOT num_blocks_x * 4). Ignored for PVRTC1 (due to texture swizzling).
    // output_rows_in_pixels: Ignored unless fmt is cRGBA32. The total number of output rows in the output buffer. If 0, the transcoder assumes the slice's orig_height (NOT num_blocks_y * 4).
    // Notes:
    // - basisu_transcoder_init() must have been called first to initialize the transcoder lookup tables before calling this function.
    // - This method assumes the output texture buffer is readable. In some cases to handle alpha, the transcoder will write temporary data to the output texture in
    // a first pass, which will be read in a second pass.
    bool basisrs_transcode_image_level(
            const basisu_transcoder *me, const void *pData, uint32_t data_size,
            uint32_t image_index, uint32_t level_index,
            void *pOutput_blocks, uint32_t output_blocks_buf_size_in_blocks_or_pixels,
            transcoder_texture_format fmt,
            uint32_t decode_flags = 0, uint32_t output_row_pitch_in_blocks_or_pixels = 0,
            basisu_transcoder_state *pState = nullptr, uint32_t output_rows_in_pixels = 0);

    // Finds the basis slice corresponding to the specified image/level/alpha params, or -1 if the slice can't be found.
    int
    basisrs_find_slice(const basisu_transcoder *me, const void *pData, uint32_t data_size, uint32_t image_index, uint32_t level_index, bool alpha_data);

    // transcode_slice() decodes a single slice from the .basis file. It's a low-level API - most likely you want to use transcode_image_level().
    // This is a low-level API, and will be needed to be called multiple times to decode some texture formats (like BC3, BC5, or ETC2).
    // output_blocks_buf_size_in_blocks_or_pixels is just used for verification to make sure the output buffer is large enough.
    // output_blocks_buf_size_in_blocks_or_pixels should be at least the image level's total_blocks (num_blocks_x * num_blocks_y), or the total number of output pixels if fmt==cTFRGBA32.
    // output_block_stride_in_bytes: Number of bytes between each output block.
    // output_row_pitch_in_blocks_or_pixels: Number of blocks or pixels per row. If 0, the transcoder uses the slice's num_blocks_x or orig_width (NOT num_blocks_x * 4). Ignored for PVRTC1 (due to texture swizzling).
    // output_rows_in_pixels: Ignored unless fmt is cRGBA32. The total number of output rows in the output buffer. If 0, the transcoder assumes the slice's orig_height (NOT num_blocks_y * 4).
    // Notes:
    // - basisu_transcoder_init() must have been called first to initialize the transcoder lookup tables before calling this function.
    bool basisrs_transcode_slice(const basisu_transcoder *me, const void *pData, uint32_t data_size, uint32_t slice_index,
                                 void *pOutput_blocks, uint32_t output_blocks_buf_size_in_blocks_or_pixels,
                                 block_format fmt, uint32_t output_block_stride_in_bytes, uint32_t decode_flags = 0,
                                 uint32_t output_row_pitch_in_blocks_or_pixels = 0,
                                 basisu_transcoder_state *pState = nullptr, void *pAlpha_blocks = nullptr,
                                 uint32_t output_rows_in_pixels = 0, int channel0 = -1, int channel1 = -1);

}