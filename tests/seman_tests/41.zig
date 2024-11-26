// cannot pass non nullable to expression
const ifj = @import("ifj24.zig");

pub fn main() void {
    var num: i32 = 10;
    if (num) |value| {  // Here
        ifj.write("Number is ");
        ifj.write(value);
    } else {
        ifj.write("Number is zero");
    }
}
