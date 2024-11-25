// Incorrect expression in return statement
const ifj = @import("ifj24.zig");

pub fn test() f64 {
    return 4.0;
}

pub fn main() f64 {
    var x = test();
    ifj.write(x);
    return;
}