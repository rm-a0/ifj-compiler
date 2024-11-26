// No Error.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var y: f64 = 3.5;
    var result: f64 = 2 * y;
    y = 1.0;
    result = 2.0;
}
