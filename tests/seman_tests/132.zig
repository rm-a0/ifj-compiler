const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.readi32();
    var x : i32 = a;
    if (x > 0) {
        ifj.write("Cislo je kladne.\n");
    } else {
        ifj.write("Cislo je zaporne alebo nula.\n");
    }
    x = 3;
}
