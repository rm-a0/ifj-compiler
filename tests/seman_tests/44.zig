// 'value' is not defined outside while loop
const ifj = @import("ifj24.zig");

pub fn main() void {
    var num: ?i32 = 5;
    num = num - 1;
    while (num) |value| {
        ifj.write(value);
        num = num - 1;
    }
    ifj.write(value); // Trying to access 'value' outside loop
}
