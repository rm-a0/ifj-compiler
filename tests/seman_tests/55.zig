// No Error

const ifj = @import("ifj24.zig");

pub fn main() void {
    var numerator: i32 = 10;
    var denominator: i32 = 3;
    var result: i32 = numerator / denominator;
    numerator = 1;
    denominator = 1;
    result = 1;
}
