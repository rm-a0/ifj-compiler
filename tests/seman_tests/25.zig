// Incorrect expression evaluation with string
const ifj = @import("ifj24.zig");

pub fn main() void {
    const x = "ahoj" + 3;
    ifj.write(x);
    return;
}