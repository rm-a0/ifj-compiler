// Correct expression evaluation
const ifj = @import("ifj24.zig");

pub fn not_main () i32 {
    return 3 + 4 + 5;
}

pub fn main () void {
    var x = not_main();
    ifj.write(x);
    x = 2;
}