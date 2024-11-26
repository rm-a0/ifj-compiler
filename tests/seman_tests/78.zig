// Error. Attempt to shadow pseudovariable '_'
const ifj = @import("ifj24.zig");

pub fn main() void {
    var _: i32 = 5; // Attempt to define '_' as a regular variable
}
