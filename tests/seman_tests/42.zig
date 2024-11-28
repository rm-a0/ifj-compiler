// 'value' is defined outside if block
const ifj = @import("ifj24.zig");

pub fn main() void {
    var num: ?i32 = 5;
    num = num - 1;
    if (num) |value| {
        ifj.write("Inside if: ");
        ifj.write(value);
    }
    ifj.write("Outside if: ");
    ifj.write(value); // Error
}
