// Error. Cannot compare 'i32' variable 'n' and 'f64' literal '1.5' without loss of precision.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var n: i32 = ifj.readi32();
    if (n > 1.5) {
        n = 2;
    }
    n = 1;
}
