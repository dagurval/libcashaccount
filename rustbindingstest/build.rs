extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    //println!("cargo:rustc-link-lib=static=cashaccount");
    println!("cargo:rustc-link-lib=cashaccount");
    println!("cargo:rustc-link-search=native=../src");

    let bindings = bindgen::Builder::default()
        .header("../includes/cashaccount.hpp")
        //.trust_clang_mangling(false)
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
