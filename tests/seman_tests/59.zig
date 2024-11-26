// Operator '+' cannot be applied to slices.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var arr: []u8 = "Hello";
    var result = arr + arr;
    arr = 'a';
    result = 'a';
}
