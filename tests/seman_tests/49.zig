// No Error. Test for nullable assignment
const ifj = @import("ifj24.zig");

pub fn main() void {
    var test : ?i32 = null;
    test = 1;
}