// Test for missing main function
const ifj = @import("ifj24.zig");

// No main function
pub fn not_main() void {
    const x = ifj.string("ahoj");
    ifj.write(x);
}
