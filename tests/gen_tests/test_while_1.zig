const ifj = @import("ifj24.zig");

pub fn main() void {
    const n = ifj.readi32(); // Načítaj hornú hranicu
    var i = 0;

    while (i < n) {
        ifj.write(i); // Vypíš aktuálnu hodnotu `i`
        i = i + 1;    // Inkrementuj `i`
    }

    ifj.write("Koniec cyklu\n");
}
