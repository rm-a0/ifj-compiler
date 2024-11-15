// Test for recursive function
const ifj = @import("ifj24.zig");

var x = 1;

pub fn main(y : i32) void {
    x = x + 1;
    main(x);
}
