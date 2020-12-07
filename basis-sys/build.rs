fn main() {
    let mut build = cc::Build::new();

    build
        .files(&["basisu/transcoder/basisu_transcoder.cpp", "src/basisrs_interface.cpp"])
        .cpp(true)
        .flag_if_supported("-std=c++14")
        .flag_if_supported("/std:c++14");

    if cfg!(debug_assertions) {
        build.define("BASISU_FORCE_DEVEL_MESSAGES", None);
    }

    build.compile("basisu")
}
