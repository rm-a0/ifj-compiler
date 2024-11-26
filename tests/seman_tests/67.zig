// No Error
const ifj = @import("ifj24.zig");

pub fn main() void {
    var num1: i32 = 10;
    var num2: ?i32 = 10;
    if (num1 == num2) {
        num1 = 3;
    }
    num1 = 1;
    num2 = 2;
}
