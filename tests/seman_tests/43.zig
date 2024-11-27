// Correct
const ifj = @import("ifj24.zig");

pub fn main() void {
    var value: i32 = 10;
    var num: ?i32 = 5;
    num = num - 1;
    if (num) |value| {
        ifj.write("Number is ");
        ifj.write(value);
    } else {
        ifj.write("Number is null");
    }
    ifj.write(value);
    value = value + 1;
}
