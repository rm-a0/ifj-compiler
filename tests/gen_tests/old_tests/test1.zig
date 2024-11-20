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
            var vysledok: i32 = 1;
            var d = val;
            ifj.write("Vysledek: ");
            ifj.write(vysledok);
            ifj.write("\n");
        }
    } else {
        ifj.write("Nebyla zadana platna hodnota\n");
    }
}
