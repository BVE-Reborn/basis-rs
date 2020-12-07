fn main() {
    cc::Build::new()
        .files(&["basisu/transcoder/basisu_transcoder.cpp", "src/basisrs_interface.cpp"])
        .cpp(true)
        .flag_if_supported("-std=c++14")
        .flag_if_supported("/std:c++17")
        .compile("basisu")
}
