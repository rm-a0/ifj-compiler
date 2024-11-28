// Error with using value before decl
const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write(value); // Trying to use 'value' before declaration
    var num: ?i32 = 5;
    num = num - 1;
    if (num) |value| {
        ifj.write(value);
    } else {
        ifj.write("Number is null");
    }
}