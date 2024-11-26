// Should crash on min size allowed for int32
const ifj = @import("ifj24.zig");

pub fn main() void {
    const x : i32 = -2147483649;
    x = x - 1;
    ifj.write(x);
}
