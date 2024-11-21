const ifj = @import("ifj24.zig");

pub fn main() void {
    const n = ifj.readi32(); // Načítaj hornú hranicu
    var i = 1;

    while (i) |x| {
        ifj.write(i); // Vypíš aktuálnu hodnotu `i`
        i = i - 1;    // Inkrementuj `i`
        ifj.write(x);
        ifj.write("\n");
    }

    ifj.write("Koniec cyklu\n");
}
