const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.readi32();
    const b = ifj.readi32();
    if (a > b) {
        ifj.write("a je vacsie ako b\n");
    } else {
        ifj.write("a nie je vacsie ako b\n");
    }
}
