// Error. Cannot compare non-nullable type 'i32' to 'null'.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var num: i32 = 10;
    if (num == null) {
        num = 2;
    }
    num = 1;
}
