// Variable initialized but not modified
const ifj = @import("ifj24.zig");

pub fn main () void {
    var x = 3;
    ifj.write(x);
    return x;
}