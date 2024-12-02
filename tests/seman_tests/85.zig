const ifj = @import("ifj24.zig");

pub fn main() void {
    const s1 = "Hello";
    const s2 = "World";
    const result = ifj.concat(s1, s2);
    ifj.write("Spojený reťazec: ");
    ifj.write(result);
    ifj.write("\n");
}
