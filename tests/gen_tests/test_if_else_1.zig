const ifj = @import("ifj24.zig");

pub fn main() void {
    const a: i32 = 5;
    if (a > 0) {
        ifj.write("Pozitivne cislo\n");
    } else {
        ifj.write("Negativne cislo\n");
    }
}

