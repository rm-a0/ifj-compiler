const ifj = @import("ifj24.zig");

pub fn main() void {
    const a = ifj.readi32();
    const b = ifj.readi32();
    const c = ifj.readi32();
    if ((a + b) * c > 100) {
        ifj.write("Výsledok je väčší ako 100\n");
    } else {
        ifj.write("Výsledok je menší alebo rovný 100\n");
    }
}


