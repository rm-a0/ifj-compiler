// Variable set to null but its specified before
const ifj = @import("ifj24.zig");

pub fn not_main() ?i32 {
    var x : ?i32 = null; // Cannot infer type from `null`
    x = 3;
    return x;
}

pub fn main() void {
    var c = not_main();
    ifj.write(c);
    c = 2;
}
