use criterion::{criterion_group, criterion_main, Criterion, Throughput};
use std::time::Duration;

pub fn inner(c: &mut Criterion, path: &str) {
    let file = std::fs::read(path).unwrap();

    let transcoder = basis::Transcoder::new();
    let image_info = transcoder.get_image_info(&file, 0).unwrap();
    let basis = transcoder.get_file_info(&file).unwrap().basis_format;

    let mut group = c.benchmark_group(format!("{:?}", basis));

    let mut prepared = transcoder.prepare_transcoding(&file).unwrap();
    for format in 0..20 {
        let format = unsafe { std::mem::transmute::<u8, basis::TargetTextureFormat>(format) };

        if !basis.supports_texture_format(format) {
            continue;
        }

        group.throughput(Throughput::Elements(
            (image_info.orig_width * image_info.orig_height) as u64,
        ));
        group.bench_function(format!("{:?} => {:?}", basis, format), |b| {
            b.iter(|| prepared.transcode_image_level(0, 0, format).unwrap())
        });
    }

    drop(prepared);
    group.finish();
}

pub fn transcode(c: &mut Criterion) {
    inner(c, concat!(env!("CARGO_MANIFEST_DIR"), "/../etc/cat_etc1s.basis"));
    inner(c, concat!(env!("CARGO_MANIFEST_DIR"), "/../etc/cat_uastc.basis"));
}
criterion_group! {
    name = benches;
    // This can be any expression that returns a `Criterion` object.
    config = Criterion::default().measurement_time(Duration::new(10, 0));
    targets = transcode
}
criterion_main!(benches);
