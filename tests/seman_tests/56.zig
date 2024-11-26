// No Error

const ifj = @import("ifj24.zig");

pub fn main() void {
    var numerator: f64 = 10.0;
    var denominator: f64 = 3.0;
    var result: f64 = numerator / denominator;
    numerator = 1;
    denominator = 1;
    result = 1;
}
