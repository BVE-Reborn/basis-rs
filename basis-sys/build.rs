fn main() {
    cc::Build::new().files(&["basisu/transcoder/basisu_transcoder.cpp", "src/basisrs_interface.cpp"]).cpp(true).compile("basisu")
}