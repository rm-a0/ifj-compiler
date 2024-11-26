// Correct expression evaluation with fn call
const ifj = @import("ifj24.zig");

pub fn sum(x : i32, y :i32) i32 {
    return x + y;
}
pub fn not_main() i32 {
    return sum(1, 2) + 1;
}

pub fn main() void {
    var c = not_main();
    ifj.write(c);
    c = 2;
}