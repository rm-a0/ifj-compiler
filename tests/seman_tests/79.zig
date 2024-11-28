// No Error. Implicit conversion
const ifj = @import("ifj24.zig");

pub fn main() void {
    var x: ?f64 = 10.0;
    if (x == 10) {
        x = 2;
    }
    x = null;
}
