// Incorrect expression evaluation in function return
const ifj = @import("ifj24.zig");

pub fn not_main() i32 {
    return 2 + 1.35;
}

pub fn main() void {

}