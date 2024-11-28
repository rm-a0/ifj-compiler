// Shouldn't crash, valid code.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var y : i32 = 5 + 6;
    y = y + 1;
    ifj.write(y);
}
