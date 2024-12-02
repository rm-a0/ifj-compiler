const ifj = @import("ifj24.zig");

pub fn main() void {
    var current : ?i32 = ifj.readi32();

    while (current) |value| {
        ifj.write("Hodnota: ");
        ifj.write(value);
        ifj.write("\n");
        current = ifj.readi32();
    }

    ifj.write("Koniec cyklu.\n");
}