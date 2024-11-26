// Should crash on max size allowed for int32
const ifj = @import("ifj24.zig");

pub fn main() void {
    var y : i32 = 2147483648;
    y = y + 1;
    ifj.write(y);
}
