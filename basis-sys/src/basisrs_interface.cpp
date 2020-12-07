#define BASISRS_INTERFACE_HAS_STRUCTS
#include "basisrs_interface.hpp"

static basist::etc1_global_selector_codebook *g_pGlobal_codebook;

extern "C" {
    void basisrs_init() {
        basisu_transcoder_init();

        if (!g_pGlobal_codebook) {
            g_pGlobal_codebook = new basist::etc1_global_selector_codebook(g_global_selector_cb_size, g_global_selector_cb);
        }
    }

    void basisrs_deinit() {
        if (g_pGlobal_codebook) {
            delete g_pGlobal_codebook;
            g_pGlobal_codebook = nullptr;
        }
    }

    basisrs_vector_u32 basisrs_file_info_get_mipmap_levels(const basisu_file_info *data) {
        return basisrs_vector_u32{
                data->m_image_mipmap_levels.data(),
                data->m_image_mipmap_levels.size(),
        };
    }

    basisrs_vector_slice_info basisrs_file_info_get_slice_info(const basisu_file_info *data) {
        return basisrs_vector_slice_info{
                data->m_slice_info.data(),
                data->m_slice_info.size(),
        };
    }

    basisu_transcoder *basisrs_create_transcoder() {
        return new basisu_transcoder(g_pGlobal_codebook);
    }

    void basisrs_destroy_transcoder(basisu_transcoder *me) {
        delete me;
    }

    bool basisrs_validate_file_checksums(const basisu_transcoder *me, const void *pData, uint32_t data_size,
                                         bool full_validation) {
        return me->validate_file_checksums(pData, data_size, full_validation);
    }

    bool basisrs_validate_header(const basisu_transcoder *me, const void *pData, uint32_t data_size) {
        return me->validate_header(pData, data_size);
    }

    basis_texture_type basisrs_get_texture_type(const basisu_transcoder *me, const void *pData, uint32_t data_size) {
        return me->get_texture_type(pData, data_size);
    }

    bool basisrs_get_userdata(const basisu_transcoder *me, const void *pData, uint32_t data_size, uint32_t &userdata0,
                              uint32_t &userdata1) {
        return me->get_userdata(pData, data_size, userdata0, userdata1);
    }

    uint32_t basisrs_get_total_images(const basisu_transcoder *me, const void *pData, uint32_t data_size) {
        return me->get_total_images(pData, data_size);
    }

    basis_tex_format basisrs_get_tex_format(const basisu_transcoder *me, const void *pData, uint32_t data_size) {
        return me->get_tex_format(pData, data_size);
    }

    uint32_t basisrs_get_total_image_levels(const basisu_transcoder *me, const void *pData, uint32_t data_size,
                                            uint32_t image_index) {
        return me->get_total_image_levels(pData, data_size, image_index);
    }

    bool
    basisrs_get_image_level_desc(const basisu_transcoder *me, const void *pData, uint32_t data_size, uint32_t image_index,
                                 uint32_t level_index, uint32_t &orig_width, uint32_t &orig_height,
                                 uint32_t &total_blocks) {
        return me->get_image_level_desc(pData, data_size, image_index, level_index, orig_width, orig_height, total_blocks);
    }

    bool basisrs_start_transcoding(basisu_transcoder *me, const void *pData, uint32_t data_size) {
        return me->start_transcoding(pData, data_size);
    }

    bool basisrs_stop_transcoding(basisu_transcoder *me) {
        return me->stop_transcoding();
    }

    bool basisrs_get_ready_to_transcode(const basisu_transcoder *me) {
        return me->get_ready_to_transcode();
    }

    int
    basisrs_find_slice(const basisu_transcoder *me, const void *pData, uint32_t data_size, uint32_t image_index,
                       uint32_t level_index, bool alpha_data) {
        return me->find_slice(pData, data_size, image_index, level_index, alpha_data);
    }

    bool basisrs_transcode_slice(const basisu_transcoder *me, const void *pData, uint32_t data_size, uint32_t slice_index,
                                 void *pOutput_blocks,
                                 uint32_t output_blocks_buf_size_in_blocks_or_pixels, block_format fmt,
                                 uint32_t output_block_stride_in_bytes, uint32_t decode_flags,
                                 uint32_t output_row_pitch_in_blocks_or_pixels, basisu_transcoder_state *pState,
                                 void *pAlpha_blocks, uint32_t output_rows_in_pixels, int channel0, int channel1) {
        return me->transcode_slice(pData, data_size, slice_index, pOutput_blocks,
                                   output_blocks_buf_size_in_blocks_or_pixels, fmt, output_block_stride_in_bytes,
                                   decode_flags, output_row_pitch_in_blocks_or_pixels, pState, pAlpha_blocks,
                                   output_rows_in_pixels, channel0, channel1);
    }

    bool
    basisrs_transcode_image_level(const basisu_transcoder *me, const void *pData, uint32_t data_size, uint32_t image_index,
                                  uint32_t level_index,
                                  void *pOutput_blocks, uint32_t output_blocks_buf_size_in_blocks_or_pixels,
                                  transcoder_texture_format fmt, uint32_t decode_flags,
                                  uint32_t output_row_pitch_in_blocks_or_pixels, basisu_transcoder_state *pState,
                                  uint32_t output_rows_in_pixels) {
        return me->transcode_image_level(pData, data_size, image_index, level_index, pOutput_blocks,
                                         output_blocks_buf_size_in_blocks_or_pixels, fmt, decode_flags,
                                         output_row_pitch_in_blocks_or_pixels, pState, output_rows_in_pixels);
    }

    bool
    basisrs_get_file_info(const basisu_transcoder *me, const void *pData, uint32_t data_size, basisu_file_info &file_info) {
        return me->get_file_info(pData, data_size, file_info);
    }

    bool basisrs_get_image_info(const basisu_transcoder *me, const void *pData, uint32_t data_size,
                                basisu_image_info &image_info, uint32_t image_index) {
        return me->get_image_info(pData, data_size, image_info, image_index);
    }

    bool basisrs_get_image_level_info(const basisu_transcoder *me, const void *pData, uint32_t data_size,
                                      basisu_image_level_info &level_info, uint32_t image_index, uint32_t level_index) {
        return me->get_image_level_info(pData, data_size, level_info, image_index, level_index);
    }
}
