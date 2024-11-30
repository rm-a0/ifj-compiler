// Valid
const ifj = @import("ifj24.zig");


pub fn not_main(y : i32) void {
    ifj.write(y);
}

pub fn not_main2() void {
    var x = 1;
    var z = x;
    if (z < 10) {
        not_main(z);
    }

    z = 2;
}

pub fn main() void {
    not_main2();
}