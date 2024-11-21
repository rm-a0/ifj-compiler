const ifj = @import("ifj24.zig");

// Hlavní funkcia programu
pub fn main() void {
    ifj.write("Zadejte cislo pro vypocet faktorialu:\n");
    const a = ifj.readi32(); // Číta celé číslo zo štandardného vstupu
    if (a) |val| {           // Kontrola, či je čítaná hodnota rôzna od null
        if (val < 0) {
            ifj.write("Faktorial ");
            ifj.write(val);
            ifj.write(" nelze spocitat\n");
        } else {
            ifj.write(val);
            var vysledok: i32 = 1;
            var d = val;
            while (d) |i| {
                vysledok = vysledok * i;
                d = d - 1;
                ifj.write("Iterace: ");
                ifj.write(i);
                ifj.write("\n");
            }
            ifj.write("Vysledek: ");
            ifj.write(vysledok);
            ifj.write("\n");
        }
    } else {
        ifj.write("Nebyla zadana platna hodnota\n");
    }
}
