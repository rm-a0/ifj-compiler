// Variable set to null but its specified before
const ifj = @import("ifj24.zig");

pub fn main() ?i32 {
    var x : ?i32 = null; // Cannot infer type from `null`
    return x;
}