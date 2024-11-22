// Unused global variable
const ifj = @import("ifj24.zig");

var x : i32 = 5;

pub fn main () void {
    const y : i32 = 5;
    ifj.write(y);
}