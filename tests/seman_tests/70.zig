// Error. Cannot use relational operator '>' with nullable type '?i32'.
const ifj = @import("ifj24.zig");

pub fn main() void {
    var num: ?i32 = null;
    if (num > 0) {
        num = 2;
    }
    num = 1;
}
