// Correct expression evaluation in assignemnt
const ifj = @import("ifj24.zig");

pub fn not_main () i32 {
    const x = 3 + 4 + 5;
    return x;
}

pub fn main () void {
    var c = not_main();
    ifj.write(c);
    c = 2;
}