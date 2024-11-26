// No Error
const ifj = @import("ifj24.zig");

pub fn main() void {
    var x: f64 = 20.0;
    if (x < 25) {
        x = 2;
    }
    x = 1;
}
