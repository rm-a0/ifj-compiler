const ifj = @import("ifj24.zig");

pub fn main() void {
    const a : ?i32 = ifj.readi32();
    const b : ?i32 = ifj.readi32();

    if (a) |value| {
        ifj.write("Prve cislo nie je null: ");
        ifj.write(value);
        ifj.write("\n");
    } else {
        ifj.write("Prve cislo je null.\n");
    }

    if (b) |value| {
        ifj.write("Druhe cislo nie je null: ");
        ifj.write(value);
        ifj.write("\n");
    } else {
        ifj.write("Druhe cislo je null.\n");
    }
}