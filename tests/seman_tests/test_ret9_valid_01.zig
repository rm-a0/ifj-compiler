// Defined and used variable
const ifj = @import("ifj24.zig");

pub fn main() void {
    var x: i32 = 10;
    x = x + 1;
    ifj.write(x);
    return;
}