// Test for using undefined variable
const ifj = @import("ifj24.zig");

pub fn main() void {
    ifj.write(str1);    // str1 is undefined
}