// Test for recursive function
const ifj = @import("ifj24.zig");

var x = 1;


pub fn not_main(y : i32) void {
    ifj.write(y);
}

pub fn not_main2() void {
    var z = x;
    if (z < 10) {
        not_main(z);
    }
}

pub fn main() void {
    not_main2();
}