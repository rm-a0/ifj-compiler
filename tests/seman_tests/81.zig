const ifj = @import("ifj24.zig");

pub fn main() void {
    const n = ifj.readi32();
    const remainder : ?i32 = n - (2 * (n / 2)); // Ekvivalent n % 2
    var x = 2;
    if (remainder) |result| {
        ifj.write("Cislo je neparne.\n");
        ifj.write("Zvysok po deleni je: ");
        ifj.write(result);
        ifj.write("\n");
    } else {
        ifj.write("Cislo je parne.\n");
    }

    if (remainder) |result| {
        if (remainder) |result| {
            if (remainder) |result| {
                if (remainder) |result| {
                    x = 3;
                }
            }
        }
    }
}
