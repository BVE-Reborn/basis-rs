use basis::TranscodeError;

fn transcode(path: &str) {
    let image = std::fs::read(path).unwrap();

    let transcoder = basis::Transcoder::new();

    assert_eq!(transcoder.get_total_images(&image).get(), 1);
    assert_eq!(transcoder.get_total_image_levels(&image, 0), 12);
    assert_eq!(transcoder.get_total_image_levels(&image, 1), 0);
    for i in 0..12 {
        let basic_level_info = transcoder.get_basic_image_level_info(&image, 0, i).unwrap();
        let size = 2048 >> i;
        assert_eq!(basic_level_info.orig_width, size);
        assert_eq!(basic_level_info.orig_height, size);
        let blocks_size = (size + 3) / 4;
        let blocks = blocks_size * blocks_size;
        assert_eq!(basic_level_info.total_blocks, blocks);
    }
    assert_eq!(transcoder.get_basic_image_level_info(&image, 0, 12), None);

    let file_format = transcoder.get_file_info(&image).unwrap().basis_format;

    let mut prepared = transcoder.prepare_transcoding(&image).unwrap();

    for format in 0..21 {
        for mip in 0..12 {
            let format = unsafe { std::mem::transmute::<u8, basis::TargetTextureFormat>(format) };

            let result = prepared.transcode_image_level(0, mip, format);

            match result {
                Err(TranscodeError::OtherError) => panic!("Unknown error!"),
                Err(e) => println!("Not transcoding because {}", e),
                Ok(_) => println!("Transcoding {:?} to {:?} mip {}", file_format, format, mip),
            }
        }
    }
}

#[test]
fn load_transcode_etc1s() {
    transcode(concat!(env!("CARGO_MANIFEST_DIR"), "/../etc/cat_etc1s.basis"));
}

#[test]
fn load_transcode_uastc() {
    transcode(concat!(env!("CARGO_MANIFEST_DIR"), "/../etc/cat_uastc.basis"));
}
