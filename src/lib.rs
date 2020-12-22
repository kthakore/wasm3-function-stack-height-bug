#![no_std]
extern crate alloc;
use arrayvec::ArrayString;
use core::fmt::Write;

extern "C" {

    fn _debug(model_idx: *const u8, model_len: u32) -> u32;

}

fn debug(s: ArrayString<[u8; 20]>) -> u32 {
    unsafe { return _debug(s.as_bytes().as_ptr(), s.len() as u32) }
}

#[no_mangle]
pub extern "C" fn _call(a: i32, b: i32) -> i32 {
    let mut buf = ArrayString::<[u8; 20]>::new();
    write!(&mut buf, "c={}", a + b).expect("Failed to make arrayString");

    debug(buf);

    return 0;
}
