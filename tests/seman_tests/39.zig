// Should crash on min size allowed for int32
const ifj = @import("ifj24.zig");

pub fn main() void {
    const x : i32 = -2 147 483 648;
    x = x - 1;
    ifj.write(x);
}
