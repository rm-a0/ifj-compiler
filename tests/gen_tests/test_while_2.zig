const ifj = @import("ifj24.zig");

pub fn main() void {
    const n = ifj.readi32(); // Načítanie čísla `n`
    var sum = 0;
    var product = 1;

    var i: i32 = 1;
    while (i < n + 1) {
        ifj.write("Aktuálne číslo: ");
        ifj.write(i);
        ifj.write("\n");

        // Sčítanie do sum
        sum = sum + i;

        // Násobenie do product
        product = product * i;

        i = i + 1;
    }

    ifj.write("Súčet: ");
    ifj.write(sum);
    ifj.write("\n");

    ifj.write("Súčin: ");
    ifj.write(product);
    ifj.write("\n");

    ifj.write("Koniec programu\n");
}
