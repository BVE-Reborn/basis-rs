use basis_sys as sys;
use once_cell::sync::Lazy;
use std::convert::TryInto;
use std::num::NonZeroU32;

pub mod error;

static GLOBAL_STATE: Lazy<()> = Lazy::new(|| unsafe { sys::basisrs_init() });

/// Initialize global state that needs to be initialized.
///
/// This function isn't necessary to call, it will be called
/// automatically when any function that needs it is used.
///
/// This allows you to control when it happens.
pub fn init() {
    Lazy::force(&GLOBAL_STATE);
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum TextureType {
    D2,
    D2Array,
    CubemapArray,
    VideoFrames,
    D3,
    Total,
}
impl TextureType {
    fn from_internal(value: sys::basis_texture_type) -> Self {
        match value {
            sys::basis_texture_type_cBASISTexType2D => Self::D2,
            sys::basis_texture_type_cBASISTexType2DArray => Self::D2Array,
            sys::basis_texture_type_cBASISTexTypeCubemapArray => Self::CubemapArray,
            sys::basis_texture_type_cBASISTexTypeVideoFrames => Self::VideoFrames,
            sys::basis_texture_type_cBASISTexTypeVolume => Self::D3,
            sys::basis_texture_type_cBASISTexTypeTotal => Self::Total,
            _ => unreachable!("invalid internal texture type"),
        }
    }
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum BasisTextureFormat {
    Etc1s,
    UAstc,
}
impl BasisTextureFormat {
    fn from_internal(value: sys::basis_tex_format) -> Self {
        match value {
            sys::basis_tex_format_cETC1S => Self::Etc1s,
            sys::basis_tex_format_cUASTC4x4 => Self::UAstc,
            _ => unreachable!("invalid internal basis texture format"),
        }
    }
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum TargetTextureFormat {
    Etc1Rgb,
    Etc2Rgba,
    Bc1Rgb,
    Bc3Rgba,
    Bc4R,
    Bc5Rg,
    Bc7Rgba,
    Pvrtc1Rgb,
    Pvrtc1Rgba,
    AstcRgba,
    AtcRgb,
    AtcRgbA,
    Fxt1Rgb,
    Pvrtc2Rgb,
    Pvrtc2Rgba,
    EacR11,
    EacRg11,
    Rgba32,
    Rgb565,
    Bgr565,
    Rgba4444,
}
impl TargetTextureFormat {
    fn from_internal(value: sys::transcoder_texture_format) -> Self {
        match value {
            sys::transcoder_texture_format_cTFETC1_RGB => Self::Etc1Rgb,
            sys::transcoder_texture_format_cTFETC2_RGBA => Self::Etc2Rgba,
            sys::transcoder_texture_format_cTFBC1_RGB => Self::Bc1Rgb,
            sys::transcoder_texture_format_cTFBC3_RGBA => Self::Bc3Rgba,
            sys::transcoder_texture_format_cTFBC4_R => Self::Bc4R,
            sys::transcoder_texture_format_cTFBC5_RG => Self::Bc5Rg,
            sys::transcoder_texture_format_cTFBC7_RGBA => Self::Bc7Rgba,
            sys::transcoder_texture_format_cTFPVRTC1_4_RGB => Self::Pvrtc1Rgb,
            sys::transcoder_texture_format_cTFPVRTC1_4_RGBA => Self::Pvrtc1Rgba,
            sys::transcoder_texture_format_cTFASTC_4x4 => Self::AstcRgba,
            sys::transcoder_texture_format_cTFATC_RGB => Self::AtcRgb,
            sys::transcoder_texture_format_cTFATC_RGBA => Self::AtcRgbA,
            sys::transcoder_texture_format_cTFFXT1_RGB => Self::Fxt1Rgb,
            sys::transcoder_texture_format_cTFPVRTC2_4_RGB => Self::Pvrtc2Rgb,
            sys::transcoder_texture_format_cTFPVRTC2_4_RGBA => Self::Pvrtc2Rgba,
            sys::transcoder_texture_format_cTFETC2_EAC_R11 => Self::EacR11,
            sys::transcoder_texture_format_cTFETC2_EAC_RG11 => Self::EacRg11,
            sys::transcoder_texture_format_cTFRGBA32 => Self::Rgba32,
            sys::transcoder_texture_format_cTFRGB565 => Self::Rgb565,
            sys::transcoder_texture_format_cTFBGR565 => Self::Bgr565,
            sys::transcoder_texture_format_cTFRGBA4444 => Self::Rgba4444,
            _ => unreachable!("invalid internal texture format"),
        }
    }
}

#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct UserData {
    pub word0: u32,
    pub word1: u32,
}

#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct BasicImageLevelInfo {
    pub orig_width: u32,
    pub orig_height: u32,
    pub total_blocks: u32,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ImageInfo {
    pub image_index: u32,
    pub total_levels: u32,
    pub orig_width: u32,
    pub orig_height: u32,
    pub width: u32,
    pub height: u32,
    pub num_blocks_x: u32,
    pub num_blocks_y: u32,
    pub total_blocks: u32,
    pub first_slice_index: u32,
    pub alpha_flag: bool,
    pub iframe_flag: bool,
}
impl ImageInfo {
    fn from_internal(value: sys::basisu_image_info) -> Self {
        Self {
            image_index: value.m_image_index,
            total_levels: value.m_total_levels,
            orig_width: value.m_orig_width,
            orig_height: value.m_orig_height,
            width: value.m_width,
            height: value.m_height,
            num_blocks_x: value.m_num_blocks_x,
            num_blocks_y: value.m_num_blocks_y,
            total_blocks: value.m_total_blocks,
            first_slice_index: value.m_first_slice_index,
            alpha_flag: value.m_alpha_flag,
            iframe_flag: value.m_iframe_flag,
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ImageLevelInfo {
    pub image_index: u32,
    pub level_index: u32,
    pub orig_width: u32,
    pub orig_height: u32,
    pub width: u32,
    pub height: u32,
    pub num_blocks_x: u32,
    pub num_blocks_y: u32,
    pub total_blocks: u32,
    pub first_slice_index: u32,
    pub alpha_flag: bool,
    pub iframe_flag: bool,
}
impl ImageLevelInfo {
    fn from_internal(value: sys::basisu_image_level_info) -> Self {
        Self {
            image_index: value.m_image_index,
            level_index: value.m_level_index,
            orig_width: value.m_orig_width,
            orig_height: value.m_orig_height,
            width: value.m_width,
            height: value.m_height,
            num_blocks_x: value.m_num_blocks_x,
            num_blocks_y: value.m_num_blocks_y,
            total_blocks: value.m_total_blocks,
            first_slice_index: value.m_first_slice_index,
            alpha_flag: value.m_alpha_flag,
            iframe_flag: value.m_iframe_flag,
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct SliceInfo {
    pub orig_width: u32,
    pub orig_height: u32,
    pub width: u32,
    pub height: u32,
    pub num_blocks_x: u32,
    pub num_blocks_y: u32,
    pub total_blocks: u32,
    pub compressed_size: u32,
    pub slice_index: u32,
    pub image_index: u32,
    pub level_index: u32,
    pub unpacked_slice_crc16: u32,
    pub alpha_flag: bool,
    pub iframe_flag: bool,
}
impl SliceInfo {
    fn from_internal(value: sys::basisu_slice_info) -> Self {
        Self {
            image_index: value.m_image_index,
            level_index: value.m_level_index,
            orig_width: value.m_orig_width,
            orig_height: value.m_orig_height,
            width: value.m_width,
            height: value.m_height,
            num_blocks_x: value.m_num_blocks_x,
            num_blocks_y: value.m_num_blocks_y,
            total_blocks: value.m_total_blocks,
            compressed_size: value.m_compressed_size,
            alpha_flag: value.m_alpha_flag,
            iframe_flag: value.m_iframe_flag,
            slice_index: value.m_slice_index,
            unpacked_slice_crc16: value.m_unpacked_slice_crc16,
        }
    }
}

fn read_slice_info(value: sys::basisrs_vector_slice_info) -> Vec<SliceInfo> {
    let mut vec = Vec::with_capacity(value.size as _);
    for v in 0..value.size {
        let value = unsafe { *value.values.offset(v as _) };
        vec.push(SliceInfo::from_internal(value));
    }
    vec
}

fn read_mipmap_levels(value: sys::basisrs_vector_u32) -> Vec<u32> {
    let mut vec = Vec::with_capacity(value.size as _);
    for v in 0..value.size {
        let value = unsafe { *value.values.offset(v as _) };
        vec.push(value);
    }
    vec
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct FileInfo {
    pub version: u32,
    pub total_header_size: u32,
    pub total_selectors: u32,
    pub selector_codebook_size: u32,
    pub total_endpoints: u32,
    pub endpoint_codebook_size: u32,
    pub tables_size: u32,
    pub slices_size: u32,
    pub tex_type: TextureType,
    pub us_per_frame: u32,
    pub slice_info: Vec<SliceInfo>,
    pub total_images: u32,
    pub image_mipmap_levels: Vec<u32>,
    pub userdata0: u32,
    pub userdata1: u32,
    pub tex_format: BasisTextureFormat,
    pub y_flipped: bool,
    pub etc1s: bool,
    pub has_alpha_slices: bool,
}
impl FileInfo {
    fn from_internal(value: sys::basisu_file_info) -> Self {
        let slice_info = unsafe { sys::basisrs_file_info_get_slice_info(&value as *const _) };
        let mipmap_levels = unsafe { sys::basisrs_file_info_get_mipmap_levels(&value as *const _) };
        Self {
            version: value.m_version,
            total_header_size: value.m_total_header_size,
            total_selectors: value.m_total_selectors,
            selector_codebook_size: value.m_selector_codebook_size,
            total_endpoints: value.m_total_endpoints,
            endpoint_codebook_size: value.m_endpoint_codebook_size,
            tables_size: value.m_tables_size,
            slices_size: value.m_slices_size,
            tex_type: TextureType::from_internal(value.m_tex_type),
            us_per_frame: value.m_us_per_frame,
            slice_info: read_slice_info(slice_info),
            total_images: value.m_total_images,
            image_mipmap_levels: read_mipmap_levels(mipmap_levels),
            userdata0: value.m_userdata0,
            userdata1: value.m_userdata1,
            tex_format: BasisTextureFormat::from_internal(value.m_tex_format),
            y_flipped: value.m_y_flipped,
            etc1s: value.m_etc1s,
            has_alpha_slices: value.m_has_alpha_slices,
        }
    }
}

pub struct Transcoder {
    inner: *mut sys::basisu_transcoder,
}
impl Transcoder {
    pub fn new() -> Self {
        let inner = unsafe { sys::basisrs_create_transcoder() };

        Self { inner }
    }

    pub fn validate_file_checksums(&self, file: &[u8], full_validation: bool) -> Result<bool, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let res =
            unsafe { sys::basisrs_validate_file_checksums(self.inner, file.as_ptr() as _, length, full_validation) };

        Ok(res)
    }

    pub fn validate_header(&self, file: &[u8]) -> Result<bool, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let res = unsafe { sys::basisrs_validate_header(self.inner, file.as_ptr() as _, length) };

        Ok(res)
    }

    pub fn get_texture_type(&self, file: &[u8]) -> Result<TextureType, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let res = unsafe { sys::basisrs_get_texture_type(self.inner, file.as_ptr() as _, length) };

        let ty = TextureType::from_internal(res);

        Ok(ty)
    }

    pub fn get_userdata(&self, file: &[u8]) -> Result<Option<UserData>, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let mut data = UserData { word0: 0, word1: 0 };

        let res = unsafe {
            sys::basisrs_get_userdata(
                self.inner,
                file.as_ptr() as _,
                length,
                &mut data.word0 as *mut _,
                &mut data.word1 as *mut _,
            )
        };

        if res {
            Ok(Some(data))
        } else {
            Ok(None)
        }
    }

    pub fn get_total_images(&self, file: &[u8]) -> Result<NonZeroU32, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let res = unsafe { sys::basisrs_get_total_images(self.inner, file.as_ptr() as _, length) };

        Ok(NonZeroU32::new(res).expect("documentation asserts that get_total_images will return non-zero number"))
    }

    pub fn get_tex_format(&self, file: &[u8]) -> Result<BasisTextureFormat, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let res = unsafe { sys::basisrs_get_tex_format(self.inner, file.as_ptr() as _, length) };

        let ty = BasisTextureFormat::from_internal(res);

        Ok(ty)
    }

    pub fn get_total_image_levels(&self, file: &[u8], image_index: u32) -> Result<u32, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let res = unsafe { sys::basisrs_get_total_image_levels(self.inner, file.as_ptr() as _, length, image_index) };

        Ok(res)
    }

    pub fn get_basic_image_level_info(
        &self,
        file: &[u8],
        image_index: u32,
        level_index: u32,
    ) -> Result<Option<BasicImageLevelInfo>, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let mut data = BasicImageLevelInfo {
            orig_width: 0,
            orig_height: 0,
            total_blocks: 0,
        };

        let res = unsafe {
            sys::basisrs_get_image_level_desc(
                self.inner,
                file.as_ptr() as _,
                length,
                image_index,
                level_index,
                &mut data.orig_width as *mut _,
                &mut data.orig_height as *mut _,
                &mut data.total_blocks as *mut _,
            )
        };

        if res {
            Ok(Some(data))
        } else {
            Ok(None)
        }
    }

    pub fn get_image_info(&self, file: &[u8], image_index: u32) -> Result<Option<ImageInfo>, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let mut data = sys::basisu_image_info {
            m_image_index: 0,
            m_total_levels: 0,
            m_orig_width: 0,
            m_orig_height: 0,
            m_width: 0,
            m_height: 0,
            m_num_blocks_x: 0,
            m_num_blocks_y: 0,
            m_total_blocks: 0,
            m_first_slice_index: 0,
            m_alpha_flag: false,
            m_iframe_flag: false,
        };

        let res = unsafe {
            sys::basisrs_get_image_info(self.inner, file.as_ptr() as _, length, &mut data as *mut _, image_index)
        };

        if res {
            Ok(Some(ImageInfo::from_internal(data)))
        } else {
            Ok(None)
        }
    }

    pub fn get_image_level_info(
        &self,
        file: &[u8],
        image_index: u32,
        level_index: u32,
    ) -> Result<Option<ImageLevelInfo>, error::ExecutionError> {
        let length = validate_slice_length(file)?;

        let mut data = sys::basisu_image_level_info {
            m_image_index: 0,
            m_level_index: 0,
            m_orig_width: 0,
            m_orig_height: 0,
            m_width: 0,
            m_height: 0,
            m_num_blocks_x: 0,
            m_num_blocks_y: 0,
            m_total_blocks: 0,
            m_first_slice_index: 0,
            m_alpha_flag: false,
            m_iframe_flag: false,
        };

        let res = unsafe {
            sys::basisrs_get_image_level_info(
                self.inner,
                file.as_ptr() as _,
                length,
                &mut data as *mut _,
                image_index,
                level_index,
            )
        };

        if res {
            Ok(Some(ImageLevelInfo::from_internal(data)))
        } else {
            Ok(None)
        }
    }
}

impl Drop for Transcoder {
    fn drop(&mut self) {
        unsafe { sys::basisrs_destroy_transcoder(self.inner) }
    }
}

fn validate_slice_length<T>(slice: &[T]) -> Result<u32, error::ExecutionError> {
    slice
        .len()
        .try_into()
        .map_err(|_| error::ExecutionError::FileTooLong(slice.len()))
}
