const ifj = @import("ifj24.zig");

pub fn main() void {
    var n: i32 = ifj.readi32();

    ifj.write("Začiatok cyklu:\n");

    while (n > 0) {
        const square = n * n;
        ifj.write("Číslo: ");
        ifj.write(n);
        ifj.write(", Druhá mocnina: ");
        ifj.write(square);
        ifj.write("\n");
        n = n - 1;
    }

    ifj.write("Koniec cyklu.\n");
}
