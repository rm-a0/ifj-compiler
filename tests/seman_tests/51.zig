// Error. Test for expression with null
const ifj = @import("ifj24.zig");

pub fn main() void {
    var test : ?i32 = 1 + null;
}
