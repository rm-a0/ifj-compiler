// No Error
const ifj = @import("ifj24.zig");

pub fn main() void {
    _ = ifj.readi32(); // Discarding the return value
    _ = 5 + 3;         // Discarding the result of an expression
}
