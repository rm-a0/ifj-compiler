// No Error
const ifj = @import("ifj24.zig");

pub fn main() void {
    var num: ?i32 = null;
    if (num == null) {
        num = 2;
    }
    num = 1;
}
