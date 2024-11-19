// Test for recursive function, fail due to the param in main func.
const ifj = @import("ifj24.zig");

var x = 1;

pub fn main(y : i32) void {
    var z = x + y;
    if (z < 10) {
        main(z);
    }
}
