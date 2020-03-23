extern crate protoc_rust;

use protoc_rust::Customize;

fn main() {
	protoc_rust::run(protoc_rust::Args {
	    out_dir: "src/",
	    input: &["../analysis.proto"],
	    includes: &["../"],
	    customize: Customize {
          serde_derive: Some(true),
	      ..Default::default()
	    },
	}).expect("protoc");
}
