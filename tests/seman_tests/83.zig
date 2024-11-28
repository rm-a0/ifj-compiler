const ifj = @import("ifj24.zig");

pub fn main() void {
    var n = ifj.readi32();

    while (n) |value| {
        ifj.write("Prvá iterácia, hodnota: ");
        ifj.write(value);
        ifj.write("\n");
        n = n - 1;
    }

    var m = ifj.readi32();

    while (m) |value| {
        ifj.write("Druhá iterácia, hodnota: ");
        ifj.write(value);
        ifj.write("\n");
        m = m - 1;
    }
}