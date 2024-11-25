// Undefined variable set to null
const ifj = @import("ifj24.zig");

pub fn not_main() ?i32 {
    var x = null; // Cannot infer type from `null`
    return x;
}

pub fn main() void {
    var c = not_main();
    ifj.write(c);
}