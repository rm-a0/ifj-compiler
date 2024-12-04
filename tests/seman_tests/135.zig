const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.readi32();
    if (a == 0) {
        ifj.write("Cislo je nula\n");
    } else {
        if (a > 0) {
            ifj.write("Cislo je kladne\n");
        } else {
            ifj.write("Cislo je zaporne\n");
        }
    }
}
