// Correct return expression
const ifj = @import("ifj24.zig");

pub fn not_main() []u8 {
    return "ahoj";
}

pub fn main() void {
    const greeting = not_main();
    ifj.write(greeting);
}